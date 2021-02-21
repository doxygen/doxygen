/*****************************************************************************
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

#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#include <mutex>
#include <unordered_set>
#include <regex>
#include <codecvt>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <cinttypes>

#include "md5.h"

#include <qfileinfo.h>
#include <qdir.h>

#include "util.h"
#include "message.h"
#include "classdef.h"
#include "filedef.h"
#include "doxygen.h"
#include "outputlist.h"
#include "defargs.h"
#include "language.h"
#include "config.h"
#include "htmlhelp.h"
#include "example.h"
#include "version.h"
#include "groupdef.h"
#include "reflist.h"
#include "pagedef.h"
#include "debug.h"
#include "searchindex.h"
#include "doxygen.h"
#include "textdocvisitor.h"
#include "latexdocvisitor.h"
#include "portable.h"
#include "parserintf.h"
#include "bufstr.h"
#include "image.h"
#include "growbuf.h"
#include "entry.h"
#include "arguments.h"
#include "memberlist.h"
#include "classlist.h"
#include "namespacedef.h"
#include "membername.h"
#include "filename.h"
#include "membergroup.h"
#include "dirdef.h"
#include "htmlentity.h"
#include "symbolresolver.h"

#define ENABLE_TRACINGSUPPORT 0

#if defined(_OS_MAC_) && ENABLE_TRACINGSUPPORT
#define TRACINGSUPPORT
#endif

#ifdef TRACINGSUPPORT
#include <execinfo.h>
#include <unistd.h>
#endif


//------------------------------------------------------------------------

#define REL_PATH_TO_ROOT "../../"

static const char *hex = "0123456789ABCDEF";

//------------------------------------------------------------------------
// TextGeneratorOLImpl implementation
//------------------------------------------------------------------------

TextGeneratorOLImpl::TextGeneratorOLImpl(OutputDocInterface &od) : m_od(od)
{
}

void TextGeneratorOLImpl::writeString(const char *s,bool keepSpaces) const
{
  if (s==0) return;
  //printf("TextGeneratorOlImpl::writeString('%s',%d)\n",s,keepSpaces);
  if (keepSpaces)
  {
    const char *p=s;
    if (p)
    {
      char cs[2];
      char c;
      cs[1]='\0';
      while ((c=*p++))
      {
        if (c==' ') m_od.writeNonBreakableSpace(1);
        else cs[0]=c,m_od.docify(cs);
      }
    }
  }
  else
  {
    m_od.docify(s);
  }
}

void TextGeneratorOLImpl::writeBreak(int indent) const
{
  m_od.lineBreak("typebreak");
  int i;
  for (i=0;i<indent;i++)
  {
    m_od.writeNonBreakableSpace(3);
  }
}

void TextGeneratorOLImpl::writeLink(const char *extRef,const char *file,
                                    const char *anchor,const char *text
                                   ) const
{
  //printf("TextGeneratorOlImpl::writeLink('%s')\n",text);
  m_od.writeObjectLink(extRef,file,anchor,text);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------

// an inheritance tree of depth of 100000 should be enough for everyone :-)
const int maxInheritanceDepth = 100000;

/*!
  Removes all anonymous scopes from string s
  Possible examples:
\verbatim
   "bla::@10::blep"      => "bla::blep"
   "bla::@10::@11::blep" => "bla::blep"
   "@10::blep"           => "blep"
   " @10::blep"          => "blep"
   "@9::@10::blep"       => "blep"
   "bla::@1"             => "bla"
   "bla::@1::@2"         => "bla"
   "bla @1"              => "bla"
\endverbatim
 */
QCString removeAnonymousScopes(const char *str)
{
  std::string result;
  if (str==0) return result;

  // helper to check if the found delimiter starts with a colon
  auto startsWithColon = [](const std::string &del)
  {
    for (size_t i=0;i<del.size();i++)
    {
      if (del[i]=='@') return false;
      else if (del[i]==':') return true;
    }
    return false;
  };

  // helper to check if the found delimiter ends with a colon
  auto endsWithColon = [](const std::string &del)
  {
    for (int i=(int)del.size()-1;i>=0;i--)
    {
      if (del[i]=='@') return false;
      else if (del[i]==':') return true;
    }
    return false;
  };

  static std::regex re("[ :]*@[[:digit:]]+[: ]*");
  std::string s = str;
  std::sregex_iterator iter( s.begin(), s.end(), re);
  std::sregex_iterator end;
  size_t p=0;
  size_t sl=s.length();
  bool needsSeparator=false;
  for ( ; iter!=end ; ++iter)
  {
    const auto &match = *iter;
    size_t i = match.position();
    if (i>p) // add non-matching prefix
    {
      if (needsSeparator) result+="::";
      needsSeparator=false;
      result+=s.substr(p,i-p);
    }
    std::string delim = match.str();
    needsSeparator = needsSeparator || (startsWithColon(delim) && endsWithColon(delim));
    p = match.position()+match.length();
  }
  if (p<sl) // add trailing remainder
  {
    if (needsSeparator) result+="::";
    result+=s.substr(p);
  }
  return result;
}

// replace anonymous scopes with __anonymous__ or replacement if provided
QCString replaceAnonymousScopes(const char *s,const char *replacement)
{
  if (s==0) return QCString();
  static std::regex marker("@[[:digit:]]+");
  std::string result = std::regex_replace(s,marker,replacement?replacement:"__anonymous__");
  //printf("replaceAnonymousScopes('%s')='%s'\n",s.data(),result.data());
  return result;
}


// strip anonymous left hand side part of the scope
QCString stripAnonymousNamespaceScope(const QCString &s)
{
  int i,p=0,l;
  QCString newScope;
  int sl = s.length();
  while ((i=getScopeFragment(s,p,&l))!=-1)
  {
    //printf("Scope fragment %s\n",s.mid(i,l).data());
    if (Doxygen::namespaceLinkedMap->find(s.left(i+l))!=0)
    {
      if (s.at(i)!='@')
      {
        if (!newScope.isEmpty()) newScope+="::";
        newScope+=s.mid(i,l);
      }
    }
    else if (i<sl)
    {
      if (!newScope.isEmpty()) newScope+="::";
      newScope+=s.right(sl-i);
      goto done;
    }
    p=i+l;
  }
done:
  //printf("stripAnonymousNamespaceScope('%s')='%s'\n",s.data(),newScope.data());
  return newScope;
}

void writePageRef(OutputDocInterface &od,const char *cn,const char *mn)
{
  od.pushGeneratorState();

  od.disable(OutputGenerator::Html);
  od.disable(OutputGenerator::Man);
  od.disable(OutputGenerator::Docbook);
  if (Config_getBool(PDF_HYPERLINKS)) od.disable(OutputGenerator::Latex);
  if (Config_getBool(RTF_HYPERLINKS)) od.disable(OutputGenerator::RTF);
  od.startPageRef();
  od.docify(theTranslator->trPageAbbreviation());
  od.endPageRef(cn,mn);

  od.popGeneratorState();
}

/*! Generate a place holder for a position in a list. Used for
 *  translators to be able to specify different elements orders
 *  depending on whether text flows from left to right or visa versa.
 */
QCString generateMarker(int id)
{
  const int maxMarkerStrLen = 20;
  char result[maxMarkerStrLen];
  qsnprintf(result,maxMarkerStrLen,"@%d",id);
  return result;
}

static QCString stripFromPath(const QCString &path,const StringVector &l)
{
  // look at all the strings in the list and strip the longest match
  QCString potential;
  unsigned int length = 0;
  for (const auto &s : l)
  {
    QCString prefix = s.c_str();
    if (prefix.length() > length &&
        qstricmp(path.left(prefix.length()),prefix)==0) // case insensitive compare
    {
      length = prefix.length();
      potential = path.right(path.length()-prefix.length());
    }
  }
  if (length) return potential;
  return path;
}

/*! strip part of \a path if it matches
 *  one of the paths in the Config_getList(STRIP_FROM_PATH) list
 */
QCString stripFromPath(const QCString &path)
{
  return stripFromPath(path,Config_getList(STRIP_FROM_PATH));
}

/*! strip part of \a path if it matches
 *  one of the paths in the Config_getList(INCLUDE_PATH) list
 */
QCString stripFromIncludePath(const QCString &path)
{
  return stripFromPath(path,Config_getList(STRIP_FROM_INC_PATH));
}

/*! try to determine if \a name is a source or a header file name by looking
 * at the extension. A number of variations is allowed in both upper and
 * lower case) If anyone knows or uses another extension please let me know :-)
 */
int guessSection(const char *name)
{
  QCString n=((QCString)name).lower();
  if (n.right(2)==".c"    || // source
      n.right(3)==".cc"   ||
      n.right(4)==".cxx"  ||
      n.right(4)==".cpp"  ||
      n.right(4)==".c++"  ||
      n.right(5)==".java" ||
      n.right(2)==".m"    ||
      n.right(3)==".mm"   ||
      n.right(3)==".ii"   || // inline
      n.right(4)==".ixx"  ||
      n.right(4)==".ipp"  ||
      n.right(4)==".i++"  ||
      n.right(4)==".inl"  ||
      n.right(4)==".xml"  ||
      n.right(4)==".sql"
     ) return Entry::SOURCE_SEC;
  if (n.right(2)==".h"    || // header
      n.right(3)==".hh"   ||
      n.right(4)==".hxx"  ||
      n.right(4)==".hpp"  ||
      n.right(4)==".h++"  ||
      n.right(4)==".idl"  ||
      n.right(4)==".ddl"  ||
      n.right(5)==".pidl" ||
      n.right(4)==".ice"
     ) return Entry::HEADER_SEC;
  return 0;
}

QCString resolveTypeDef(const Definition *context,const QCString &qualifiedName,
                        const Definition **typedefContext)
{
  //printf("<<resolveTypeDef(%s,%s)\n",
  //          context ? context->name().data() : "<none>",qualifiedName.data());
  QCString result;
  if (qualifiedName.isEmpty())
  {
    //printf("  qualified name empty!\n");
    return result;
  }

  const Definition *mContext=context;
  if (typedefContext) *typedefContext=context;

  // see if the qualified name has a scope part
  int scopeIndex = qualifiedName.findRev("::");
  QCString resName=qualifiedName;
  if (scopeIndex!=-1) // strip scope part for the name
  {
    resName=qualifiedName.right(qualifiedName.length()-scopeIndex-2);
    if (resName.isEmpty())
    {
      // qualifiedName was of form A:: !
      //printf("  qualified name of form A::!\n");
      return result;
    }
  }
  const MemberDef *md=0;
  while (mContext && md==0)
  {
    // step 1: get the right scope
    const Definition *resScope=mContext;
    if (scopeIndex!=-1)
    {
      // split-off scope part
      QCString resScopeName = qualifiedName.left(scopeIndex);
      //printf("resScopeName='%s'\n",resScopeName.data());

      // look-up scope in context
      int is,ps=0;
      int l;
      while ((is=getScopeFragment(resScopeName,ps,&l))!=-1)
      {
        QCString qualScopePart = resScopeName.mid(is,l);
        QCString tmp = resolveTypeDef(mContext,qualScopePart);
        if (!tmp.isEmpty()) qualScopePart=tmp;
        resScope = resScope->findInnerCompound(qualScopePart);
        //printf("qualScopePart='%s' resScope=%p\n",qualScopePart.data(),resScope);
        if (resScope==0) break;
        ps=is+l;
      }
    }
    //printf("resScope=%s\n",resScope?resScope->name().data():"<none>");

    // step 2: get the member
    if (resScope) // no scope or scope found in the current context
    {
      //printf("scope found: %s, look for typedef %s\n",
      //     resScope->qualifiedName().data(),resName.data());
      MemberNameLinkedMap *mnd=0;
      if (resScope->definitionType()==Definition::TypeClass)
      {
        mnd=Doxygen::memberNameLinkedMap;
      }
      else
      {
        mnd=Doxygen::functionNameLinkedMap;
      }
      MemberName *mn=mnd->find(resName);
      if (mn)
      {
        int minDist=-1;
        for (const auto &tmd_p : *mn)
        {
          const MemberDef *tmd = tmd_p.get();
          //printf("Found member %s resScope=%s outerScope=%s mContext=%p\n",
          //    tmd->name().data(), resScope->name().data(),
          //    tmd->getOuterScope()->name().data(), mContext);
          if (tmd->isTypedef() /*&& tmd->getOuterScope()==resScope*/)
          {
            SymbolResolver resolver;
            int dist=resolver.isAccessibleFrom(resScope,tmd);
            if (dist!=-1 && (md==0 || dist<minDist))
            {
              md = tmd;
              minDist = dist;
            }
          }
        }
      }
    }
    mContext=mContext->getOuterScope();
  }

  // step 3: get the member's type
  if (md)
  {
    //printf(">>resolveTypeDef: Found typedef name '%s' in scope '%s' value='%s' args='%s'\n",
    //    qualifiedName.data(),context->name().data(),md->typeString(),md->argsString()
    //    );
    result=md->typeString();
    QCString args = md->argsString();
    if (args.find(")(")!=-1) // typedef of a function/member pointer
    {
      result+=args;
    }
    else if (args.find('[')!=-1) // typedef of an array
    {
      result+=args;
    }
    if (typedefContext) *typedefContext=md->getOuterScope();
  }
  else
  {
    //printf(">>resolveTypeDef: Typedef '%s' not found in scope '%s'!\n",
    //    qualifiedName.data(),context ? context->name().data() : "<global>");
  }
  return result;

}


/*! Get a class definition given its name.
 *  Returns 0 if the class is not found.
 */
ClassDef *getClass(const char *n)
{
  if (n==0 || n[0]=='\0') return 0;
  return Doxygen::classLinkedMap->find(n);
}

NamespaceDef *getResolvedNamespace(const char *name)
{
  if (name==0 || name[0]=='\0') return 0;
  auto it = Doxygen::namespaceAliasMap.find(name);
  if (it!=Doxygen::namespaceAliasMap.end())
  {
    int count=0; // recursion detection guard
    StringUnorderedMap::iterator it2;
    while ((it2=Doxygen::namespaceAliasMap.find(it->second))!=Doxygen::namespaceAliasMap.end() &&
           count<10)
    {
      it=it2;
      count++;
    }
    if (count==10)
    {
      warn_uncond("possible recursive namespace alias detected for %s!\n",name);
    }
    return Doxygen::namespaceLinkedMap->find(it->second.data());
  }
  else
  {
    return Doxygen::namespaceLinkedMap->find(name);
  }
}

int computeQualifiedIndex(const QCString &name)
{
  int i = name.find('<');
  return name.findRev("::",i==-1 ? name.length() : i);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

static const char constScope[]    = { 'c', 'o', 'n', 's', 't', ':' };
static const char virtualScope[]  = { 'v', 'i', 'r', 't', 'u', 'a', 'l', ':' };
static const char operatorScope[] = { 'o', 'p', 'e', 'r', 'a', 't', 'o', 'r', '?', '?', '?' };

struct CharAroundSpace
{
  CharAroundSpace()
  {
    charMap[static_cast<int>('(')].before=FALSE;
    charMap[static_cast<int>('=')].before=FALSE;
    charMap[static_cast<int>('&')].before=FALSE;
    charMap[static_cast<int>('*')].before=FALSE;
    charMap[static_cast<int>('[')].before=FALSE;
    charMap[static_cast<int>('|')].before=FALSE;
    charMap[static_cast<int>('+')].before=FALSE;
    charMap[static_cast<int>(';')].before=FALSE;
    charMap[static_cast<int>(':')].before=FALSE;
    charMap[static_cast<int>('/')].before=FALSE;

    charMap[static_cast<int>('=')].after=FALSE;
    charMap[static_cast<int>(' ')].after=FALSE;
    charMap[static_cast<int>('[')].after=FALSE;
    charMap[static_cast<int>(']')].after=FALSE;
    charMap[static_cast<int>('\t')].after=FALSE;
    charMap[static_cast<int>('\n')].after=FALSE;
    charMap[static_cast<int>(')')].after=FALSE;
    charMap[static_cast<int>(',')].after=FALSE;
    charMap[static_cast<int>('<')].after=FALSE;
    charMap[static_cast<int>('|')].after=FALSE;
    charMap[static_cast<int>('+')].after=FALSE;
    charMap[static_cast<int>('(')].after=FALSE;
    charMap[static_cast<int>('/')].after=FALSE;
  }
  struct CharElem
  {
    CharElem() : before(TRUE), after(TRUE) {}
    bool before;
    bool after;
  };

  CharElem charMap[256];
};

static CharAroundSpace g_charAroundSpace;

// Note: this function is not reentrant due to the use of static buffer!
QCString removeRedundantWhiteSpace(const QCString &s)
{
  bool cliSupport = Config_getBool(CPP_CLI_SUPPORT);
  bool vhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);

  if (s.isEmpty() || vhdl) return s;

  // We use a static character array to
  // improve the performance of this function
  // and thread_local is needed to make it multi-thread safe
  static THREAD_LOCAL char *growBuf = 0;
  static THREAD_LOCAL int growBufLen = 0;
  if ((int)s.length()*3>growBufLen) // For input character we produce at most 3 output characters,
  {
    growBufLen = s.length()*3;
    growBuf = (char *)realloc(growBuf,growBufLen+1); // add 1 for 0-terminator
  }
  if (growBuf==0) return s; // should not happen, only we run out of memory

  char *src=s.rawData();
  char *dst=growBuf;

  uint i=0;
  uint l=s.length();
  uint csp=0;
  uint vsp=0;
  uint osp=0;
  char c;
  char pc=0;
  // skip leading whitespace
  while (i<l && isspace((uchar)src[i]))
  {
    i++;
  }
  for (;i<l;i++)
  {
    c=src[i];
    char nc=i<l-1 ? src[i+1] : ' ';

    // search for "const"
    if (csp<6 && c==constScope[csp] && // character matches substring "const"
         (csp>0 ||                     // inside search string
          i==0  ||                     // if it is the first character
          !isId(pc)                    // the previous may not be a digit
         )
       )
      csp++;
    else // reset counter
      csp=0;

    // search for "virtual"
    if (vsp<8 && c==virtualScope[vsp] && // character matches substring "virtual"
         (vsp>0 ||                       // inside search string
          i==0  ||                       // if it is the first character
          !isId(pc)                      // the previous may not be a digit
         )
       )
      vsp++;
    else // reset counter
      vsp=0;

    // search for "operator"
    if (osp<11 && (osp>=8 || c==operatorScope[osp]) && // character matches substring "operator" followed by 3 arbitrary characters
        (osp>0 ||                         // inside search string
         i==0 ||                          // if it is the first character
         !isId(pc)                        // the previous may not be a digit
        )
       )
      osp++;
    else // reset counter
      osp=0;

    switch(c)
    {
      case '"': // quoted string
        {
          *dst++=c;
          pc = c;
          i++;
          for (;i<l;i++) // find end of string
          {
            c = src[i];
            *dst++=c;
            if (c=='\\' && i+1<l)
            {
              pc = c;
              i++;
              c = src[i];
              *dst++=c;
            }
            else if (c=='"')
            {
              break;
            }
            pc = c;
          }
        }
        break;
      case '<': // current char is a <
        *dst++=c;
        if (i<l-1 &&
            (isId(nc)) && // next char is an id char
            (osp<8) // string in front is not "operator"
           )
        {
          *dst++=' '; // add extra space
        }
        break;
      case '>': // current char is a >
        if (i>0 && !isspace((uchar)pc) &&
            (isId(pc) || pc=='*' || pc=='&' || pc=='.') && // prev char is an id char or space or *&.
            (osp<8 || (osp==8 && pc!='-')) // string in front is not "operator>" or "operator->"
           )
        {
          *dst++=' '; // add extra space in front
        }
        *dst++=c;
        if (i<l-1 && (nc=='-' || nc=='&')) // '>-' -> '> -'
        {
          *dst++=' '; // add extra space after
        }
        break;
      case ',': // current char is a ,
        *dst++=c;
        if (i>0 && !isspace((uchar)pc) &&
            ((i<l-1 && (isId(nc) || nc=='[')) || // the [ is for attributes (see bug702170)
             (i<l-2 && nc=='$' && isId(src[i+2])) ||   // for PHP: ',$name' -> ', $name'
             (i<l-3 && nc=='&' && src[i+2]=='$' && isId(src[i+3])) // for PHP: ',&$name' -> ', &$name'
            )
           )
        {
          *dst++=' '; // add extra space after
        }
        break;
      case '^':  // CLI 'Type^name' -> 'Type^ name'
      case '%':  // CLI 'Type%name' -> 'Type% name'
        *dst++=c;
        if (cliSupport && i<l-1 && (isId(nc) || nc=='-'))
        {
          *dst++=' '; // add extra space after
        }
        break;
      case ')':  // current char is a )  -> ')name' -> ') name'
        *dst++=c;
        if (i<l-1 && (isId(nc) || nc=='-'))
        {
          *dst++=' '; // add extra space after
        }
        break;
      case '*':
        if (i>0 && pc!=' ' && pc!='\t' && pc!=':' &&
                   pc!='*' && pc!='&'  && pc!='(' && pc!='/' &&
                   pc!='.' && osp<9
           )
          // avoid splitting &&, **, .*, operator*, operator->*
        {
          *dst++=' ';
        }
        *dst++=c;
        break;
      case '&':
        if (i>0 && isId(pc) && osp<9)
        {
          if (nc != '=')
          // avoid splitting operator&=
          {
            *dst++=' ';
          }
        }
        *dst++=c;
        break;
      case '@':  // '@name' -> ' @name'
      case '$':  // '$name' -> ' $name'
      case '\'': // ''name' -> '' name'
        if (i>0 && i<l-1 && pc!='=' && pc!=':' && !isspace((uchar)pc) &&
            isId(nc) && osp<8) // ")id" -> ") id"
        {
          *dst++=' ';
        }
        *dst++=c;
        break;
      case ':': // current char is a :
        if (csp==6) // replace const::A by const ::A
        {
          *dst++=' ';
          csp=0;
        }
        else if (vsp==8) // replace virtual::A by virtual ::A
        {
          *dst++=' ';
          vsp=0;
        }
        *dst++=c;
        break;
      case ' ':  // fallthrough
      case '\n': // fallthrough
      case '\t':
        {
          if (g_charAroundSpace.charMap[(uchar)pc].before &&
              g_charAroundSpace.charMap[(uchar)nc].after  &&
              !(pc==',' && nc=='.') &&
              (osp<8 || (osp>=8 && isId(pc) && isId(nc)))
                  // e.g.    'operator >>' -> 'operator>>',
                  //         'operator "" _x' -> 'operator""_x',
                  // but not 'operator int' -> 'operatorint'
             )
          { // keep space
            *dst++=' ';
          }
          else if ((pc=='*' || pc=='&' || pc=='.') && nc=='>')
          {
            *dst++=' ';
          }
        }
        break;
      default:
        *dst++=c;
        if (c=='t' && csp==5 && i<l-1 && // found 't' in 'const'
             !(isId(nc) || nc==')' || nc==',' || isspace((uchar)nc))
           ) // prevent const ::A from being converted to const::A
        {
          *dst++=' ';
          csp=0;
        }
        else if (c=='l' && vsp==7 && i<l-1 && // found 'l' in 'virtual'
             !(isId(nc) || nc==')' || nc==',' || isspace((uchar)nc))
            ) // prevent virtual ::A from being converted to virtual::A
        {
          *dst++=' ';
          vsp=0;
        }
        break;
    }
    pc=c;
  }
  *dst++='\0';
  //printf("removeRedundantWhitespace(%s)->%s\n",s.data(),growBuf);
  return growBuf;
}

/**
 * Returns the position in the string where a function parameter list
 * begins, or -1 if one is not found.
 */
int findParameterList(const QCString &name)
{
  int pos=-1;
  int templateDepth=0;
  do
  {
    if (templateDepth > 0)
    {
      int nextOpenPos=name.findRev('>', pos);
      int nextClosePos=name.findRev('<', pos);
      if (nextOpenPos!=-1 && nextOpenPos>nextClosePos)
      {
        ++templateDepth;
        pos=nextOpenPos-1;
      }
      else if (nextClosePos!=-1)
      {
        --templateDepth;
        pos=nextClosePos-1;
      }
      else // more >'s than <'s, see bug701295
      {
        return -1;
      }
    }
    else
    {
      int lastAnglePos=name.findRev('>', pos);
      int bracePos=name.findRev('(', pos);
      if (lastAnglePos!=-1 && lastAnglePos>bracePos)
      {
        ++templateDepth;
        pos=lastAnglePos-1;
      }
      else
      {
        int bp = bracePos>0 ? name.findRev('(',bracePos-1) : -1;
        // bp test is to allow foo(int(&)[10]), but we need to make an exception for operator()
        return bp==-1 || (bp>=8 && name.mid(bp-8,10)=="operator()") ? bracePos : bp;
      }
    }
  } while (pos!=-1);
  return -1;
}

bool rightScopeMatch(const QCString &scope, const QCString &name)
{
  int sl=scope.length();
  int nl=name.length();
  return (name==scope || // equal
          (scope.right(nl)==name && // substring
           sl-nl>1 && scope.at(sl-nl-1)==':' && scope.at(sl-nl-2)==':' // scope
          )
         );
}

bool leftScopeMatch(const QCString &scope, const QCString &name)
{
  int sl=scope.length();
  int nl=name.length();
  return (name==scope || // equal
          (scope.left(nl)==name && // substring
           sl>nl+1 && scope.at(nl)==':' && scope.at(nl+1)==':' // scope
          )
         );
}


void linkifyText(const TextGeneratorIntf &out, const Definition *scope,
    const FileDef *fileScope,const Definition *self,
    const char *text, bool autoBreak,bool external,
    bool keepSpaces,int indentLevel)
{
  if (text==0) return;
  //printf("linkify='%s'\n",text);
  std::string txtStr=text;
  size_t strLen = txtStr.length();
  if (strLen==0) return;

  static std::regex regExp("[[:alpha:]_][[:alnum:]_~!\\\\.:$]*");
  std::sregex_iterator it( txtStr.begin(), txtStr.end(), regExp);
  std::sregex_iterator end;

  //printf("linkifyText scope=%s fileScope=%s strtxt=%s strlen=%d external=%d\n",
  //    scope?scope->name().data():"<none>",
  //    fileScope?fileScope->name().data():"<none>",
  //    txtStr.data(),strLen,external);
  size_t index=0;
  size_t skipIndex=0;
  size_t floatingIndex=0;
  for (; it!=end ; ++it) // for each word from the text string
  {
    const auto &match = *it;
    size_t newIndex = match.position();
    size_t matchLen = match.length();
    floatingIndex+=newIndex-skipIndex+matchLen;
    if (newIndex>0 && txtStr.at(newIndex-1)=='0') // ignore hex numbers (match x00 in 0x00)
    {
      std::string part = txtStr.substr(skipIndex,newIndex+matchLen-skipIndex);
      out.writeString(part.c_str(),keepSpaces);
      skipIndex=index=newIndex+matchLen;
      continue;
    }

    // add non-word part to the result
    bool insideString=FALSE;
    for (size_t i=index;i<newIndex;i++)
    {
      if (txtStr.at(i)=='"') insideString=!insideString;
    }

    //printf("floatingIndex=%d strlen=%d autoBreak=%d\n",floatingIndex,strLen,autoBreak);
    if (strLen>35 && floatingIndex>30 && autoBreak) // try to insert a split point
    {
      std::string splitText = txtStr.substr(skipIndex,newIndex-skipIndex);
      size_t splitLength = splitText.length();
      size_t offset=1;
      size_t i = splitText.find(',');
      if (i==std::string::npos) { i=splitText.find('<'); if (i!=std::string::npos) offset=0; }
      if (i==std::string::npos) i=splitText.find('>');
      if (i==std::string::npos) i=splitText.find(' ');
      //printf("splitText=[%s] len=%d i=%d offset=%d\n",splitText.data(),splitLength,i,offset);
      if (i!=std::string::npos) // add a link-break at i in case of Html output
      {
        std::string part1 = splitText.substr(0,i+offset);
        out.writeString(part1.c_str(),keepSpaces);
        out.writeBreak(indentLevel==0 ? 0 : indentLevel+1);
        std::string part2 = splitText.substr(i+offset);
        out.writeString(part2.c_str(),keepSpaces);
        floatingIndex=splitLength-i-offset+matchLen;
      }
      else
      {
        out.writeString(splitText.c_str(),keepSpaces);
      }
    }
    else
    {
      //ol.docify(txtStr.mid(skipIndex,newIndex-skipIndex));
      std::string part = txtStr.substr(skipIndex,newIndex-skipIndex);
      out.writeString(part.c_str(),keepSpaces);
    }
    // get word from string
    std::string word=txtStr.substr(newIndex,matchLen);
    QCString matchWord = substitute(substitute(word,"\\","::"),".","::");
    //printf("linkifyText word=%s matchWord=%s scope=%s\n",
    //    word.data(),matchWord.data(),scope?scope->name().data():"<none>");
    bool found=FALSE;
    if (!insideString)
    {
      const MemberDef    *md=0;
      const ClassDef     *cd=0;
      const FileDef      *fd=0;
      const NamespaceDef *nd=0;
      const GroupDef     *gd=0;
      //printf("** Match word '%s'\n",matchWord.data());

      SymbolResolver resolver(fileScope);
      cd=resolver.resolveClass(scope,matchWord);
      const MemberDef *typeDef = resolver.getTypedef();
      if (typeDef) // First look at typedef then class, see bug 584184.
      {
        //printf("Found typedef %s\n",typeDef->name().data());
        if (external ? typeDef->isLinkable() : typeDef->isLinkableInProject())
        {
          if (typeDef->getOuterScope()!=self)
          {
            out.writeLink(typeDef->getReference(),
                typeDef->getOutputFileBase(),
                typeDef->anchor(),
                word.c_str());
            found=TRUE;
          }
        }
      }
      if (!found && (cd || (cd=getClass(matchWord))))
      {
        //printf("Found class %s\n",cd->name().data());
        // add link to the result
        if (external ? cd->isLinkable() : cd->isLinkableInProject())
        {
          if (cd!=self)
          {
            out.writeLink(cd->getReference(),cd->getOutputFileBase(),cd->anchor(),word.c_str());
            found=TRUE;
          }
        }
      }
      else if ((cd=getClass(matchWord+"-p"))) // search for Obj-C protocols as well
      {
        // add link to the result
        if (external ? cd->isLinkable() : cd->isLinkableInProject())
        {
          if (cd!=self)
          {
            out.writeLink(cd->getReference(),cd->getOutputFileBase(),cd->anchor(),word.c_str());
            found=TRUE;
          }
        }
      }
//      else if ((cd=getClass(matchWord+"-g"))) // C# generic as well
//      {
//        // add link to the result
//        if (external ? cd->isLinkable() : cd->isLinkableInProject())
//        {
//          if (cd!=self)
//          {
//            out.writeLink(cd->getReference(),cd->getOutputFileBase(),cd->anchor(),word);
//            found=TRUE;
//          }
//        }
//      }
      else
      {
        //printf("   -> nothing\n");
      }

      int m = matchWord.findRev("::");
      QCString scopeName;
      if (scope &&
          (scope->definitionType()==Definition::TypeClass ||
           scope->definitionType()==Definition::TypeNamespace
          )
         )
      {
        scopeName=scope->name();
      }
      else if (m!=-1)
      {
        scopeName = matchWord.left(m);
        matchWord = matchWord.mid(m+2);
      }

      //printf("ScopeName=%s\n",scopeName.data());
      //if (!found) printf("Trying to link %s in %s\n",word.data(),scopeName.data());
      if (!found &&
          getDefs(scopeName,matchWord,0,md,cd,fd,nd,gd) &&
          //(md->isTypedef() || md->isEnumerate() ||
          // md->isReference() || md->isVariable()
          //) &&
          (external ? md->isLinkable() : md->isLinkableInProject())
         )
      {
        //printf("Found ref scope=%s\n",d?d->name().data():"<global>");
        //ol.writeObjectLink(d->getReference(),d->getOutputFileBase(),
        //                       md->anchor(),word);
        if (md!=self && (self==0 || md->name()!=self->name()))
          // name check is needed for overloaded members, where getDefs just returns one
        {
          /* in case of Fortran scop and the variable is a non Fortran variable: don't link,
           * see also getLink in fortrancode.l
           */
          if (!(scope && (scope->getLanguage() == SrcLangExt_Fortran) && md->isVariable() && (md->getLanguage() != SrcLangExt_Fortran)))
          {
            out.writeLink(md->getReference(),md->getOutputFileBase(),
                md->anchor(),word.c_str());
            //printf("found symbol %s\n",matchWord.data());
            found=TRUE;
          }
        }
      }
    }

    if (!found) // add word to the result
    {
      out.writeString(word.c_str(),keepSpaces);
    }
    // set next start point in the string
    //printf("index=%d/%d\n",index,txtStr.length());
    skipIndex=index=newIndex+matchLen;
  }
  // add last part of the string to the result.
  //ol.docify(txtStr.right(txtStr.length()-skipIndex));
  std::string lastPart = txtStr.substr(skipIndex);
  out.writeString(lastPart.c_str(),keepSpaces);
}

void writeMarkerList(OutputList &ol,const std::string &markerText,size_t numMarkers,
                     std::function<void(size_t)> replaceFunc)
{
  static std::regex marker("@([[:digit:]]+)");
  std::sregex_iterator it(markerText.begin(),markerText.end(),marker);
  std::sregex_iterator end;
  size_t index=0;
  // now replace all markers in inheritLine with links to the classes
  for ( ; it!=end ; ++it)
  {
    const auto &match = *it;
    size_t newIndex = match.position();
    size_t matchLen = match.length();
    ol.parseText(markerText.substr(index,newIndex-index));
    unsigned long entryIndex = std::stoul(match[1]);
    if (entryIndex<(unsigned long)numMarkers)
    {
      replaceFunc(entryIndex);
    }
    index=newIndex+matchLen;
  }
  ol.parseText(markerText.substr(index));
}

void writeExamples(OutputList &ol,const ExampleList &list)
{
  auto replaceFunc = [&list,&ol](size_t entryIndex)
  {
    const auto &e = list[entryIndex];
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.disable(OutputGenerator::Docbook);
    // link for Html / man
    //printf("writeObjectLink(file=%s)\n",e->file.data());
    ol.writeObjectLink(0,e.file,e.anchor,e.name);
    ol.popGeneratorState();

    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Man);
    ol.disable(OutputGenerator::Html);
    // link for Latex / pdf with anchor because the sources
    // are not hyperlinked (not possible with a verbatim environment).
    ol.writeObjectLink(0,e.file,0,e.name);
    ol.popGeneratorState();
  };

  writeMarkerList(ol, theTranslator->trWriteList((int)list.size()).str(), list.size(), replaceFunc);

  ol.writeString(".");
}


QCString argListToString(const ArgumentList &al,bool useCanonicalType,bool showDefVals)
{
  QCString result;
  if (!al.hasParameters()) return result;
  result+="(";
  for (auto it = al.begin() ; it!=al.end() ;)
  {
    Argument a = *it;
    QCString type1 = useCanonicalType && !a.canType.isEmpty() ? a.canType : a.type;
    QCString type2;
    int i=type1.find(")("); // hack to deal with function pointers
    if (i!=-1)
    {
      type2=type1.mid(i);
      type1=type1.left(i);
    }
    if (!a.attrib.isEmpty())
    {
      result+=a.attrib+" ";
    }
    if (!a.name.isEmpty() || !a.array.isEmpty())
    {
      result+= type1+" "+a.name+type2+a.array;
    }
    else
    {
      result+= type1+type2;
    }
    if (!a.defval.isEmpty() && showDefVals)
    {
      result+="="+a.defval;
    }
    ++it;
    if (it!=al.end()) result+=", ";
  }
  result+=")";
  if (al.constSpecifier()) result+=" const";
  if (al.volatileSpecifier()) result+=" volatile";
  if (al.refQualifier()==RefQualifierLValue) result+=" &";
  else if (al.refQualifier()==RefQualifierRValue) result+=" &&";
  if (!al.trailingReturnType().isEmpty()) result+=al.trailingReturnType();
  if (al.pureSpecifier()) result+=" =0";
  return removeRedundantWhiteSpace(result);
}

QCString tempArgListToString(const ArgumentList &al,SrcLangExt lang,bool includeDefault)
{
  QCString result;
  if (al.empty()) return result;
  result="<";
  bool first=true;
  for (const auto &a : al)
  {
    if (a.defval.isEmpty() || includeDefault)
    {
      if (!first) result+=", ";
      if (!a.name.isEmpty()) // add template argument name
      {
        if (a.type.left(4)=="out") // C# covariance
        {
          result+="out ";
        }
        else if (a.type.left(3)=="in") // C# contravariance
        {
          result+="in ";
        }
        if (lang==SrcLangExt_Java || lang==SrcLangExt_CSharp)
        {
          result+=a.type+" ";
        }
        result+=a.name;
      }
      else // extract name from type
      {
        int i=a.type.length()-1;
        while (i>=0 && isId(a.type.at(i))) i--;
        if (i>0)
        {
          result+=a.type.right(a.type.length()-i-1);
          if (a.type.find("...")!=-1)
          {
            result+="...";
          }
        }
        else // nothing found -> take whole name
        {
          result+=a.type;
        }
      }
      if (!a.typeConstraint.isEmpty() && lang==SrcLangExt_Java)
      {
        result+=" extends "; // TODO: now Java specific, C# has where...
        result+=a.typeConstraint;
      }
      first=false;
    }
  }
  result+=">";
  return removeRedundantWhiteSpace(result);
}


//----------------------------------------------------------------------------

/*! takes the \a buf of the given length \a len and converts CR LF (DOS)
 * or CR (MAC) line ending to LF (Unix).  Returns the length of the
 * converted content (i.e. the same as \a len (Unix, MAC) or
 * smaller (DOS).
 */
int filterCRLF(char *buf,int len)
{
  int src = 0;    // source index
  int dest = 0;   // destination index
  char c;         // current character

  while (src<len)
  {
    c = buf[src++];            // Remember the processed character.
    if (c == '\r')             // CR to be solved (MAC, DOS)
    {
      c = '\n';                // each CR to LF
      if (src<len && buf[src] == '\n')
        ++src;                 // skip LF just after CR (DOS)
    }
    else if ( c == '\0' && src<len-1) // filter out internal \0 characters, as it will confuse the parser
    {
      c = ' ';                 // turn into a space
    }
    buf[dest++] = c;           // copy the (modified) character to dest
  }
  return dest;                 // length of the valid part of the buf
}

static std::string wildcard2regex(const std::string &pattern)
{
  std::string result="^"; // match start of input
  char c;
  const char *p = pattern.c_str();
  while ((c=*p++))
  {
    switch(c)
    {
      case '*': result+=".*"; break; // '*' => '.*'
      case '?': result+='.';  break; // '?' => '.'
      case '.': case '+': case '\\': case '$': case '^': result+='\\'; result+=c; break; // escape
      case '[': if (*p=='^') { result+="[^"; p++; } else result+=c; break; // don't escape ^ after [
      default: result+=c; break; // just copy
    }
  }
  result+='$'; // match end of input
  return result;
}

static bool isMatchingWildcard(const std::string &input,const std::string &pattern,
                               bool caseSensitive=false)
{

  std::regex::flag_type flags = std::regex::ECMAScript;
  if (!caseSensitive) flags |= std::regex::icase;
  std::string re_str = wildcard2regex(pattern);
  std::regex rePattern(wildcard2regex(pattern),flags);
  return std::regex_match(input,rePattern);
}

static QCString getFilterFromList(const char *name,const StringVector &filterList,bool &found)
{
  found=FALSE;
  // compare the file name to the filter pattern list
  for (const auto &filterStr : filterList)
  {
    QCString fs = filterStr.c_str();
    int i_equals=fs.find('=');
    if (i_equals!=-1)
    {
      QCString filterPattern = fs.left(i_equals);
      if (isMatchingWildcard(name,filterPattern.str(),Portable::fileSystemIsCaseSensitive()))
      {
        // found a match!
        QCString filterName = fs.mid(i_equals+1);
        if (filterName.find(' ')!=-1)
        { // add quotes if the name has spaces
          filterName="\""+filterName+"\"";
        }
        found=TRUE;
        return filterName;
      }
    }
  }

  // no match
  return "";
}

/*! looks for a filter for the file \a name.  Returns the name of the filter
 *  if there is a match for the file name, otherwise an empty string.
 *  In case \a inSourceCode is TRUE then first the source filter list is
 *  considered.
 */
QCString getFileFilter(const char* name,bool isSourceCode)
{
  // sanity check
  if (name==0) return "";

  const StringVector& filterSrcList = Config_getList(FILTER_SOURCE_PATTERNS);
  const StringVector& filterList    = Config_getList(FILTER_PATTERNS);

  QCString filterName;
  bool found=FALSE;
  if (isSourceCode && !filterSrcList.empty())
  { // first look for source filter pattern list
    filterName = getFilterFromList(name,filterSrcList,found);
  }
  if (!found && filterName.isEmpty())
  { // then look for filter pattern list
    filterName = getFilterFromList(name,filterList,found);
  }
  if (!found)
  { // then use the generic input filter
    return Config_getString(INPUT_FILTER);
  }
  else
  {
    /* remove surrounding double quotes */
    if ((filterName.right(1) == "\"") && (filterName.left(1) == "\""))
    {
       filterName.remove(filterName.length() - 1, 1);
       filterName.remove(0, 1);
    }
    return filterName;
  }
}


QCString transcodeCharacterStringToUTF8(const QCString &input)
{
  bool error=FALSE;
  static QCString inputEncoding = Config_getString(INPUT_ENCODING);
  const char *outputEncoding = "UTF-8";
  if (inputEncoding.isEmpty() || qstricmp(inputEncoding,outputEncoding)==0) return input;
  int inputSize=input.length();
  int outputSize=inputSize*4+1;
  QCString output(outputSize);
  void *cd = portable_iconv_open(outputEncoding,inputEncoding);
  if (cd==(void *)(-1))
  {
    err("unsupported character conversion: '%s'->'%s'\n",
        inputEncoding.data(),outputEncoding);
    error=TRUE;
  }
  if (!error)
  {
    size_t iLeft=inputSize;
    size_t oLeft=outputSize;
    char *inputPtr = input.rawData();
    char *outputPtr = output.rawData();
    if (!portable_iconv(cd, &inputPtr, &iLeft, &outputPtr, &oLeft))
    {
      outputSize-=(int)oLeft;
      output.resize(outputSize+1);
      output.at(outputSize)='\0';
      //printf("iconv: input size=%d output size=%d\n[%s]\n",size,newSize,srcBuf.data());
    }
    else
    {
      err("failed to translate characters from %s to %s: check INPUT_ENCODING\ninput=[%s]\n",
          inputEncoding.data(),outputEncoding,input.data());
      error=TRUE;
    }
  }
  portable_iconv_close(cd);
  return error ? input : output;
}

/*! reads a file with name \a name and returns it as a string. If \a filter
 *  is TRUE the file will be filtered by any user specified input filter.
 *  If \a name is "-" the string will be read from standard input.
 */
QCString fileToString(const char *name,bool filter,bool isSourceCode)
{
  if (name==0 || name[0]==0) return 0;
  QFile f;

  bool fileOpened=FALSE;
  if (name[0]=='-' && name[1]==0) // read from stdin
  {
    fileOpened=f.open(IO_ReadOnly,stdin);
    if (fileOpened)
    {
      const int bSize=4096;
      QCString contents(bSize+1);
      int totalSize=0;
      int size;
      while ((size=f.readBlock(contents.rawData()+totalSize,bSize))==bSize)
      {
        totalSize+=bSize;
        contents.resize(totalSize+bSize+1);
      }
      totalSize = filterCRLF(contents.rawData(),totalSize+size)+2;
      contents.resize(totalSize);
      contents.at(totalSize-2)='\n'; // to help the scanner
      contents.at(totalSize-1)='\0';
      return contents;
    }
  }
  else // read from file
  {
    QFileInfo fi(name);
    if (!fi.exists() || !fi.isFile())
    {
      err("file '%s' not found\n",name);
      return "";
    }
    BufStr buf(fi.size());
    fileOpened=readInputFile(name,buf,filter,isSourceCode);
    if (fileOpened)
    {
      int s = buf.size();
      if (s>1 && buf.at(s-2)!='\n')
      {
        buf.at(s-1)='\n';
        buf.addChar(0);
      }
      return buf.data();
    }
  }
  if (!fileOpened)
  {
    err("cannot open file '%s' for reading\n",name);
  }
  return "";
}

static std::tm getCurrentDateTime()
{
  QCString sourceDateEpoch = Portable::getenv("SOURCE_DATE_EPOCH");
  if (!sourceDateEpoch.isEmpty()) // see https://reproducible-builds.org/specs/source-date-epoch/
  {
    bool ok;
    uint64 epoch = sourceDateEpoch.toUInt64(&ok);
    if (!ok)
    {
      static bool warnedOnce=FALSE;
      if (!warnedOnce)
      {
        warn_uncond("Environment variable SOURCE_DATE_EPOCH does not contain a valid number; value is '%s'\n",
            sourceDateEpoch.data());
        warnedOnce=TRUE;
      }
    }
    else // use given epoch value as current 'built' time
    {
      auto epoch_start    = std::chrono::time_point<std::chrono::system_clock>{};
      auto epoch_seconds  = std::chrono::seconds(epoch);
      auto build_time     = epoch_start + epoch_seconds;
      std::time_t time    = std::chrono::system_clock::to_time_t(build_time);
      return *gmtime(&time);
    }
  }

  // return current local time
  auto now = std::chrono::system_clock::now();
  std::time_t time = std::chrono::system_clock::to_time_t(now);
  return *localtime(&time);
}

QCString dateToString(bool includeTime)
{
  auto current = getCurrentDateTime();
  return theTranslator->trDateTime(current.tm_year + 1900,
                                   current.tm_mon + 1,
                                   current.tm_mday,
                                   (current.tm_wday+6)%7+1, // map: Sun=0..Sat=6 to Mon=1..Sun=7
                                   current.tm_hour,
                                   current.tm_min,
                                   current.tm_sec,
                                   includeTime);
}

QCString yearToString()
{
  auto current = getCurrentDateTime();
  return QCString().setNum(current.tm_year+1900);
}

//----------------------------------------------------------------------
// recursive function that returns the number of branches in the
// inheritance tree that the base class 'bcd' is below the class 'cd'

int minClassDistance(const ClassDef *cd,const ClassDef *bcd,int level)
{
  if (bcd->categoryOf()) // use class that is being extended in case of
    // an Objective-C category
  {
    bcd=bcd->categoryOf();
  }
  if (cd==bcd) return level;
  if (level==256)
  {
    warn_uncond("class %s seem to have a recursive "
        "inheritance relation!\n",cd->name().data());
    return -1;
  }
  int m=maxInheritanceDepth;
  for (const auto &bcdi : cd->baseClasses())
  {
    int mc=minClassDistance(bcdi.classDef,bcd,level+1);
    if (mc<m) m=mc;
    if (m<0) break;
  }
  return m;
}

Protection classInheritedProtectionLevel(const ClassDef *cd,const ClassDef *bcd,Protection prot,int level)
{
  if (bcd->categoryOf()) // use class that is being extended in case of
    // an Objective-C category
  {
    bcd=bcd->categoryOf();
  }
  if (cd==bcd)
  {
    goto exit;
  }
  if (level==256)
  {
    err("Internal inconsistency: found class %s seem to have a recursive "
        "inheritance relation! Please send a bug report to doxygen@gmail.com\n",cd->name().data());
  }
  else if (prot!=Private)
  {
    for (const auto &bcdi : cd->baseClasses())
    {
      Protection baseProt = classInheritedProtectionLevel(bcdi.classDef,bcd,bcdi.prot,level+1);
      if (baseProt==Private)        prot=Private;
      else if (baseProt==Protected) prot=Protected;
    }
  }
exit:
  //printf("classInheritedProtectionLevel(%s,%s)=%d\n",cd->name().data(),bcd->name().data(),prot);
  return prot;
}

void trimBaseClassScope(const BaseClassList &bcl,QCString &s,int level=0)
{
  //printf("trimBaseClassScope level=%d '%s'\n",level,s.data());
  for (const auto &bcd : bcl)
  {
    ClassDef *cd=bcd.classDef;
    //printf("Trying class %s\n",cd->name().data());
    int spos=s.find(cd->name()+"::");
    if (spos!=-1)
    {
      s = s.left(spos)+s.right(
          s.length()-spos-cd->name().length()-2
          );
    }
    //printf("base class '%s'\n",cd->name().data());
    if (!cd->baseClasses().empty())
    {
      trimBaseClassScope(cd->baseClasses(),s,level+1);
    }
  }
}

static void stripIrrelevantString(QCString &target,const QCString &str)
{
  if (target==str) { target.resize(0); return; }
  int i,p=0;
  int l=str.length();
  bool changed=FALSE;
  while ((i=target.find(str,p))!=-1)
  {
    bool isMatch = (i==0 || !isId(target.at(i-1))) && // not a character before str
      (i+l==(int)target.length() || !isId(target.at(i+l))); // not a character after str
    if (isMatch)
    {
      int i1=target.find('*',i+l);
      int i2=target.find('&',i+l);
      if (i1==-1 && i2==-1)
      {
        // strip str from target at index i
        target=target.left(i)+target.right(target.length()-i-l);
        changed=TRUE;
        i-=l;
      }
      else if ((i1!=-1 && i<i1) || (i2!=-1 && i<i2)) // str before * or &
      {
        // move str to front
        target=str+" "+target.left(i)+target.right(target.length()-i-l);
        changed=TRUE;
        i++;
      }
    }
    p = i+l;
  }
  if (changed) target=target.stripWhiteSpace();
}

/*! According to the C++ spec and Ivan Vecerina:

  Parameter declarations  that differ only in the presence or absence
  of const and/or volatile are equivalent.

  So the following example, show what is stripped by this routine
  for const. The same is done for volatile.

  For Java code we also strip the "final" keyword, see bug 765070.

  \code
  const T param     ->   T param          // not relevant
  const T& param    ->   const T& param   // const needed
  T* const param    ->   T* param         // not relevant
  const T* param    ->   const T* param   // const needed
  \endcode
 */
void stripIrrelevantConstVolatile(QCString &s)
{
  //printf("stripIrrelevantConstVolatile(%s)=",s.data());
  stripIrrelevantString(s,"const");
  stripIrrelevantString(s,"volatile");
  stripIrrelevantString(s,"final");
  //printf("%s\n",s.data());
}


// a bit of debug support for matchArguments
#define MATCH
#define NOMATCH
//#define MATCH printf("Match at line %d\n",__LINE__);
//#define NOMATCH printf("Nomatch at line %d\n",__LINE__);

static QCString stripDeclKeywords(const QCString &s)
{
  int i=s.find(" class ");
  if (i!=-1) return s.left(i)+s.mid(i+6);
  i=s.find(" typename ");
  if (i!=-1) return s.left(i)+s.mid(i+9);
  i=s.find(" union ");
  if (i!=-1) return s.left(i)+s.mid(i+6);
  i=s.find(" struct ");
  if (i!=-1) return s.left(i)+s.mid(i+7);
  return s;
}

// forward decl for circular dependencies
static QCString extractCanonicalType(const Definition *d,const FileDef *fs,QCString type);

QCString getCanonicalTemplateSpec(const Definition *d,const FileDef *fs,const QCString& spec)
{

  QCString templSpec = spec.stripWhiteSpace();
  // this part had been commented out before... but it is needed to match for instance
  // std::list<std::string> against list<string> so it is now back again!
  if (!templSpec.isEmpty() && templSpec.at(0) == '<')
  {
    templSpec = "< " + extractCanonicalType(d,fs,templSpec.right(templSpec.length()-1).stripWhiteSpace());
  }
  QCString resolvedType = resolveTypeDef(d,templSpec);
  if (!resolvedType.isEmpty()) // not known as a typedef either
  {
    templSpec = resolvedType;
  }
  //printf("getCanonicalTemplateSpec(%s)=%s\n",spec.data(),templSpec.data());
  return templSpec;
}


static QCString getCanonicalTypeForIdentifier(
    const Definition *d,const FileDef *fs,const QCString &word,
    QCString *tSpec,int count=0)
{
  if (count>10) return word; // oops recursion

  QCString symName,result,templSpec,tmpName;
  if (tSpec && !tSpec->isEmpty())
    templSpec = stripDeclKeywords(getCanonicalTemplateSpec(d,fs,*tSpec));

  if (word.findRev("::")!=-1 && !(tmpName=stripScope(word)).isEmpty())
  {
    symName=tmpName; // name without scope
  }
  else
  {
    symName=word;
  }
  //printf("getCanonicalTypeForIdentifier(%s d=%s fs=%s ,[%s->%s]) start\n",
  //    word.data(),
  //    d ? d->name().data() : "<null>",fs ? fs->name().data() : "<null>",
  //    tSpec?tSpec->data():"<none>",templSpec.data());

  // lookup class / class template instance
  SymbolResolver resolver(fs);
  const ClassDef *cd     = resolver.resolveClass(d,word+templSpec,true,true);
  const MemberDef *mType = resolver.getTypedef();
  QCString ts            = resolver.getTemplateSpec();
  QCString resolvedType  = resolver.getResolvedType();

  bool isTemplInst = cd && !templSpec.isEmpty();
  if (!cd && !templSpec.isEmpty())
  {
    // class template specialization not known, look up class template
    cd           = resolver.resolveClass(d,word,true,true);
    mType        = resolver.getTypedef();
    ts           = resolver.getTemplateSpec();
    resolvedType = resolver.getResolvedType();
  }
  if (cd && cd->isUsedOnly()) cd=0; // ignore types introduced by usage relations

  //printf("cd=%p mtype=%p\n",cd,mType);
  //printf("  getCanonicalTypeForIdentifier: symbol=%s word=%s cd=%s d=%s fs=%s cd->isTemplate=%d\n",
  //    symName.data(),
  //    word.data(),
  //    cd?cd->name().data():"<none>",
  //    d?d->name().data():"<none>",
  //    fs?fs->name().data():"<none>",
  //    cd?cd->isTemplate():-1
  //   );

  //printf("  >>>> word '%s' => '%s' templSpec=%s ts=%s tSpec=%s isTemplate=%d resolvedType=%s\n",
  //    (word+templSpec).data(),
  //    cd?cd->qualifiedName().data():"<none>",
  //    templSpec.data(),ts.data(),
  //    tSpec?tSpec->data():"<null>",
  //    cd?cd->isTemplate():FALSE,
  //    resolvedType.data());

  //printf("  mtype=%s\n",mType?mType->name().data():"<none>");

  if (cd) // resolves to a known class type
  {
    if (cd==d && tSpec) *tSpec="";

    if (mType && mType->isTypedef()) // but via a typedef
    {
      result = resolvedType+ts; // the +ts was added for bug 685125
    }
    else
    {
      if (isTemplInst)
      {
        // spec is already part of class type
        templSpec="";
        if (tSpec) *tSpec="";
      }
      else if (!ts.isEmpty() && templSpec.isEmpty())
      {
        // use formal template args for spec
        templSpec = stripDeclKeywords(getCanonicalTemplateSpec(d,fs,ts));
      }

      result = removeRedundantWhiteSpace(cd->qualifiedName() + templSpec);

      if (cd->isTemplate() && tSpec) //
      {
        if (!templSpec.isEmpty()) // specific instance
        {
          result=cd->name()+templSpec;
        }
        else // use template type
        {
          result=cd->qualifiedNameWithTemplateParameters();
        }
        // template class, so remove the template part (it is part of the class name)
        *tSpec="";
      }
      else if (ts.isEmpty() && !templSpec.isEmpty() && cd && !cd->isTemplate() && tSpec)
      {
        // obscure case, where a class is used as a template, but doxygen think it is
        // not (could happen when loading the class from a tag file).
        *tSpec="";
      }
    }
  }
  else if (mType && mType->isEnumerate()) // an enum
  {
    result = mType->qualifiedName();
  }
  else if (mType && mType->isTypedef()) // a typedef
  {
    //result = mType->qualifiedName(); // changed after 1.7.2
    //result = mType->typeString();
    //printf("word=%s typeString=%s\n",word.data(),mType->typeString());
    if (word!=mType->typeString())
    {
      QCString type = mType->typeString();
      if (type.startsWith("typename "))
      {
        type.stripPrefix("typename ");
        type = stripTemplateSpecifiersFromScope(type,FALSE);
      }
      result = getCanonicalTypeForIdentifier(d,fs,type,tSpec,count+1);
    }
    else
    {
      result = mType->typeString();
    }
  }
  else // fallback
  {
    resolvedType = resolveTypeDef(d,word);
    //printf("typedef [%s]->[%s]\n",word.data(),resolvedType.data());
    if (resolvedType.isEmpty()) // not known as a typedef either
    {
      result = word;
    }
    else
    {
      result = resolvedType;
    }
  }
  //printf("getCanonicalTypeForIdentifier [%s]->[%s]\n",word.data(),result.data());
  return result;
}

static QCString extractCanonicalType(const Definition *d,const FileDef *fs,QCString type)
{
  type = type.stripWhiteSpace();

  // strip const and volatile keywords that are not relevant for the type
  stripIrrelevantConstVolatile(type);

  // strip leading keywords
  type.stripPrefix("class ");
  type.stripPrefix("struct ");
  type.stripPrefix("union ");
  type.stripPrefix("enum ");
  type.stripPrefix("typename ");

  type = removeRedundantWhiteSpace(type);
  //printf("extractCanonicalType(type=%s) start: def=%s file=%s\n",type.data(),
  //    d ? d->name().data() : "<null>",fs ? fs->name().data() : "<null>");

  QCString canType;
  QCString templSpec,word;
  int i,p=0,pp=0;
  while ((i=extractClassNameFromType(type,p,word,templSpec))!=-1)
    // foreach identifier in the type
  {
    //printf("     i=%d p=%d\n",i,p);
    if (i>pp) canType += type.mid(pp,i-pp);

    QCString ct = getCanonicalTypeForIdentifier(d,fs,word,&templSpec);

    // in case the ct is empty it means that "word" represents scope "d"
    // and this does not need to be added to the canonical
    // type (it is redundant), so/ we skip it. This solves problem 589616.
    if (ct.isEmpty() && type.mid(p,2)=="::")
    {
      p+=2;
    }
    else
    {
      canType += ct;
    }
    //printf(" word=%s templSpec=%s canType=%s ct=%s\n",
    //    word.data(),templSpec.data(),canType.data(),ct.data());
    if (!templSpec.isEmpty()) // if we didn't use up the templSpec already
                              // (i.e. type is not a template specialization)
                              // then resolve any identifiers inside.
    {
      static std::regex re("[[:alpha:]_][[:alnum:]_]*");
      std::string ts = templSpec.str();
      std::sregex_iterator it(ts.begin(),ts.end(),re);
      std::sregex_iterator end;

      size_t tp=0;
      // for each identifier template specifier
      //printf("adding resolved %s to %s\n",templSpec.data(),canType.data());
      for (; it!=end ; ++it)
      {
        const auto &match = *it;
        size_t ti = match.position();
        size_t tl = match.length();
        std::string matchStr = match.str();
        canType += ts.substr(tp,ti-tp);
        canType += getCanonicalTypeForIdentifier(d,fs,matchStr.c_str(),0);
        tp=ti+tl;
      }
      canType+=ts.substr(tp);
    }

    pp=p;
  }
  canType += type.right(type.length()-pp);
  //printf("extractCanonicalType = '%s'->'%s'\n",type.data(),canType.data());

  return removeRedundantWhiteSpace(canType);
}

static QCString extractCanonicalArgType(const Definition *d,const FileDef *fs,const Argument &arg)
{
  QCString type = arg.type.stripWhiteSpace();
  QCString name = arg.name;
  //printf("----- extractCanonicalArgType(type=%s,name=%s)\n",type.data(),name.data());
  if ((type=="const" || type=="volatile") && !name.isEmpty())
  { // name is part of type => correct
    type+=" ";
    type+=name;
  }
  if (name=="const" || name=="volatile")
  { // name is part of type => correct
    if (!type.isEmpty()) type+=" ";
    type+=name;
  }
  if (!arg.array.isEmpty())
  {
    type+=arg.array;
  }

  return extractCanonicalType(d,fs,type);
}

static bool matchArgument2(
    const Definition *srcScope,const FileDef *srcFileScope,Argument &srcA,
    const Definition *dstScope,const FileDef *dstFileScope,Argument &dstA
    )
{
  //printf(">> match argument: %s::'%s|%s' (%s) <-> %s::'%s|%s' (%s)\n",
  //    srcScope ? srcScope->name().data() : "",
  //    srcA.type.data(),srcA.name.data(),srcA.canType.data(),
  //    dstScope ? dstScope->name().data() : "",
  //    dstA.type.data(),dstA.name.data(),dstA.canType.data());

  //if (srcA->array!=dstA->array) // nomatch for char[] against char
  //{
  //  NOMATCH
  //  return FALSE;
  //}
  QCString sSrcName = " "+srcA.name;
  QCString sDstName = " "+dstA.name;
  QCString srcType  = srcA.type;
  QCString dstType  = dstA.type;
  stripIrrelevantConstVolatile(srcType);
  stripIrrelevantConstVolatile(dstType);
  //printf("'%s'<->'%s'\n",sSrcName.data(),dstType.right(sSrcName.length()).data());
  //printf("'%s'<->'%s'\n",sDstName.data(),srcType.right(sDstName.length()).data());
  if (sSrcName==dstType.right(sSrcName.length()))
  { // case "unsigned int" <-> "unsigned int i"
    srcA.type+=sSrcName;
    srcA.name="";
    srcA.canType=""; // invalidate cached type value
  }
  else if (sDstName==srcType.right(sDstName.length()))
  { // case "unsigned int i" <-> "unsigned int"
    dstA.type+=sDstName;
    dstA.name="";
    dstA.canType=""; // invalidate cached type value
  }

  if (srcA.canType.isEmpty() || dstA.canType.isEmpty())
  {
    // need to re-evaluate both see issue #8370
    srcA.canType = extractCanonicalArgType(srcScope,srcFileScope,srcA);
    dstA.canType = extractCanonicalArgType(dstScope,dstFileScope,dstA);
  }

  if (srcA.canType==dstA.canType)
  {
    MATCH
    return TRUE;
  }
  else
  {
    //printf("   Canonical types do not match [%s]<->[%s]\n",
    //    srcA->canType.data(),dstA->canType.data());
    NOMATCH
    return FALSE;
  }
}


// new algorithm for argument matching
bool matchArguments2(const Definition *srcScope,const FileDef *srcFileScope,const ArgumentList *srcAl,
                     const Definition *dstScope,const FileDef *dstFileScope,const ArgumentList *dstAl,
                     bool checkCV)
{
  ASSERT(srcScope!=0 && dstScope!=0);

  if (srcAl==0 || dstAl==0)
  {
    bool match = srcAl==dstAl;
    if (match)
    {
      MATCH
      return TRUE;
    }
    else
    {
      NOMATCH
      return FALSE;
    }
  }

  // handle special case with void argument
  if ( srcAl->empty() && dstAl->size()==1 && dstAl->front().type=="void" )
  { // special case for finding match between func() and func(void)
    Argument a;
    a.type = "void";
    const_cast<ArgumentList*>(srcAl)->push_back(a);
    MATCH
    return TRUE;
  }
  if ( dstAl->empty() && srcAl->size()==1 && srcAl->front().type=="void" )
  { // special case for finding match between func(void) and func()
    Argument a;
    a.type = "void";
    const_cast<ArgumentList*>(dstAl)->push_back(a);
    MATCH
    return TRUE;
  }

  if (srcAl->size() != dstAl->size())
  {
    NOMATCH
    return FALSE; // different number of arguments -> no match
  }

  if (checkCV)
  {
    if (srcAl->constSpecifier() != dstAl->constSpecifier())
    {
      NOMATCH
      return FALSE; // one member is const, the other not -> no match
    }
    if (srcAl->volatileSpecifier() != dstAl->volatileSpecifier())
    {
      NOMATCH
      return FALSE; // one member is volatile, the other not -> no match
    }
  }

  if (srcAl->refQualifier() != dstAl->refQualifier())
  {
    NOMATCH
    return FALSE; // one member is has a different ref-qualifier than the other
  }

  // so far the argument list could match, so we need to compare the types of
  // all arguments.
  auto srcIt = srcAl->begin();
  auto dstIt = dstAl->begin();
  for (;srcIt!=srcAl->end() && dstIt!=dstAl->end();++srcIt,++dstIt)
  {
    Argument &srcA = const_cast<Argument&>(*srcIt);
    Argument &dstA = const_cast<Argument&>(*dstIt);
    if (!matchArgument2(srcScope,srcFileScope,srcA,
          dstScope,dstFileScope,dstA)
       )
    {
      NOMATCH
      return FALSE;
    }
  }
  MATCH
  return TRUE; // all arguments match
}



// merges the initializer of two argument lists
// pre:  the types of the arguments in the list should match.
void mergeArguments(ArgumentList &srcAl,ArgumentList &dstAl,bool forceNameOverwrite)
{
  //printf("mergeArguments '%s', '%s'\n",
  //    argListToString(srcAl).data(),argListToString(dstAl).data());

  if (srcAl.size()!=dstAl.size())
  {
    return; // invalid argument lists -> do not merge
  }

  auto srcIt=srcAl.begin();
  auto dstIt=dstAl.begin();
  while (srcIt!=srcAl.end() && dstIt!=dstAl.end())
  {
    Argument &srcA = *srcIt;
    Argument &dstA = *dstIt;

    if (srcA.defval.isEmpty() && !dstA.defval.isEmpty())
    {
      //printf("Defval changing '%s'->'%s'\n",srcA.defval.data(),dstA.defval.data());
      srcA.defval=dstA.defval;
    }
    else if (!srcA.defval.isEmpty() && dstA.defval.isEmpty())
    {
      //printf("Defval changing '%s'->'%s'\n",dstA.defval.data(),srcA.defval.data());
      dstA.defval=srcA.defval;
    }

    // fix wrongly detected const or volatile specifiers before merging.
    // example: "const A *const" is detected as type="const A *" name="const"
    if (srcA.name=="const" || srcA.name=="volatile")
    {
      srcA.type+=" "+srcA.name;
      srcA.name.resize(0);
    }
    if (dstA.name=="const" || dstA.name=="volatile")
    {
      dstA.type+=" "+dstA.name;
      dstA.name.resize(0);
    }

    if (srcA.type==dstA.type)
    {
      //printf("1. merging %s:%s <-> %s:%s\n",srcA.type.data(),srcA.name.data(),dstA.type.data(),dstA.name.data());
      if (srcA.name.isEmpty() && !dstA.name.isEmpty())
      {
        //printf("type: '%s':='%s'\n",srcA.type.data(),dstA.type.data());
        //printf("name: '%s':='%s'\n",srcA.name.data(),dstA.name.data());
        srcA.type = dstA.type;
        srcA.name = dstA.name;
      }
      else if (!srcA.name.isEmpty() && dstA.name.isEmpty())
      {
        //printf("type: '%s':='%s'\n",dstA.type.data(),srcA.type.data());
        //printf("name: '%s':='%s'\n",dstA.name.data(),srcA.name.data());
        dstA.type = srcA.type;
        dstA.name = dstA.name;
      }
      else if (!srcA.name.isEmpty() && !dstA.name.isEmpty())
      {
        //printf("srcA.name=%s dstA.name=%s\n",srcA.name.data(),dstA.name.data());
        if (forceNameOverwrite)
        {
          srcA.name = dstA.name;
        }
        else
        {
          if (srcA.docs.isEmpty() && !dstA.docs.isEmpty())
          {
            srcA.name = dstA.name;
          }
          else if (!srcA.docs.isEmpty() && dstA.docs.isEmpty())
          {
            dstA.name = srcA.name;
          }
        }
      }
    }
    else
    {
      //printf("2. merging '%s':'%s' <-> '%s':'%s'\n",srcA.type.data(),srcA.name.data(),dstA.type.data(),dstA.name.data());
      srcA.type=srcA.type.stripWhiteSpace();
      dstA.type=dstA.type.stripWhiteSpace();
      if (srcA.type+" "+srcA.name==dstA.type) // "unsigned long:int" <-> "unsigned long int:bla"
      {
        srcA.type+=" "+srcA.name;
        srcA.name=dstA.name;
      }
      else if (dstA.type+" "+dstA.name==srcA.type) // "unsigned long int bla" <-> "unsigned long int"
      {
        dstA.type+=" "+dstA.name;
        dstA.name=srcA.name;
      }
      else if (srcA.name.isEmpty() && !dstA.name.isEmpty())
      {
        srcA.name = dstA.name;
      }
      else if (dstA.name.isEmpty() && !srcA.name.isEmpty())
      {
        dstA.name = srcA.name;
      }
    }
    int i1=srcA.type.find("::"),
        i2=dstA.type.find("::"),
        j1=srcA.type.length()-i1-2,
        j2=dstA.type.length()-i2-2;
    if (i1!=-1 && i2==-1 && srcA.type.right(j1)==dstA.type)
    {
      //printf("type: '%s':='%s'\n",dstA.type.data(),srcA.type.data());
      //printf("name: '%s':='%s'\n",dstA.name.data(),srcA.name.data());
      dstA.type = srcA.type.left(i1+2)+dstA.type;
      dstA.name = dstA.name;
    }
    else if (i1==-1 && i2!=-1 && dstA.type.right(j2)==srcA.type)
    {
      //printf("type: '%s':='%s'\n",srcA.type.data(),dstA.type.data());
      //printf("name: '%s':='%s'\n",dstA.name.data(),srcA.name.data());
      srcA.type = dstA.type.left(i2+2)+srcA.type;
      srcA.name = dstA.name;
    }
    if (srcA.docs.isEmpty() && !dstA.docs.isEmpty())
    {
      srcA.docs = dstA.docs;
    }
    else if (dstA.docs.isEmpty() && !srcA.docs.isEmpty())
    {
      dstA.docs = srcA.docs;
    }
    //printf("Merge argument '%s|%s' '%s|%s'\n",
    //  srcA.type.data(),srcA.name.data(),
    //  dstA.type.data(),dstA.name.data());
    ++srcIt;
    ++dstIt;
  }
}

static void findMembersWithSpecificName(const MemberName *mn,
                                        const char *args,
                                        bool checkStatics,
                                        const FileDef *currentFile,
                                        bool checkCV,
                                        std::vector<const MemberDef *> &members)
{
  //printf("  Function with global scope name '%s' args='%s'\n",
  //       mn->memberName(),args);
  for (const auto &md_p : *mn)
  {
    const MemberDef *md = md_p.get();
    const FileDef  *fd=md->getFileDef();
    const GroupDef *gd=md->getGroupDef();
    //printf("  md->name()='%s' md->args='%s' fd=%p gd=%p current=%p ref=%s\n",
    //    md->name().data(),args,fd,gd,currentFile,md->getReference().data());
    if (
        ((gd && gd->isLinkable()) || (fd && fd->isLinkable()) || md->isReference()) &&
        md->getNamespaceDef()==0 && md->isLinkable() &&
        (!checkStatics || (!md->isStatic() && !md->isDefine()) ||
         currentFile==0 || fd==currentFile) // statics must appear in the same file
       )
    {
      bool match=TRUE;
      if (args && !md->isDefine() && qstrcmp(args,"()")!=0)
      {
        const ArgumentList &mdAl = md->argumentList();
        auto argList_p = stringToArgumentList(md->getLanguage(),args);
        match=matchArguments2(
            md->getOuterScope(),fd,&mdAl,
            Doxygen::globalScope,fd,argList_p.get(),
            checkCV);
      }
      if (match)
      {
        //printf("Found match!\n");
        members.push_back(md);
      }
    }
  }
}

/*!
 * Searches for a member definition given its name 'memberName' as a string.
 * memberName may also include a (partial) scope to indicate the scope
 * in which the member is located.
 *
 * The parameter 'scName' is a string representing the name of the scope in
 * which the link was found.
 *
 * In case of a function args contains a string representation of the
 * argument list. Passing 0 means the member has no arguments.
 * Passing "()" means any argument list will do, but "()" is preferred.
 *
 * The function returns TRUE if the member is known and documented or
 * FALSE if it is not.
 * If TRUE is returned parameter 'md' contains a pointer to the member
 * definition. Furthermore exactly one of the parameter 'cd', 'nd', or 'fd'
 * will be non-zero:
 *   - if 'cd' is non zero, the member was found in a class pointed to by cd.
 *   - if 'nd' is non zero, the member was found in a namespace pointed to by nd.
 *   - if 'fd' is non zero, the member was found in the global namespace of
 *     file fd.
 */
bool getDefs(const QCString &scName,
             const QCString &mbName,
             const char *args,
             const MemberDef *&md,
             const ClassDef *&cd,
             const FileDef *&fd,
             const NamespaceDef *&nd,
             const GroupDef *&gd,
             bool forceEmptyScope,
             const FileDef *currentFile,
             bool checkCV
            )
{
  fd=0, md=0, cd=0, nd=0, gd=0;
  if (mbName.isEmpty()) return FALSE; /* empty name => nothing to link */

  QCString scopeName=scName;
  QCString memberName=mbName;
  scopeName = substitute(scopeName,"\\","::"); // for PHP
  memberName = substitute(memberName,"\\","::"); // for PHP
  //printf("Search for name=%s args=%s in scope=%s forceEmpty=%d\n",
  //          memberName.data(),args,scopeName.data(),forceEmptyScope);

  int is,im=0,pm=0;
  // strip common part of the scope from the scopeName
  while ((is=scopeName.findRev("::"))!=-1 &&
         (im=memberName.find("::",pm))!=-1 &&
          (scopeName.right(scopeName.length()-is-2)==memberName.mid(pm,im-pm))
        )
  {
    scopeName=scopeName.left(is);
    pm=im+2;
  }
  //printf("result after scope corrections scope=%s name=%s\n",
  //          scopeName.data(),memberName.data());

  QCString mName=memberName;
  QCString mScope;
  if (memberName.left(9)!="operator " && // treat operator conversion methods
      // as a special case
      (im=memberName.findRev("::"))!=-1 &&
      im<(int)memberName.length()-2 // not A::
     )
  {
    mScope=memberName.left(im);
    mName=memberName.right(memberName.length()-im-2);
  }

  // handle special the case where both scope name and member scope are equal
  if (mScope==scopeName) scopeName.resize(0);

  //printf("mScope='%s' mName='%s'\n",mScope.data(),mName.data());

  MemberName *mn = Doxygen::memberNameLinkedMap->find(mName);
  //printf("mName=%s mn=%p\n",mName.data(),mn);

  if ((!forceEmptyScope || scopeName.isEmpty()) && // this was changed for bug638856, forceEmptyScope => empty scopeName
      mn && !(scopeName.isEmpty() && mScope.isEmpty()))
  {
    //printf("  >member name '%s' found\n",mName.data());
    int scopeOffset=scopeName.length();
    do
    {
      QCString className = scopeName.left(scopeOffset);
      if (!className.isEmpty() && !mScope.isEmpty())
      {
        className+="::"+mScope;
      }
      else if (!mScope.isEmpty())
      {
        className=mScope;
      }

      SymbolResolver resolver;
      const ClassDef *fcd=resolver.resolveClass(Doxygen::globalScope,className);
      const MemberDef *tmd=resolver.getTypedef();

      if (fcd==0 && className.find('<')!=-1) // try without template specifiers as well
      {
         QCString nameWithoutTemplates = stripTemplateSpecifiersFromScope(className,FALSE);
         fcd=resolver.resolveClass(Doxygen::globalScope,nameWithoutTemplates);
         tmd=resolver.getTypedef();
      }
      //printf("Trying class scope %s: fcd=%p tmd=%p\n",className.data(),fcd,tmd);
      // todo: fill in correct fileScope!
      if (fcd &&  // is it a documented class
          fcd->isLinkable()
         )
      {
        //printf("  Found fcd=%p\n",fcd);
        int mdist=maxInheritanceDepth;
        std::unique_ptr<ArgumentList> argList;
        if (args)
        {
          argList = stringToArgumentList(fcd->getLanguage(),args);
        }
        for (const auto &mmd_p : *mn)
        {
          MemberDef *mmd = mmd_p.get();
          if (!mmd->isStrongEnumValue())
          {
            const ArgumentList &mmdAl = mmd->argumentList();
            bool match=args==0 ||
              matchArguments2(mmd->getOuterScope(),mmd->getFileDef(),&mmdAl,
                             fcd,                  fcd->getFileDef(),argList.get(),
                             checkCV);
            //printf("match=%d\n",match);
            if (match)
            {
              const ClassDef *mcd=mmd->getClassDef();
              if (mcd)
              {
                int m=minClassDistance(fcd,mcd);
                if (m<mdist && mcd->isLinkable())
                {
                  mdist=m;
                  cd=mcd;
                  md=mmd;
                }
              }
            }
          }
        }
        if (mdist==maxInheritanceDepth && args && qstrcmp(args,"()")==0)
          // no exact match found, but if args="()" an arbitrary member will do
        {
          //printf("  >Searching for arbitrary member\n");
          for (const auto &mmd_p : *mn)
          {
            MemberDef *mmd = mmd_p.get();
            //if (mmd->isLinkable())
            //{
            const ClassDef *mcd=mmd->getClassDef();
            //printf("  >Class %s found\n",mcd->name().data());
            if (mcd)
            {
              int m=minClassDistance(fcd,mcd);
              if (m<mdist /* && mcd->isLinkable()*/ )
              {
                //printf("Class distance %d\n",m);
                mdist=m;
                cd=mcd;
                md=mmd;
              }
            }
            //}
          }
        }
        //printf("  >Success=%d\n",mdist<maxInheritanceDepth);
        if (mdist<maxInheritanceDepth)
        {
          if (!md->isLinkable() || md->isStrongEnumValue())
          {
            md=0; // avoid returning things we cannot link to
            cd=0;
            return FALSE; // match found, but was not linkable
          }
          else
          {
            gd=md->getGroupDef();
            if (gd) cd=0;
            return TRUE; /* found match */
          }
        }
      }
      if (tmd && tmd->isEnumerate() && tmd->isStrong()) // scoped enum
      {
        //printf("Found scoped enum!\n");
        for (const auto &emd : tmd->enumFieldList())
        {
          if (emd->localName()==mName)
          {
            if (emd->isLinkable())
            {
              cd=tmd->getClassDef();
              md=emd;
              return TRUE;
            }
            else
            {
              cd=0;
              md=0;
              return FALSE;
            }
          }
        }
      }
      /* go to the parent scope */
      if (scopeOffset==0)
      {
        scopeOffset=-1;
      }
      else if ((scopeOffset=scopeName.findRev("::",scopeOffset-1))==-1)
      {
        scopeOffset=0;
      }
    } while (scopeOffset>=0);

  }
  if (mn && scopeName.isEmpty() && mScope.isEmpty()) // Maybe a related function?
  {
    //printf("Global symbol\n");
    const MemberDef *fuzzy_mmd = 0;
    std::unique_ptr<ArgumentList> argList;
    bool hasEmptyArgs = args && qstrcmp(args, "()") == 0;

    if (args)
    {
      argList = stringToArgumentList(SrcLangExt_Cpp, args);
    }

    for (const auto &mmd_p : *mn)
    {
      const MemberDef *mmd = mmd_p.get();
      if (!mmd->isLinkable() || (!mmd->isRelated() && !mmd->isForeign()) ||
           !mmd->getClassDef())
      {
        continue;
      }

      if (!args)
      {
        fuzzy_mmd = mmd;
        break;
      }

      const ArgumentList &mmdAl = mmd->argumentList();
      if (matchArguments2(mmd->getOuterScope(),mmd->getFileDef(),&mmdAl,
            Doxygen::globalScope,mmd->getFileDef(),argList.get(),
            checkCV
            )
         )
      {
        fuzzy_mmd = mmd;
        break;
      }

      if (!fuzzy_mmd && hasEmptyArgs)
      {
        fuzzy_mmd = mmd;
      }
    }

    if (fuzzy_mmd && !fuzzy_mmd->isStrongEnumValue())
    {
      md = fuzzy_mmd;
      cd = fuzzy_mmd->getClassDef();
      return TRUE;
    }
  }


  // maybe an namespace, file or group member ?
  //printf("Testing for global symbol scopeName='%s' mScope='%s' :: mName='%s'\n",
  //              scopeName.data(),mScope.data(),mName.data());
  if ((mn=Doxygen::functionNameLinkedMap->find(mName))) // name is known
  {
    //printf("  >symbol name found\n");
    NamespaceDef *fnd=0;
    int scopeOffset=scopeName.length();
    do
    {
      QCString namespaceName = scopeName.left(scopeOffset);
      if (!namespaceName.isEmpty() && !mScope.isEmpty())
      {
        namespaceName+="::"+mScope;
      }
      else if (!mScope.isEmpty())
      {
        namespaceName=mScope.copy();
      }
      //printf("Trying namespace %s\n",namespaceName.data());
      if (!namespaceName.isEmpty() &&
          (fnd=Doxygen::namespaceLinkedMap->find(namespaceName)) &&
          fnd->isLinkable()
         )
      {
        //printf("Symbol inside existing namespace '%s' count=%d\n",
        //    namespaceName.data(),mn->count());
        bool found=FALSE;
        for (const auto &mmd_p : *mn)
        {
          const MemberDef *mmd = mmd_p.get();
          //printf("mmd->getNamespaceDef()=%p fnd=%p\n",
          //    mmd->getNamespaceDef(),fnd);
          const MemberDef *emd = mmd->getEnumScope();
          if (emd && emd->isStrong())
          {
            //printf("yes match %s<->%s!\n",mScope.data(),emd->localName().data());
            if (emd->getNamespaceDef()==fnd &&
                rightScopeMatch(mScope,emd->localName()))
            {
              //printf("found it!\n");
              nd=fnd;
              md=mmd;
              found=TRUE;
              break;
            }
            else
            {
              md=0;
              cd=0;
              return FALSE;
            }
          }
          else if (mmd->getOuterScope()==fnd /* && mmd->isLinkable() */ )
          { // namespace is found
            bool match=TRUE;
            if (args && qstrcmp(args,"()")!=0)
            {
              const ArgumentList &mmdAl = mmd->argumentList();
              auto argList_p = stringToArgumentList(mmd->getLanguage(),args);
              match=matchArguments2(
                  mmd->getOuterScope(),mmd->getFileDef(),&mmdAl,
                  fnd,mmd->getFileDef(),argList_p.get(),
                  checkCV);
            }
            if (match)
            {
              nd=fnd;
              md=mmd;
              found=TRUE;
              break;
            }
          }
        }
        if (!found && args && !qstrcmp(args,"()"))
          // no exact match found, but if args="()" an arbitrary
          // member will do
        {
          for (const auto &mmd_p : *mn)
          {
            const MemberDef *mmd = mmd_p.get();
            if (mmd->getNamespaceDef()==fnd /*&& mmd->isLinkable() */ )
            {
              nd=fnd;
              md=mmd;
              found=TRUE;
              break;
            }
          }
        }
        if (found)
        {
          if (!md->isLinkable())
          {
            md=0; // avoid returning things we cannot link to
            nd=0;
            return FALSE; // match found but not linkable
          }
          else
          {
            gd=md->resolveAlias()->getGroupDef();
            if (gd && gd->isLinkable()) nd=0; else gd=0;
            return TRUE;
          }
        }
      }
      else
      {
        //printf("not a namespace\n");
        for (const auto &mmd_p : *mn)
        {
          const MemberDef *mmd = mmd_p.get();
          const MemberDef *tmd = mmd->getEnumScope();
          //printf("try member %s tmd=%s\n",mmd->name().data(),tmd?tmd->name().data():"<none>");
          int ni=namespaceName.findRev("::");
          //printf("namespaceName=%s ni=%d\n",namespaceName.data(),ni);
          bool notInNS = tmd && ni==-1 && tmd->getNamespaceDef()==0 && (mScope.isEmpty() || mScope==tmd->name());
          bool sameNS  = tmd && tmd->getNamespaceDef() && namespaceName.left(ni)==tmd->getNamespaceDef()->name() && namespaceName.mid(ni+2)==tmd->name();
          //printf("notInNS=%d sameNS=%d\n",notInNS,sameNS);
          if (tmd && tmd->isStrong() && // C++11 enum class
              (notInNS || sameNS) &&
              namespaceName.length()>0  // enum is part of namespace so this should not be empty
             )
          {
            md=mmd;
            fd=mmd->getFileDef();
            gd=mmd->getGroupDef();
            if (gd && gd->isLinkable()) fd=0; else gd=0;
            //printf("Found scoped enum %s fd=%p gd=%p\n",
            //    mmd->name().data(),fd,gd);
            return TRUE;
          }
        }
      }
      if (scopeOffset==0)
      {
        scopeOffset=-1;
      }
      else if ((scopeOffset=scopeName.findRev("::",scopeOffset-1))==-1)
      {
        scopeOffset=0;
      }
    } while (scopeOffset>=0);

    //else // no scope => global function
    {
      std::vector<const MemberDef *> members;
      // search for matches with strict static checking
      findMembersWithSpecificName(mn,args,TRUE,currentFile,checkCV,members);
      if (members.empty()) // nothing found
      {
        // search again without strict static checking
        findMembersWithSpecificName(mn,args,FALSE,currentFile,checkCV,members);
      }
      //printf("found %d members\n",members.count());
      if (members.size()!=1 && args && !qstrcmp(args,"()"))
      {
        // no exact match found, but if args="()" an arbitrary
        // member will do
        //MemberNameIterator mni(*mn);
        //for (mni.toLast();(md=mni.current());--mni)
        for (auto it = mn->rbegin(); it!=mn->rend(); ++it)
        {
          const auto &mmd_p = *it;
          const MemberDef *mmd = mmd_p.get();
          //printf("Found member '%s'\n",mmd->name().data());
          //printf("member is linkable mmd->name()='%s'\n",mmd->name().data());
          fd=mmd->getFileDef();
          gd=mmd->getGroupDef();
          const MemberDef *tmd = mmd->getEnumScope();
          if (
              (gd && gd->isLinkable()) || (fd && fd->isLinkable()) ||
              (tmd && tmd->isStrong())
             )
          {
            members.push_back(mmd);
          }
        }
      }
      //printf("found %d candidate members\n",members.count());
      if (!members.empty()) // at least one match
      {
        if (currentFile)
        {
          //printf("multiple results; pick one from file:%s\n", currentFile->name().data());
          for (const auto &rmd : members)
          {
            if (rmd->getFileDef() && rmd->getFileDef()->name() == currentFile->name())
            {
              md = rmd;
              break; // found match in the current file
            }
          }
          if (!md) // member not in the current file
          {
            md=members.back();
          }
        }
        else
        {
          md=members.back();
        }
      }
      if (md && (md->getEnumScope()==0 || !md->getEnumScope()->isStrong()))
           // found a matching global member, that is not a scoped enum value (or uniquely matches)
      {
        fd=md->getFileDef();
        gd=md->getGroupDef();
        //printf("fd=%p gd=%p gd->isLinkable()=%d\n",fd,gd,gd->isLinkable());
        if (gd && gd->isLinkable()) fd=0; else gd=0;
        return TRUE;
      }
    }
  }

  // no nothing found
  return FALSE;
}

/*!
 * Searches for a scope definition given its name as a string via parameter
 * `scope`.
 *
 * The parameter `docScope` is a string representing the name of the scope in
 * which the `scope` string was found.
 *
 * The function returns TRUE if the scope is known and documented or
 * FALSE if it is not.
 * If TRUE is returned exactly one of the parameter `cd`, `nd`
 * will be non-zero:
 *   - if `cd` is non zero, the scope was a class pointed to by cd.
 *   - if `nd` is non zero, the scope was a namespace pointed to by nd.
 */
static bool getScopeDefs(const char *docScope,const char *scope,
    ClassDef *&cd, NamespaceDef *&nd)
{
  cd=0;nd=0;

  QCString scopeName=scope;
  //printf("getScopeDefs: docScope='%s' scope='%s'\n",docScope,scope);
  if (scopeName.isEmpty()) return FALSE;

  bool explicitGlobalScope=FALSE;
  if (scopeName.at(0)==':' && scopeName.at(1)==':')
  {
    scopeName=scopeName.right(scopeName.length()-2);
    explicitGlobalScope=TRUE;
  }
  if (scopeName.isEmpty())
  {
    return FALSE;
  }

  QCString docScopeName=docScope;
  int scopeOffset=explicitGlobalScope ? 0 : docScopeName.length();

  do // for each possible docScope (from largest to and including empty)
  {
    QCString fullName=scopeName.copy();
    if (scopeOffset>0) fullName.prepend(docScopeName.left(scopeOffset)+"::");

    if (((cd=getClass(fullName)) ||         // normal class
         (cd=getClass(fullName+"-p")) //||    // ObjC protocol
         //(cd=getClass(fullName+"-g"))       // C# generic
        ) && cd->isLinkable())
    {
      return TRUE; // class link written => quit
    }
    else if ((nd=Doxygen::namespaceLinkedMap->find(fullName)) && nd->isLinkable())
    {
      return TRUE; // namespace link written => quit
    }
    if (scopeOffset==0)
    {
      scopeOffset=-1;
    }
    else if ((scopeOffset=docScopeName.findRev("::",scopeOffset-1))==-1)
    {
      scopeOffset=0;
    }
  } while (scopeOffset>=0);

  return FALSE;
}

static bool isLowerCase(QCString &s)
{
  uchar *p=(uchar*)s.data();
  if (p==0) return TRUE;
  int c;
  while ((c=*p++)) if (!islower(c)) return FALSE;
  return TRUE;
}

/*! Returns an object to reference to given its name and context
 *  @post return value TRUE implies *resContext!=0 or *resMember!=0
 */
bool resolveRef(/* in */  const char *scName,
    /* in */  const char *name,
    /* in */  bool inSeeBlock,
    /* out */ const Definition **resContext,
    /* out */ const MemberDef  **resMember,
    bool lookForSpecialization,
    const FileDef *currentFile,
    bool checkScope
    )
{
  //printf("resolveRef(scope=%s,name=%s,inSeeBlock=%d)\n",scName,name,inSeeBlock);
  QCString tsName = name;
  //bool memberScopeFirst = tsName.find('#')!=-1;
  QCString fullName = substitute(tsName,"#","::");
  if (fullName.find("anonymous_namespace{")==-1)
  {
    fullName = removeRedundantWhiteSpace(substitute(fullName,".","::",3));
  }
  else
  {
    fullName = removeRedundantWhiteSpace(fullName);
  }

  int bracePos=findParameterList(fullName);
  int endNamePos=bracePos!=-1 ? bracePos : fullName.length();
  int scopePos=fullName.findRev("::",endNamePos);
  bool explicitScope = fullName.left(2)=="::" &&   // ::scope or #scope
                       (scopePos>2 ||              // ::N::A
                        tsName.left(2)=="::" ||    // ::foo in local scope
                        scName==0                  // #foo  in global scope
                       );

  // default result values
  *resContext=0;
  *resMember=0;

  if (bracePos==-1) // simple name
  {
    ClassDef *cd=0;
    NamespaceDef *nd=0;

    // the following if() was commented out for releases in the range
    // 1.5.2 to 1.6.1, but has been restored as a result of bug report 594787.
    if (!inSeeBlock && scopePos==-1 && isLowerCase(tsName))
    { // link to lower case only name => do not try to autolink
      return FALSE;
    }

    //printf("scName=%s fullName=%s\n",scName,fullName.data());

    // check if this is a class or namespace reference
    if (scName!=fullName && getScopeDefs(scName,fullName,cd,nd))
    {
      if (cd) // scope matches that of a class
      {
        *resContext = cd;
      }
      else // scope matches that of a namespace
      {
        ASSERT(nd!=0);
        *resContext = nd;
      }
      return TRUE;
    }
    else if (scName==fullName || (!inSeeBlock && scopePos==-1))
      // nothing to link => output plain text
    {
      //printf("found scName=%s fullName=%s scName==fullName=%d "
      //    "inSeeBlock=%d scopePos=%d!\n",
      //    scName,fullName.data(),scName==fullName,inSeeBlock,scopePos);
      return FALSE;
    }
    // continue search...
  }

  // extract userscope+name
  QCString nameStr=fullName.left(endNamePos);
  if (explicitScope) nameStr=nameStr.mid(2);

  // extract arguments
  QCString argsStr;
  if (bracePos!=-1) argsStr=fullName.right(fullName.length()-bracePos);

  // strip template specifier
  // TODO: match against the correct partial template instantiation
  int templPos=nameStr.find('<');
  bool tryUnspecializedVersion = FALSE;
  if (templPos!=-1 && nameStr.find("operator")==-1)
  {
    int endTemplPos=nameStr.findRev('>');
    if (endTemplPos!=-1)
    {
      if (!lookForSpecialization)
      {
        nameStr=nameStr.left(templPos)+nameStr.right(nameStr.length()-endTemplPos-1);
      }
      else
      {
        tryUnspecializedVersion = TRUE;
      }
    }
  }

  QCString scopeStr=scName;

  const MemberDef    *md = 0;
  const ClassDef     *cd = 0;
  const FileDef      *fd = 0;
  const NamespaceDef *nd = 0;
  const GroupDef     *gd = 0;

  // check if nameStr is a member or global.
  //printf("getDefs(scope=%s,name=%s,args=%s checkScope=%d)\n",
  //    scopeStr.data(),nameStr.data(),argsStr.data(),checkScope);
  if (getDefs(scopeStr,nameStr,argsStr,
        md,cd,fd,nd,gd,
        //scopePos==0 && !memberScopeFirst, // forceEmptyScope
        explicitScope, // replaces prev line due to bug 600829
        currentFile,
        TRUE                              // checkCV
        )
     )
  {
    //printf("after getDefs checkScope=%d nameStr=%s cd=%p nd=%p\n",checkScope,nameStr.data(),cd,nd);
    if (checkScope && md && md->getOuterScope()==Doxygen::globalScope &&
        !md->isStrongEnumValue() &&
        (!scopeStr.isEmpty() || nameStr.find("::")>0))
    {
      // we did find a member, but it is a global one while we were explicitly
      // looking for a scoped variable. See bug 616387 for an example why this check is needed.
      // note we do need to support autolinking to "::symbol" hence the >0
      //printf("not global member!\n");
      *resContext=0;
      *resMember=0;
      return FALSE;
    }
    //printf("after getDefs md=%p cd=%p fd=%p nd=%p gd=%p\n",md,cd,fd,nd,gd);
    if      (md) { *resMember=md; *resContext=md; }
    else if (cd) *resContext=cd;
    else if (nd) *resContext=nd;
    else if (fd) *resContext=fd;
    else if (gd) *resContext=gd;
    else         { *resContext=0; *resMember=0; return FALSE; }
    //printf("member=%s (md=%p) anchor=%s linkable()=%d context=%s\n",
    //    md->name().data(),md,md->anchor().data(),md->isLinkable(),(*resContext)->name().data());
    return TRUE;
  }
  else if (inSeeBlock && !nameStr.isEmpty() && (gd=Doxygen::groupLinkedMap->find(nameStr)))
  { // group link
    *resContext=gd;
    return TRUE;
  }
  else if (tsName.find('.')!=-1) // maybe a link to a file
  {
    bool ambig;
    fd=findFileDef(Doxygen::inputNameLinkedMap,tsName,ambig);
    if (fd && !ambig)
    {
      *resContext=fd;
      return TRUE;
    }
  }

  if (tryUnspecializedVersion)
  {
    return resolveRef(scName,name,inSeeBlock,resContext,resMember,FALSE,0,checkScope);
  }
  if (bracePos!=-1) // Try without parameters as well, could be a constructor invocation
  {
    *resContext=getClass(fullName.left(bracePos));
    if (*resContext)
    {
      return TRUE;
    }
  }
  //printf("resolveRef: %s not found!\n",name);

  return FALSE;
}

QCString linkToText(SrcLangExt lang,const char *link,bool isFileName)
{
  //static bool optimizeOutputJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  QCString result=link;
  if (!result.isEmpty())
  {
    // replace # by ::
    result=substitute(result,"#","::");
    // replace . by ::
    if (!isFileName && result.find('<')==-1) result=substitute(result,".","::",3);
    // strip leading :: prefix if present
    if (result.at(0)==':' && result.at(1)==':')
    {
      result=result.right(result.length()-2);
    }
    QCString sep = getLanguageSpecificSeparator(lang);
    if (sep!="::")
    {
      result=substitute(result,"::",sep);
    }
  }
  return result;
}

#if 0
/*
 * generate a reference to a class, namespace or member.
 * 'scName' is the name of the scope that contains the documentation
 * string that is returned.
 * 'name' is the name that we want to link to.
 * 'name' may have the following formats:
 *    1) "ScopeName"
 *    2) "memberName()"    one of the (overloaded) function or define
 *                         with name memberName.
 *    3) "memberName(...)" a specific (overloaded) function or define
 *                         with name memberName
 *    4) "::name           a global variable or define
 *    4) "\#memberName     member variable, global variable or define
 *    5) ("ScopeName::")+"memberName()"
 *    6) ("ScopeName::")+"memberName(...)"
 *    7) ("ScopeName::")+"memberName"
 * instead of :: the \# symbol may also be used.
 */

bool generateRef(OutputDocInterface &od,const char *scName,
    const char *name,bool inSeeBlock,const char *rt)
{
  //printf("generateRef(scName=%s,name=%s,inSee=%d,rt=%s)\n",scName,name,inSeeBlock,rt);

  Definition *compound;
  MemberDef *md;

  // create default link text
  QCString linkText = linkToText(rt,FALSE);

  if (resolveRef(scName,name,inSeeBlock,&compound,&md))
  {
    if (md && md->isLinkable()) // link to member
    {
      od.writeObjectLink(md->getReference(),
          md->getOutputFileBase(),
          md->anchor(),linkText);
      // generate the page reference (for LaTeX)
      if (!md->isReference())
      {
        writePageRef(od,md->getOutputFileBase(),md->anchor());
      }
      return TRUE;
    }
    else if (compound && compound->isLinkable()) // link to compound
    {
      if (rt==0 && compound->definitionType()==Definition::TypeGroup)
      {
        linkText=((GroupDef *)compound)->groupTitle();
      }
      if (compound && compound->definitionType()==Definition::TypeFile)
      {
        linkText=linkToText(rt,TRUE);
      }
      od.writeObjectLink(compound->getReference(),
          compound->getOutputFileBase(),
          0,linkText);
      if (!compound->isReference())
      {
        writePageRef(od,compound->getOutputFileBase(),0);
      }
      return TRUE;
    }
  }
  od.docify(linkText);
  return FALSE;
}
#endif

bool resolveLink(/* in */ const char *scName,
    /* in */ const char *lr,
    /* in */ bool /*inSeeBlock*/,
    /* out */ const Definition **resContext,
    /* out */ QCString &resAnchor
    )
{
  *resContext=0;

  QCString linkRef=lr;
  QCString linkRefWithoutTemplates = stripTemplateSpecifiersFromScope(linkRef,FALSE);
  //printf("ResolveLink linkRef=%s\n",lr);
  const FileDef  *fd;
  const GroupDef *gd;
  const PageDef  *pd;
  const ClassDef *cd;
  const DirDef   *dir;
  const NamespaceDef *nd;
  const SectionInfo *si=0;
  bool ambig;
  if (linkRef.isEmpty()) // no reference name!
  {
    return FALSE;
  }
  else if ((pd=Doxygen::pageLinkedMap->find(linkRef))) // link to a page
  {
    gd = pd->getGroupDef();
    if (gd)
    {
      if (!pd->name().isEmpty()) si=SectionManager::instance().find(pd->name());
      *resContext=gd;
      if (si) resAnchor = si->label();
    }
    else
    {
      *resContext=pd;
    }
    return TRUE;
  }
  else if ((si=SectionManager::instance().find(linkRef)))
  {
    *resContext=si->definition();
    resAnchor = si->label();
    return TRUE;
  }
  else if ((pd=Doxygen::exampleLinkedMap->find(linkRef))) // link to an example
  {
    *resContext=pd;
    return TRUE;
  }
  else if ((gd=Doxygen::groupLinkedMap->find(linkRef))) // link to a group
  {
    *resContext=gd;
    return TRUE;
  }
  else if ((fd=findFileDef(Doxygen::inputNameLinkedMap,linkRef,ambig)) // file link
      && fd->isLinkable())
  {
    *resContext=fd;
    return TRUE;
  }
  else if ((cd=getClass(linkRef))) // class link
  {
    *resContext=cd;
    resAnchor=cd->anchor();
    return TRUE;
  }
  else if ((cd=getClass(linkRefWithoutTemplates))) // C#/Java generic class link
  {
    *resContext=cd;
    resAnchor=cd->anchor();
    return TRUE;
  }
  else if ((cd=getClass(linkRef+"-p"))) // Obj-C protocol link
  {
    *resContext=cd;
    resAnchor=cd->anchor();
    return TRUE;
  }
//  else if ((cd=getClass(linkRef+"-g"))) // C# generic link
//  {
//    *resContext=cd;
//    resAnchor=cd->anchor();
//    return TRUE;
//  }
  else if ((nd=Doxygen::namespaceLinkedMap->find(linkRef)))
  {
    *resContext=nd;
    return TRUE;
  }
  else if ((dir=Doxygen::dirLinkedMap->find(QFileInfo(linkRef).absFilePath().utf8()+"/"))
      && dir->isLinkable()) // TODO: make this location independent like filedefs
  {
    *resContext=dir;
    return TRUE;
  }
  else // probably a member reference
  {
    const MemberDef *md = 0;
    bool res = resolveRef(scName,lr,TRUE,resContext,&md);
    if (md) resAnchor=md->anchor();
    return res;
  }
}


//----------------------------------------------------------------------
// General function that generates the HTML code for a reference to some
// file, class or member from text 'lr' within the context of class 'clName'.
// This link has the text 'lt' (if not 0), otherwise 'lr' is used as a
// basis for the link's text.
// returns TRUE if a link could be generated.

bool generateLink(OutputDocInterface &od,const char *clName,
    const char *lr,bool inSeeBlock,const char *lt)
{
  //printf("generateLink(clName=%s,lr=%s,lr=%s)\n",clName,lr,lt);
  const Definition *compound = 0;
  //PageDef *pageDef=0;
  QCString anchor,linkText=linkToText(SrcLangExt_Unknown,lt,FALSE);
  //printf("generateLink linkText=%s\n",linkText.data());
  if (resolveLink(clName,lr,inSeeBlock,&compound,anchor))
  {
    if (compound) // link to compound
    {
      if (lt==0 && anchor.isEmpty() &&                      /* compound link */
          compound->definitionType()==Definition::TypeGroup /* is group */
         )
      {
        linkText=(toGroupDef(compound))->groupTitle(); // use group's title as link
      }
      else if (compound->definitionType()==Definition::TypeFile)
      {
        linkText=linkToText(compound->getLanguage(),lt,TRUE);
      }
      od.writeObjectLink(compound->getReference(),
          compound->getOutputFileBase(),anchor,linkText);
      if (!compound->isReference())
      {
        writePageRef(od,compound->getOutputFileBase(),anchor);
      }
    }
    else
    {
      err("%s:%d: Internal error: resolveLink successful but no compound found!",__FILE__,__LINE__);
    }
    return TRUE;
  }
  else // link could not be found
  {
    od.docify(linkText);
    return FALSE;
  }
}

void generateFileRef(OutputDocInterface &od,const char *name,const char *text)
{
  //printf("generateFileRef(%s,%s)\n",name,text);
  QCString linkText = text ? text : name;
  //FileInfo *fi;
  FileDef *fd;
  bool ambig;
  if ((fd=findFileDef(Doxygen::inputNameLinkedMap,name,ambig)) &&
      fd->isLinkable())
    // link to documented input file
    od.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,linkText);
  else
    od.docify(linkText);
}

//----------------------------------------------------------------------

/** Cache element for the file name to FileDef mapping cache. */
struct FindFileCacheElem
{
  FindFileCacheElem(FileDef *fd,bool ambig) : fileDef(fd), isAmbig(ambig) {}
  FileDef *fileDef;
  bool isAmbig;
};

static Cache<std::string,FindFileCacheElem> g_findFileDefCache(5000);

static std::mutex g_findFileDefMutex;

FileDef *findFileDef(const FileNameLinkedMap *fnMap,const char *n,bool &ambig)
{
  ambig=FALSE;
  if (n==0) return 0;

  std::lock_guard<std::mutex> lock(g_findFileDefMutex);

  const int maxAddrSize = 20;
  char addr[maxAddrSize];
  qsnprintf(addr,maxAddrSize,"%p:",(void*)fnMap);
  QCString key = addr;
  key+=n;

  FindFileCacheElem *cachedResult = g_findFileDefCache.find(key.str());
  //printf("key=%s cachedResult=%p\n",key.data(),cachedResult);
  if (cachedResult)
  {
    ambig = cachedResult->isAmbig;
    //printf("cached: fileDef=%p\n",cachedResult->fileDef);
    return cachedResult->fileDef;
  }
  else
  {
    cachedResult = g_findFileDefCache.insert(key.str(),FindFileCacheElem(0,FALSE));
  }

  QCString name=QDir::cleanDirPath(n).utf8();
  QCString path;
  int slashPos;
  const FileName *fn;
  if (name.isEmpty()) goto exit;
  slashPos=QMAX(name.findRev('/'),name.findRev('\\'));
  if (slashPos!=-1)
  {
    path=name.left(slashPos+1);
    name=name.right(name.length()-slashPos-1);
    //printf("path=%s name=%s\n",path.data(),name.data());
  }
  if (name.isEmpty()) goto exit;
  if ((fn=fnMap->find(name)))
  {
    //printf("fn->count()=%d\n",fn->count());
    if (fn->size()==1)
    {
      const std::unique_ptr<FileDef> &fd = fn->front();
      bool isSamePath = Portable::fileSystemIsCaseSensitive() ?
                 fd->getPath().right(path.length())==path :
                 fd->getPath().right(path.length()).lower()==path.lower();
      if (path.isEmpty() || isSamePath)
      {
        cachedResult->fileDef = fd.get();
        return fd.get();
      }
    }
    else // file name alone is ambiguous
    {
      int count=0;
      FileDef *lastMatch=0;
      QCString pathStripped = stripFromIncludePath(path);
      for (const auto &fd_p : *fn)
      {
        FileDef *fd = fd_p.get();
        QCString fdStripPath = stripFromIncludePath(fd->getPath());
        if (path.isEmpty() || fdStripPath.right(pathStripped.length())==pathStripped)
        {
          count++;
          lastMatch=fd;
        }
      }

      ambig=(count>1);
      cachedResult->isAmbig = ambig;
      cachedResult->fileDef = lastMatch;
      return lastMatch;
    }
  }
  else
  {
    //printf("not found!\n");
  }
exit:
  //delete cachedResult;
  return 0;
}

//----------------------------------------------------------------------

QCString showFileDefMatches(const FileNameLinkedMap *fnMap,const char *n)
{
  QCString result;
  QCString name=n;
  QCString path;
  int slashPos=QMAX(name.findRev('/'),name.findRev('\\'));
  if (slashPos!=-1)
  {
    path=name.left(slashPos+1);
    name=name.right(name.length()-slashPos-1);
  }
  const FileName *fn;
  if ((fn=fnMap->find(name)))
  {
    for (const auto &fd : *fn)
    {
      if (path.isEmpty() || fd->getPath().right(path.length())==path)
      {
        result+="   "+fd->absFilePath()+"\n";
      }
    }
  }
  return result;
}

//----------------------------------------------------------------------

QCString substituteKeywords(const QCString &s,const char *title,
         const char *projName,const char *projNum,const char *projBrief)
{
  QCString result = s;
  if (title) result = substitute(result,"$title",title);
  result = substitute(result,"$datetime",dateToString(TRUE));
  result = substitute(result,"$date",dateToString(FALSE));
  result = substitute(result,"$year",yearToString());
  result = substitute(result,"$doxygenversion",getDoxygenVersion());
  result = substitute(result,"$projectname",projName);
  result = substitute(result,"$projectnumber",projNum);
  result = substitute(result,"$projectbrief",projBrief);
  result = substitute(result,"$projectlogo",stripPath(Config_getString(PROJECT_LOGO)));
  return result;
}

//----------------------------------------------------------------------

/*! Returns the character index within \a name of the first prefix
 *  in Config_getList(IGNORE_PREFIX) that matches \a name at the left hand side,
 *  or zero if no match was found
 */
int getPrefixIndex(const QCString &name)
{
  if (name.isEmpty()) return 0;
  const StringVector &sl = Config_getList(IGNORE_PREFIX);
  for (const auto &s : sl)
  {
    const char *ps=s.c_str();
    const char *pd=name.data();
    int i=0;
    while (*ps!=0 && *pd!=0 && *ps==*pd) ps++,pd++,i++;
    if (*ps==0 && *pd!=0)
    {
      return i;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------

bool classHasVisibleChildren(const ClassDef *cd)
{
  BaseClassList bcl;

  if (cd->getLanguage()==SrcLangExt_VHDL) // reverse baseClass/subClass relation
  {
    if (cd->baseClasses().empty()) return FALSE;
    bcl=cd->baseClasses();
  }
  else
  {
    if (cd->subClasses().empty()) return FALSE;
    bcl=cd->subClasses();
  }

  for (const auto &bcd : bcl)
  {
    if (bcd.classDef->isVisibleInHierarchy())
    {
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------

bool hasVisibleRoot(const BaseClassList &bcl)
{
  for (const auto &bcd : bcl)
  {
    const ClassDef *cd=bcd.classDef;
    if (cd->isVisibleInHierarchy()) return true;
    if (hasVisibleRoot(cd->baseClasses())) return true;
  }
  return false;
}

//----------------------------------------------------------------------

// copies the next UTF8 character from input stream into buffer ids
// returns the size of the character in bytes (or 0 if it is invalid)
// the character itself will be copied as a UTF-8 encoded string to ids.
int getUtf8Char(const char *input,char ids[MAX_UTF8_CHAR_SIZE],CaseModifier modifier)
{
  int inputLen=1;
  const unsigned char uc = (unsigned char)*input;
  bool validUTF8Char = false;
  if (uc <= 0xf7)
  {
    const char* pt = input+1;
    int l = 0;
    if ((uc&0x80)==0x00)
    {
      switch (modifier)
      {
        case CaseModifier::None:    ids[0]=*input;                break;
        case CaseModifier::ToUpper: ids[0]=(char)toupper(*input); break;
        case CaseModifier::ToLower: ids[0]=(char)tolower(*input); break;
      }
      l=1; // 0xxx.xxxx => normal single byte ascii character
    }
    else
    {
      ids[ 0 ] = *input;
      if ((uc&0xE0)==0xC0)
      {
        l=2; // 110x.xxxx: >=2 byte character
      }
      if ((uc&0xF0)==0xE0)
      {
        l=3; // 1110.xxxx: >=3 byte character
      }
      if ((uc&0xF8)==0xF0)
      {
        l=4; // 1111.0xxx: >=4 byte character
      }
    }
    validUTF8Char = l>0;
    for (int m=1; m<l && validUTF8Char; ++m)
    {
      unsigned char ct = (unsigned char)*pt;
      if (ct==0 || (ct&0xC0)!=0x80) // invalid unicode character
      {
        validUTF8Char=false;
      }
      else
      {
        ids[ m ] = *pt++;
      }
    }
    if (validUTF8Char) // got a valid unicode character
    {
      ids[ l ] = 0;
      inputLen=l;
    }
  }
  return inputLen;
}


// note that this function is not reentrant due to the use of static growBuf!
QCString escapeCharsInString(const char *name,bool allowDots,bool allowUnderscore)
{
  bool caseSenseNames = Config_getBool(CASE_SENSE_NAMES);
  bool allowUnicodeNames = Config_getBool(ALLOW_UNICODE_NAMES);
  if (name==0) return "";
  GrowBuf growBuf;
  signed char c;
  const char *p=name;
  while ((c=*p++)!=0)
  {
    switch(c)
    {
      case '_': if (allowUnderscore) growBuf.addChar('_'); else growBuf.addStr("__"); break;
      case '-': growBuf.addChar('-');  break;
      case ':': growBuf.addStr("_1"); break;
      case '/': growBuf.addStr("_2"); break;
      case '<': growBuf.addStr("_3"); break;
      case '>': growBuf.addStr("_4"); break;
      case '*': growBuf.addStr("_5"); break;
      case '&': growBuf.addStr("_6"); break;
      case '|': growBuf.addStr("_7"); break;
      case '.': if (allowDots) growBuf.addChar('.'); else growBuf.addStr("_8"); break;
      case '!': growBuf.addStr("_9"); break;
      case ',': growBuf.addStr("_00"); break;
      case ' ': growBuf.addStr("_01"); break;
      case '{': growBuf.addStr("_02"); break;
      case '}': growBuf.addStr("_03"); break;
      case '?': growBuf.addStr("_04"); break;
      case '^': growBuf.addStr("_05"); break;
      case '%': growBuf.addStr("_06"); break;
      case '(': growBuf.addStr("_07"); break;
      case ')': growBuf.addStr("_08"); break;
      case '+': growBuf.addStr("_09"); break;
      case '=': growBuf.addStr("_0a"); break;
      case '$': growBuf.addStr("_0b"); break;
      case '\\': growBuf.addStr("_0c"); break;
      case '@': growBuf.addStr("_0d"); break;
      case ']': growBuf.addStr("_0e"); break;
      case '[': growBuf.addStr("_0f"); break;
      case '#': growBuf.addStr("_0g"); break;
      default:
                if (c<0)
                {
                  char ids[MAX_UTF8_CHAR_SIZE];
                  bool doEscape = true;
                  if (allowUnicodeNames)
                  {
                    int charLen = getUtf8Char(p-1,ids);
                    if (charLen>0)
                    {
                      growBuf.addStr(ids);
                      p+=charLen-1;
                      doEscape = false;
                    }
                  }
                  if (doEscape) // not a valid unicode char or escaping needed
                  {
                    unsigned char id = (unsigned char)c;
                    ids[0]='_';
                    ids[1]='x';
                    ids[2]=hex[id>>4];
                    ids[3]=hex[id&0xF];
                    ids[4]=0;
                    growBuf.addStr(ids);
                  }
                }
                else if (caseSenseNames || !isupper(c))
                {
                  growBuf.addChar(c);
                }
                else
                {
                  growBuf.addChar('_');
                  growBuf.addChar((char)tolower(c));
                }
                break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}

QCString unescapeCharsInString(const char *s)
{
  static bool caseSenseNames = Config_getBool(CASE_SENSE_NAMES);
  QCString result;
  const char *p = s;
  if (p)
  {
    char c;
    while ((c=*p++))
    {
      if (c=='_') // 2 or 3 character escape
      {
        switch (*p)
        {
          case '_': result+=c;   p++; break; // __ -> '_'
          case '1': result+=':'; p++; break; // _1 -> ':'
          case '2': result+='/'; p++; break; // _2 -> '/'
          case '3': result+='<'; p++; break; // _3 -> '<'
          case '4': result+='>'; p++; break; // _4 -> '>'
          case '5': result+='*'; p++; break; // _5 -> '*'
          case '6': result+='&'; p++; break; // _6 -> '&'
          case '7': result+='|'; p++; break; // _7 -> '|'
          case '8': result+='.'; p++; break; // _8 -> '.'
          case '9': result+='!'; p++; break; // _9 -> '!'
          case '0': // 3 character escape
             switch (*(p+1))
             {
               case '0': result+=','; p+=2; break; // _00 -> ','
               case '1': result+=' '; p+=2; break; // _01 -> ' '
               case '2': result+='{'; p+=2; break; // _02 -> '{'
               case '3': result+='}'; p+=2; break; // _03 -> '}'
               case '4': result+='?'; p+=2; break; // _04 -> '?'
               case '5': result+='^'; p+=2; break; // _05 -> '^'
               case '6': result+='%'; p+=2; break; // _06 -> '%'
               case '7': result+='('; p+=2; break; // _07 -> '('
               case '8': result+=')'; p+=2; break; // _08 -> ')'
               case '9': result+='+'; p+=2; break; // _09 -> '+'
               case 'a': result+='='; p+=2; break; // _0a -> '='
               case 'b': result+='$'; p+=2; break; // _0b -> '$'
               case 'c': result+='\\'; p+=2; break;// _0c -> '\'
               case 'd': result+='@'; p+=2; break; // _0d -> '@'
               case 'e': result+=']'; p+=2; break; // _0e -> ']'
               case 'f': result+='['; p+=2; break; // _0f -> '['
               case 'g': result+='#'; p+=2; break; // _0g -> '#'
               default: // unknown escape, just pass underscore character as-is
                 result+=c;
                 break;
             }
             break;
          default:
            if (!caseSenseNames && c>='a' && c<='z') // lower to upper case escape, _a -> 'A'
            {
              result+=(char)toupper(*p);
              p++;
            }
            else // unknown escape, pass underscore character as-is
            {
              result+=c;
            }
            break;
        }
      }
      else // normal character; pass as is
      {
        result+=c;
      }
    }
  }
  return result;
}

static std::unordered_map<std::string,int> g_usedNames;
static std::mutex g_usedNamesMutex;
static int g_usedNamesCount=1;

/*! This function determines the file name on disk of an item
 *  given its name, which could be a class name with template
 *  arguments, so special characters need to be escaped.
 */
QCString convertNameToFile(const char *name,bool allowDots,bool allowUnderscore)
{
  if (name==0 || name[0]=='\0') return "";
  static bool shortNames = Config_getBool(SHORT_NAMES);
  static bool createSubdirs = Config_getBool(CREATE_SUBDIRS);
  QCString result;
  if (shortNames) // use short names only
  {
    std::lock_guard<std::mutex> lock(g_usedNamesMutex);
    auto kv = g_usedNames.find(name);
    uint num=0;
    if (kv!=g_usedNames.end())
    {
      num = kv->second;
    }
    else
    {
      num = g_usedNamesCount;
      g_usedNames.insert(std::make_pair(name,g_usedNamesCount++));
    }
    result.sprintf("a%05d",num);
  }
  else // long names
  {
    result=escapeCharsInString(name,allowDots,allowUnderscore);
    int resultLen = result.length();
    if (resultLen>=128) // prevent names that cannot be created!
    {
      // third algorithm based on MD5 hash
      uchar md5_sig[16];
      QCString sigStr(33);
      MD5Buffer((const unsigned char *)result.data(),resultLen,md5_sig);
      MD5SigToString(md5_sig,sigStr.rawData(),33);
      result=result.left(128-32)+sigStr;
    }
  }
  if (createSubdirs)
  {
    int l1Dir=0,l2Dir=0;

    // compute md5 hash to determine sub directory to use
    uchar md5_sig[16];
    MD5Buffer((const unsigned char *)result.data(),result.length(),md5_sig);
    l1Dir = md5_sig[14]&0xf;
    l2Dir = md5_sig[15];

    result.prepend(QCString().sprintf("d%x/d%02x/",l1Dir,l2Dir));
  }
  //printf("*** convertNameToFile(%s)->%s\n",name,result.data());
  return result;
}

QCString relativePathToRoot(const char *name)
{
  QCString result;
  if (Config_getBool(CREATE_SUBDIRS))
  {
    if (name==0)
    {
      return REL_PATH_TO_ROOT;
    }
    else
    {
      QCString n = name;
      int i = n.findRev('/');
      if (i!=-1)
      {
        result=REL_PATH_TO_ROOT;
      }
    }
  }
  return result;
}

void createSubDirs(QDir &d)
{
  if (Config_getBool(CREATE_SUBDIRS))
  {
    // create 4096 subdirectories
    int l1,l2;
    for (l1=0;l1<16;l1++)
    {
      QCString subdir;
      subdir.sprintf("d%x",l1);
      if (!d.exists(subdir) && !d.mkdir(subdir))
      {
        term("Failed to create output directory '%s'\n",subdir.data());
      }
      for (l2=0;l2<256;l2++)
      {
        QCString subsubdir;
        subsubdir.sprintf("d%x/d%02x",l1,l2);
        if (!d.exists(subsubdir) && !d.mkdir(subsubdir))
        {
          term("Failed to create output directory '%s'\n",subsubdir.data());
        }
      }
    }
  }
}

/*! Input is a scopeName, output is the scopename split into a
 *  namespace part (as large as possible) and a classname part.
 */
void extractNamespaceName(const QCString &scopeName,
    QCString &className,QCString &namespaceName,
    bool allowEmptyClass)
{
  int i,p;
  QCString clName=scopeName;
  NamespaceDef *nd = 0;
  if (!clName.isEmpty() && (nd=getResolvedNamespace(clName)) && getClass(clName)==0)
  { // the whole name is a namespace (and not a class)
    namespaceName=nd->name().copy();
    className.resize(0);
    goto done;
  }
  p=clName.length()-2;
  while (p>=0 && (i=clName.findRev("::",p))!=-1)
    // see if the first part is a namespace (and not a class)
  {
    //printf("Trying %s\n",clName.left(i).data());
    if (i>0 && (nd=getResolvedNamespace(clName.left(i))) && getClass(clName.left(i))==0)
    {
      //printf("found!\n");
      namespaceName=nd->name().copy();
      className=clName.right(clName.length()-i-2);
      goto done;
    }
    p=i-2; // try a smaller piece of the scope
  }
  //printf("not found!\n");

  // not found, so we just have to guess.
  className=scopeName.copy();
  namespaceName.resize(0);

done:
  if (className.isEmpty() && !namespaceName.isEmpty() && !allowEmptyClass)
  {
    // class and namespace with the same name, correct to return the class.
    className=namespaceName.copy();
    namespaceName.resize(0);
  }
  //printf("extractNamespace '%s' => '%s|%s'\n",scopeName.data(),
  //       className.data(),namespaceName.data());
  if (/*className.right(2)=="-g" ||*/ className.right(2)=="-p")
  {
    className = className.left(className.length()-2);
  }
  return;
}

QCString insertTemplateSpecifierInScope(const QCString &scope,const QCString &templ)
{
  QCString result=scope.copy();
  if (!templ.isEmpty() && scope.find('<')==-1)
  {
    int si,pi=0;
    ClassDef *cd=0;
    while (
        (si=scope.find("::",pi))!=-1 && !getClass(scope.left(si)+templ) &&
        ((cd=getClass(scope.left(si)))==0 || cd->templateArguments().empty())
        )
    {
      //printf("Tried '%s'\n",(scope.left(si)+templ).data());
      pi=si+2;
    }
    if (si==-1) // not nested => append template specifier
    {
      result+=templ;
    }
    else // nested => insert template specifier before after first class name
    {
      result=scope.left(si) + templ + scope.right(scope.length()-si);
    }
  }
  //printf("insertTemplateSpecifierInScope('%s','%s')=%s\n",
  //    scope.data(),templ.data(),result.data());
  return result;
}


/*! Strips the scope from a name. Examples: A::B will return A
 *  and A<T>::B<N::C<D> > will return A<T>.
 */
QCString stripScope(const char *name)
{
  QCString result = name;
  int l=result.length();
  int p;
  bool done = FALSE;
  bool skipBracket=FALSE; // if brackets do not match properly, ignore them altogether
  int count=0;
  int round=0;

  do
  {
    p=l-1; // start at the end of the string
    while (p>=0 && count>=0)
    {
      char c=result.at(p);
      switch (c)
      {
        case ':':
          // only exit in the case of ::
          //printf("stripScope(%s)=%s\n",name,result.right(l-p-1).data());
          if (p>0 && result.at(p-1)==':' && (count==0 || skipBracket))
          {
            return result.right(l-p-1);
          }
          p--;
          break;
        case '>':
          if (skipBracket) // we don't care about brackets
          {
            p--;
          }
          else // count open/close brackets
          {
            if (p>0 && result.at(p-1)=='>') // skip >> operator
            {
              p-=2;
              break;
            }
            count=1;
            //printf("pos < = %d\n",p);
            p--;
            bool foundMatch=false;
            while (p>=0 && !foundMatch)
            {
              c=result.at(p--);
              switch (c)
              {
                case ')':
                  round++;
                  break;
                case '(':
                  round--;
                  break;
                case '>': // ignore > inside (...) to support e.g. (sizeof(T)>0) inside template parameters
                  if (round==0) count++;
                  break;
                case '<':
                  if (round==0)
                  {
                    if (p>0)
                    {
                      if (result.at(p-1) == '<') // skip << operator
                      {
                        p--;
                        break;
                      }
                    }
                    count--;
                    foundMatch = count==0;
                  }
                  break;
                default:
                  //printf("c=%c count=%d\n",c,count);
                  break;
              }
            }
          }
          //printf("pos > = %d\n",p+1);
          break;
        default:
          p--;
      }
    }
    done = count==0 || skipBracket; // reparse if brackets do not match
    skipBracket=TRUE;
  }
  while (!done); // if < > unbalanced repeat ignoring them
  //printf("stripScope(%s)=%s\n",name,name);
  return name;
}

/*! Converts a string to a HTML id string */
QCString convertToId(const char *s)
{
  if (s==0) return "";
  GrowBuf growBuf;
  const char *p=s;
  char c;
  bool first=TRUE;
  while ((c=*p++))
  {
    char encChar[4];
    if ((c>='0' && c<='9') || (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='-' || c==':' || c=='.')
    { // any permissive character except _
      if (first && c>='0' && c<='9') growBuf.addChar('a'); // don't start with a digit
      growBuf.addChar(c);
    }
    else
    {
      encChar[0]='_';
      encChar[1]=hex[((unsigned char)c)>>4];
      encChar[2]=hex[((unsigned char)c)&0xF];
      encChar[3]=0;
      growBuf.addStr(encChar);
    }
    first=FALSE;
  }
  growBuf.addChar(0);
  return growBuf.get();
}

/*! Some strings have been corrected but the requirement regarding the fact
 *  that an id cannot have a digit at the first position. To overcome problems
 *  with double labels we always place an "a" in front
 */
QCString correctId(QCString s)
{
  if (s.isEmpty()) return s;
  return "a" + s;
}

/*! Converts a string to an XML-encoded string */
QCString convertToXML(const char *s, bool keepEntities)
{
  if (s==0) return "";
  GrowBuf growBuf;
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '<':  growBuf.addStr("&lt;");   break;
      case '>':  growBuf.addStr("&gt;");   break;
      case '&':  if (keepEntities)
                 {
                   const char *e=p;
                   char ce;
                   while ((ce=*e++))
                   {
                     if (ce==';' || (!(isId(ce) || ce=='#'))) break;
                   }
                   if (ce==';') // found end of an entity
                   {
                     // copy entry verbatim
                     growBuf.addChar(c);
                     while (p<e) growBuf.addChar(*p++);
                   }
                   else
                   {
                     growBuf.addStr("&amp;");
                   }
                 }
                 else
                 {
                   growBuf.addStr("&amp;");
                 }
                 break;
      case '\'': growBuf.addStr("&apos;"); break;
      case '"':  growBuf.addStr("&quot;"); break;
      case  1: case  2: case  3: case  4: case  5: case  6: case  7: case  8:
      case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
      case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
      case 27: case 28: case 29: case 30: case 31:
        break; // skip invalid XML characters (see http://www.w3.org/TR/2000/REC-xml-20001006#NT-Char)
      default:   growBuf.addChar(c);       break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}

/*! Converts a string to an DocBook-encoded string */
QCString convertToDocBook(const char *s)
{
  if (s==0) return "";
  GrowBuf growBuf;
  const unsigned char *q;
  int cnt;
  const unsigned char *p=(const unsigned char *)s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '<':  growBuf.addStr("&lt;");   break;
      case '>':  growBuf.addStr("&gt;");   break;
      case '&':  // possibility to have a special symbol
        q = p;
        cnt = 2; // we have to count & and ; as well
        while ((*q >= 'a' && *q <= 'z') || (*q >= 'A' && *q <= 'Z') || (*q >= '0' && *q <= '9'))
        {
          cnt++;
          q++;
        }
        if (*q == ';')
        {
           --p; // we need & as well
           DocSymbol::SymType res = HtmlEntityMapper::instance()->name2sym(QCString((char *)p).left(cnt));
           if (res == DocSymbol::Sym_Unknown)
           {
             p++;
             growBuf.addStr("&amp;");
           }
           else
           {
             growBuf.addStr(HtmlEntityMapper::instance()->docbook(res));
             q++;
             p = q;
           }
        }
        else
        {
          growBuf.addStr("&amp;");
        }
        break;
      case '\'': growBuf.addStr("&apos;"); break;
      case '"':  growBuf.addStr("&quot;"); break;
      case  1: case  2: case  3: case  4: case  5: case  6: case 7:  case  8:
      case 11: case 12: case 14: case 15: case 16: case 17: case 18:
      case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
      case 27: case 28: case 29: case 30: case 31:
        growBuf.addStr("&#x24");
        growBuf.addChar(hex[static_cast<uchar>(c)>>4]);
        growBuf.addChar(hex[static_cast<uchar>(c)&0xF]);
        growBuf.addChar(';');
        break;
      default:
        growBuf.addChar(c);
        break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}

/*! Converts a string to a HTML-encoded string */
QCString convertToHtml(const char *s,bool keepEntities)
{
  if (s==0) return "";
  GrowBuf growBuf;
  growBuf.addStr(getHtmlDirEmbeddingChar(getTextDirByConfig(s)));
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '<':  growBuf.addStr("&lt;");   break;
      case '>':  growBuf.addStr("&gt;");   break;
      case '&':  if (keepEntities)
                 {
                   const char *e=p;
                   char ce;
                   while ((ce=*e++))
                   {
                     if (ce==';' || (!(isId(ce) || ce=='#'))) break;
                   }
                   if (ce==';') // found end of an entity
                   {
                     // copy entry verbatim
                     growBuf.addChar(c);
                     while (p<e) growBuf.addChar(*p++);
                   }
                   else
                   {
                     growBuf.addStr("&amp;");
                   }
                 }
                 else
                 {
                   growBuf.addStr("&amp;");
                 }
                 break;
      case '\'': growBuf.addStr("&#39;");  break;
      case '"':  growBuf.addStr("&quot;"); break;
      default:
        {
          uchar uc = static_cast<uchar>(c);
          if (uc<32 && !isspace(c))
          {
            growBuf.addStr("&#x24");
            growBuf.addChar(hex[uc>>4]);
            growBuf.addChar(hex[uc&0xF]);
            growBuf.addChar(';');
          }
          else
          {
            growBuf.addChar(c);
          }
        }
        break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}

QCString convertToJSString(const char *s, bool applyTextDir)
{
  if (s==0) return "";
  GrowBuf growBuf;
  if (applyTextDir)
    growBuf.addStr(getJsDirEmbeddingChar(getTextDirByConfig(s)));
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '"':  growBuf.addStr("\\\""); break;
      case '\\': growBuf.addStr("\\\\"); break;
      default:   growBuf.addChar(c);   break;
    }
  }
  growBuf.addChar(0);
  return convertCharEntitiesToUTF8(growBuf.get());
}

QCString convertToPSString(const char *s)
{
  if (s==0) return "";
  GrowBuf growBuf;
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '(':  growBuf.addStr("\\("); break;
      case ')': growBuf.addStr("\\)"); break;
      default:   growBuf.addChar(c);   break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}

QCString convertToLaTeX(const QCString &s,bool insideTabbing,bool keepSpaces)
{
  QGString result;
  FTextStream t(&result);
  filterLatexString(t,s,insideTabbing,false,false,false,keepSpaces);
  return result.data();
}



QCString convertCharEntitiesToUTF8(const char *str)
{
  if (str==0) return QCString();

  static std::regex re("&[[:alpha:]][[:alnum:]]*;");
  std::string s = str;
  std::sregex_iterator it(s.begin(),s.end(),re);
  std::sregex_iterator end;

  GrowBuf growBuf;
  size_t p,i=0,l;
  //while ((p=entityPat.match(s,i,&l))!=-1)
  for (; it!=end ; ++it)
  {
    const auto &match = *it;
    p = match.position();
    l = match.length();
    if (p>i)
    {
      growBuf.addStr(s.substr(i,p-i));
    }
    std::string entity = match.str();
    DocSymbol::SymType symType = HtmlEntityMapper::instance()->name2sym(entity);
    const char *code=0;
    if (symType!=DocSymbol::Sym_Unknown && (code=HtmlEntityMapper::instance()->utf8(symType)))
    {
      growBuf.addStr(code);
    }
    else
    {
      growBuf.addStr(entity);
    }
    i=p+l;
  }
  growBuf.addStr(s.substr(i));
  growBuf.addChar(0);
  //printf("convertCharEntitiesToUTF8(%s)->%s\n",s.data(),growBuf.get());
  return growBuf.get();
}

/*! Returns the standard string that is generated when the \\overload
 * command is used.
 */
QCString getOverloadDocs()
{
  return theTranslator->trOverloadText();
  //"This is an overloaded member function, "
  //       "provided for convenience. It differs from the above "
  //       "function only in what argument(s) it accepts.";
}

void addMembersToMemberGroup(MemberList *ml,
    MemberGroupList *pMemberGroups,
    const Definition *context)
{
  ASSERT(context!=0);
  //printf("addMemberToMemberGroup() context=%s\n",context->name().data());
  if (ml==0) return;

  struct MoveMemberInfo
  {
    MoveMemberInfo(const MemberDef *md,MemberGroup *mg,const RefItemVector &rv)
      : memberDef(md), memberGroup(mg), sli(rv) {}
    const MemberDef *memberDef;
    MemberGroup *memberGroup;
    RefItemVector sli;
  };
  std::vector<MoveMemberInfo> movedMembers;

  for (const auto &md : *ml)
  {
    if (md->isEnumerate()) // insert enum value of this enum into groups
    {
      for (const auto &fmd : md->enumFieldList())
      {
        int groupId=fmd->getMemberGroupId();
        if (groupId!=-1)
        {
          auto it = Doxygen::memberGroupInfoMap.find(groupId);
          if (it!=Doxygen::memberGroupInfoMap.end())
          {
            auto &info = it->second;
            auto mg_it = std::find_if(pMemberGroups->begin(),
                                      pMemberGroups->end(),
                                      [&groupId](const auto &g)
                                      { return g->groupId()==groupId; }
                                     );
            MemberGroup *mg_ptr = 0;
            if (mg_it==pMemberGroups->end())
            {
              auto mg = std::make_unique<MemberGroup>(
                        context,
                        groupId,
                        info->header,
                        info->doc,
                        info->docFile,
                        info->docLine);
              mg_ptr = mg.get();
              pMemberGroups->push_back(std::move(mg));
            }
            else
            {
              mg_ptr = (*mg_it).get();
            }
            mg_ptr->insertMember(fmd); // insert in member group
            MemberDefMutable *fmdm = toMemberDefMutable(fmd);
            if (fmdm)
            {
              fmdm->setMemberGroup(mg_ptr);
            }
          }
        }
      }
    }
    int groupId=md->getMemberGroupId();
    if (groupId!=-1)
    {
      auto it = Doxygen::memberGroupInfoMap.find(groupId);
      if (it!=Doxygen::memberGroupInfoMap.end())
      {
        auto &info = it->second;
        auto mg_it = std::find_if(pMemberGroups->begin(),
                                  pMemberGroups->end(),
                                  [&groupId](const auto &g)
                                  { return g->groupId()==groupId; }
                                 );
        MemberGroup *mg_ptr = 0;
        if (mg_it==pMemberGroups->end())
        {
          auto mg = std::make_unique<MemberGroup>(
                    context,
                    groupId,
                    info->header,
                    info->doc,
                    info->docFile,
                    info->docLine);
          mg_ptr = mg.get();
          pMemberGroups->push_back(std::move(mg));
        }
        else
        {
          mg_ptr = (*mg_it).get();
        }
        movedMembers.push_back(MoveMemberInfo(md,mg_ptr,info->m_sli));
      }
    }
  }

  // move the members to their group
  for (const auto &mmi : movedMembers)
  {
    ml->remove(mmi.memberDef); // remove from member list
    mmi.memberGroup->insertMember(mmi.memberDef->resolveAlias()); // insert in member group
    mmi.memberGroup->setRefItems(mmi.sli);
    MemberDefMutable *rmdm = toMemberDefMutable(mmi.memberDef);
    if (rmdm)
    {
      rmdm->setMemberGroup(mmi.memberGroup);
    }
  }
}

/*! Extracts a (sub-)string from \a type starting at \a pos that
 *  could form a class. The index of the match is returned and the found
 *  class \a name and a template argument list \a templSpec. If -1 is returned
 *  there are no more matches.
 */
int extractClassNameFromType(const char *type,int &pos,QCString &name,QCString &templSpec,SrcLangExt lang)
{
  static std::regex re_norm("[[:alpha:]_][[:alnum:]_:]*");
  static std::regex re_fortran("[[:alpha:]_][[:alnum:]_:()=]*");
  static std::regex &re = re_norm;

  name.resize(0);
  templSpec.resize(0);
  if (type==0) return -1;
  int typeLen=qstrlen(type);
  if (typeLen>0)
  {
    if (lang == SrcLangExt_Fortran)
    {
      if (type[pos]==',') return -1;
      if (QCString(type).left(4).lower()!="type")
      {
        re = re_fortran;
      }
    }
    std::string s = type;
    std::sregex_iterator it(s.begin()+pos,s.end(),re);
    std::sregex_iterator end;

    if (it!=end)
    {
      const auto &match = *it;
      int i = pos+(int)match.position();
      int l = (int)match.length();
      int ts = i+l;
      int te = ts;
      int tl = 0;

      while (ts<typeLen && type[ts]==' ') ts++,tl++; // skip any whitespace
      if (ts<typeLen && type[ts]=='<') // assume template instance
      {
        // locate end of template
        te=ts+1;
        int brCount=1;
        while (te<typeLen && brCount!=0)
        {
          if (type[te]=='<')
          {
            if (te<typeLen-1 && type[te+1]=='<') te++; else brCount++;
          }
          if (type[te]=='>')
          {
            if (te<typeLen-1 && type[te+1]=='>') te++; else brCount--;
          }
          te++;
        }
      }
      name = match.str();
      if (te>ts)
      {
        templSpec = QCString(type).mid(ts,te-ts);
        tl+=te-ts;
        pos=i+l+tl;
      }
      else // no template part
      {
        pos=i+l;
      }
      //printf("extractClassNameFromType([in] type=%s,[out] pos=%d,[out] name=%s,[out] templ=%s)=TRUE i=%d\n",
      //    type,pos,name.data(),templSpec.data(),i);
      return i;
    }
  }
  pos = typeLen;
  //printf("extractClassNameFromType([in] type=%s,[out] pos=%d,[out] name=%s,[out] templ=%s)=FALSE\n",
  //       type,pos,name.data(),templSpec.data());
  return -1;
}

QCString normalizeNonTemplateArgumentsInString(
       const QCString &name,
       const Definition *context,
       const ArgumentList &formalArgs)
{
  // skip until <
  int p=name.find('<');
  if (p==-1) return name;
  p++;
  QCString result = name.left(p);

  static std::regex re("[[:alpha:]_:][[:alnum:]_:]*");
  std::string s = result.mid(p).str();
  std::sregex_iterator it(s.begin(),s.end(),re);
  std::sregex_iterator end;
  size_t pi=0;
  // for each identifier in the template part (e.g. B<T> -> T)
  for (; it!=end ; ++it)
  {
    const auto &match = *it;
    size_t i = match.position();
    size_t l = match.length();
    result += s.substr(pi,i-pi);
    std::string n = match.str();
    bool found=FALSE;
    for (const Argument &formArg : formalArgs)
    {
      if (formArg.name == n)
      {
        found=TRUE;
        break;
      }
    }
    if (!found)
    {
      // try to resolve the type
      SymbolResolver resolver;
      const ClassDef *cd = resolver.resolveClass(context,n.c_str());
      if (cd)
      {
        result+=cd->name();
      }
      else
      {
        result+=n;
      }
    }
    else
    {
      result+=n;
    }
    pi=i+l;
  }
  result+=s.substr(pi);
  //printf("normalizeNonTemplateArgumentInString(%s)=%s\n",name.data(),result.data());
  return removeRedundantWhiteSpace(result);
}


/*! Substitutes any occurrence of a formal argument from argument list
 *  \a formalArgs in \a name by the corresponding actual argument in
 *  argument list \a actualArgs. The result after substitution
 *  is returned as a string. The argument \a name is used to
 *  prevent recursive substitution.
 */
QCString substituteTemplateArgumentsInString(
    const std::string &name,
    const ArgumentList &formalArgs,
    const std::unique_ptr<ArgumentList> &actualArgs)
{
  //printf("substituteTemplateArgumentsInString(name=%s formal=%s actualArg=%s)\n",
  //    name.data(),argListToString(formalArgs).data(),argListToString(actualArgs).data());
  if (formalArgs.empty()) return name;
  std::string result;

  static std::regex re("[[:alpha:]_][[:alnum:]_:]*");
  std::sregex_iterator it(name.begin(),name.end(),re);
  std::sregex_iterator end;
  size_t p=0;

  for (; it!=end ; ++it)
  {
    const auto &match = *it;
    size_t i = match.position();
    size_t l = match.length();
    if (i>p) result += name.substr(p,i-p);
    std::string n = match.str();
    ArgumentList::iterator actIt;
    if (actualArgs)
    {
      actIt = actualArgs->begin();
    }

    // if n is a template argument, then we substitute it
    // for its template instance argument.
    bool found=FALSE;
    for (auto formIt = formalArgs.begin();
        formIt!=formalArgs.end() && !found;
        ++formIt
        )
    {
      Argument formArg = *formIt;
      Argument actArg;
      if (actualArgs && actIt!=actualArgs->end())
      {
        actArg = *actIt;
      }
      if (formArg.type.left(6)=="class " && formArg.name.isEmpty())
      {
        formArg.name = formArg.type.mid(6);
        formArg.type = "class";
      }
      if (formArg.type.left(9)=="typename " && formArg.name.isEmpty())
      {
        formArg.name = formArg.type.mid(9);
        formArg.type = "typename";
      }
      if (formArg.type=="class" || formArg.type=="typename" || formArg.type.left(8)=="template")
      {
        //printf("n=%s formArg->type='%s' formArg->name='%s' formArg->defval='%s'\n",
        //  n.data(),formArg->type.data(),formArg->name.data(),formArg->defval.data());
        //printf(">> n='%s' formArg->name='%s' actArg->type='%s' actArg->name='%s'\n",
        //    n.data(),formArg.name.data(),actIt!=actualArgs.end() ? actIt->type.data() : "",actIt!=actualArgs.end() ? actIt->name.data() : ""
        //    );
        if (formArg.name==n && actualArgs && actIt!=actualArgs->end() && !actArg.type.isEmpty()) // base class is a template argument
        {
          // replace formal argument with the actual argument of the instance
          if (!leftScopeMatch(actArg.type,n))
            // the scope guard is to prevent recursive lockup for
            // template<class A> class C : public<A::T>,
            // where A::T would become A::T::T here,
            // since n==A and actArg->type==A::T
            // see bug595833 for an example
          {
            if (actArg.name.isEmpty())
            {
              result += actArg.type+" ";
              found=TRUE;
            }
            else
              // for case where the actual arg is something like "unsigned int"
              // the "int" part is in actArg->name.
            {
              result += actArg.type+" "+actArg.name+" ";
              found=TRUE;
            }
          }
        }
        else if (formArg.name==n &&
                 (actualArgs==nullptr || actIt==actualArgs->end()) &&
                 !formArg.defval.isEmpty() &&
                 formArg.defval!=name /* to prevent recursion */
            )
        {
          result += substituteTemplateArgumentsInString(formArg.defval.str(),formalArgs,actualArgs)+" ";
          found=TRUE;
        }
      }
      else if (formArg.name==n &&
               (actualArgs==nullptr || actIt==actualArgs->end()) &&
               !formArg.defval.isEmpty() &&
               formArg.defval!=name /* to prevent recursion */
              )
      {
        result += substituteTemplateArgumentsInString(formArg.defval.str(),formalArgs,actualArgs)+" ";
        found=TRUE;
      }
      if (actualArgs && actIt!=actualArgs->end())
      {
        actIt++;
      }
    }
    if (!found)
    {
      result += n;
    }
    p=i+l;
  }
  result+=name.substr(p);
  //printf("      Inheritance relation %s -> %s\n",
  //    name.data(),result.data());
  return QCString(result).stripWhiteSpace();
}


/*! Strips template specifiers from scope \a fullName, except those
 *  that make up specialized classes. The switch \a parentOnly
 *  determines whether or not a template "at the end" of a scope
 *  should be considered, e.g. with \a parentOnly is \c TRUE, \c A<T>::B<S> will
 *  try to strip `<T>` and not `<S>`, while \a parentOnly is \c FALSE will
 *  strip both unless `A<T>` or `B<S>` are specialized template classes.
 */
QCString stripTemplateSpecifiersFromScope(const QCString &fullName,
    bool parentOnly,
    QCString *pLastScopeStripped)
{
  int i=fullName.find('<');
  if (i==-1) return fullName;
  QCString result;
  int p=0;
  int l=fullName.length();
  while (i!=-1)
  {
    //printf("1:result+=%s\n",fullName.mid(p,i-p).data());
    int e=i+1;
    int count=1;
    int round=0;
    while (e<l && count>0)
    {
      char c=fullName.at(e++);
      switch (c)
      {
        case '(': round++; break;
        case ')': if (round>0) round--; break;
        case '<': if (round==0) count++; break;
        case '>': if (round==0) count--; break;
        default:
          break;
      }
    }
    int si= fullName.find("::",e);

    if (parentOnly && si==-1) break;
    // we only do the parent scope, so we stop here if needed

    result+=fullName.mid(p,i-p);
    //printf("  trying %s\n",(result+fullName.mid(i,e-i)).data());
    if (getClass(result+fullName.mid(i,e-i))!=0)
    {
      result+=fullName.mid(i,e-i);
      //printf("  2:result+=%s\n",fullName.mid(i,e-i-1).data());
    }
    else if (pLastScopeStripped)
    {
      //printf("  last stripped scope '%s'\n",fullName.mid(i,e-i).data());
      *pLastScopeStripped=fullName.mid(i,e-i);
    }
    p=e;
    i=fullName.find('<',p);
  }
  result+=fullName.right(l-p);
  //printf("3:result+=%s\n",fullName.right(l-p).data());
  return result;
}

/*! Merges two scope parts together. The parts may (partially) overlap.
 *  Example1: \c A::B and \c B::C will result in \c A::B::C <br>
 *  Example2: \c A and \c B will be \c A::B <br>
 *  Example3: \c A::B and B will be \c A::B
 *
 *  @param leftScope the left hand part of the scope.
 *  @param rightScope the right hand part of the scope.
 *  @returns the merged scope.
 */
QCString mergeScopes(const QCString &leftScope,const QCString &rightScope)
{
  // case leftScope=="A" rightScope=="A::B" => result = "A::B"
  if (leftScopeMatch(rightScope,leftScope)) return rightScope;
  QCString result;
  int i=0,p=leftScope.length();

  // case leftScope=="A::B" rightScope=="B::C" => result = "A::B::C"
  // case leftScope=="A::B" rightScope=="B" => result = "A::B"
  bool found=FALSE;
  while ((i=leftScope.findRev("::",p))>0)
  {
    if (leftScopeMatch(rightScope,leftScope.right(leftScope.length()-i-2)))
    {
      result = leftScope.left(i+2)+rightScope;
      found=TRUE;
    }
    p=i-1;
  }
  if (found) return result;

  // case leftScope=="A" rightScope=="B" => result = "A::B"
  result=leftScope.copy();
  if (!result.isEmpty() && !rightScope.isEmpty()) result+="::";
  result+=rightScope;
  return result;
}

/*! Returns a fragment from scope \a s, starting at position \a p.
 *
 *  @param s the scope name as a string.
 *  @param p the start position (0 is the first).
 *  @param l the resulting length of the fragment.
 *  @returns the location of the fragment, or -1 if non is found.
 */
int getScopeFragment(const QCString &s,int p,int *l)
{
  int sl=s.length();
  int sp=p;
  int count=0;
  bool done;
  if (sp>=sl) return -1;
  while (sp<sl)
  {
    char c=s.at(sp);
    if (c==':') sp++,p++; else break;
  }
  while (sp<sl)
  {
    char c=s.at(sp);
    switch (c)
    {
      case ':': // found next part
        goto found;
      case '<': // skip template specifier
        count=1;sp++;
        done=FALSE;
        while (sp<sl && !done)
        {
          // TODO: deal with << and >> operators!
          c=s.at(sp++);
          switch(c)
          {
            case '<': count++; break;
            case '>': count--; if (count==0) done=TRUE; break;
            default: break;
          }
        }
        break;
      default:
        sp++;
        break;
    }
  }
found:
  *l=sp-p;
  //printf("getScopeFragment(%s,%d)=%s\n",s.data(),p,s.mid(p,*l).data());
  return p;
}

//----------------------------------------------------------------------------

PageDef *addRelatedPage(const char *name,const QCString &ptitle,
    const QCString &doc,
    const char *fileName,
    int docLine,
    int startLine,
    const RefItemVector &sli,
    GroupDef *gd,
    const TagInfo *tagInfo,
    bool xref,
    SrcLangExt lang
    )
{
  PageDef *pd=0;
  //printf("addRelatedPage(name=%s gd=%p)\n",name,gd);
  QCString title=ptitle.stripWhiteSpace();
  if ((pd=Doxygen::pageLinkedMap->find(name)) && !tagInfo)
  {
    if (!xref && !title.isEmpty() && pd->title()!=title)
    {
      warn(fileName,startLine,"multiple use of page label '%s', (other occurrence: %s, line: %d)",
         name,pd->docFile().data(),pd->getStartBodyLine());
    }
    // append documentation block to the page.
    pd->setDocumentation(doc,fileName,docLine);
    //printf("Adding page docs '%s' pi=%p name=%s\n",doc.data(),pd,name);
    // append (x)refitems to the page.
    pd->setRefItems(sli);
  }
  else // new page
  {
    QCString baseName=name;
    if (baseName.right(4)==".tex")
      baseName=baseName.left(baseName.length()-4);
    else if (baseName.right(Doxygen::htmlFileExtension.length())==Doxygen::htmlFileExtension)
      baseName=baseName.left(baseName.length()-Doxygen::htmlFileExtension.length());

    //printf("Appending page '%s'\n",baseName.data());
    pd = Doxygen::pageLinkedMap->add(baseName,
        std::unique_ptr<PageDef>(
           createPageDef(fileName,docLine,baseName,doc,title)));
    pd->setBodySegment(startLine,startLine,-1);

    pd->setRefItems(sli);
    pd->setLanguage(lang);

    if (tagInfo)
    {
      pd->setReference(tagInfo->tagName);
      pd->setFileName(tagInfo->fileName);
    }


    if (gd) gd->addPage(pd);

    if (pd->hasTitle())
    {
      //outputList->writeTitle(pi->name,pi->title);

      // a page name is a label as well!
      QCString file;
      QCString orgFile;
      int line  = -1;
      if (gd)
      {
        file=gd->getOutputFileBase();
        orgFile=gd->getOutputFileBase();
      }
      else
      {
        file=pd->getOutputFileBase();
        orgFile=pd->docFile();
        line = pd->getStartBodyLine();
      }
      const SectionInfo *si = SectionManager::instance().find(pd->name());
      if (si)
      {
        if (si->lineNr() != -1)
        {
          warn(orgFile,line,"multiple use of section label '%s', (first occurrence: %s, line %d)",pd->name().data(),si->fileName().data(),si->lineNr());
        }
        else
        {
          warn(orgFile,line,"multiple use of section label '%s', (first occurrence: %s)",pd->name().data(),si->fileName().data());
        }
      }
      else
      {
        SectionManager::instance().add(pd->name(),
            file,-1,pd->title(),SectionType::Page,0,pd->getReference());
        //printf("si->label='%s' si->definition=%s si->fileName='%s'\n",
        //      si->label.data(),si->definition?si->definition->name().data():"<none>",
        //      si->fileName.data());
        //printf("  SectionInfo: sec=%p sec->fileName=%s\n",si,si->fileName.data());
        //printf("Adding section key=%s si->fileName=%s\n",pageName.data(),si->fileName.data());
      }
    }
  }
  return pd;
}

//----------------------------------------------------------------------------

void addRefItem(const RefItemVector &sli,
    const char *key,
    const char *prefix, const char *name,const char *title,const char *args,const Definition *scope)
{
  //printf("addRefItem(sli=%d,key=%s,prefix=%s,name=%s,title=%s,args=%s)\n",(int)sli.size(),key,prefix,name,title,args);
  if (key && key[0]!='@') // check for @ to skip anonymous stuff (see bug427012)
  {
    for (RefItem *item : sli)
    {
        item->setPrefix(prefix);
        item->setScope(scope);
        item->setName(name);
        item->setTitle(title);
        item->setArgs(args);
        item->setGroup(key);
    }
  }
}

bool recursivelyAddGroupListToTitle(OutputList &ol,const Definition *d,bool root)
{
  if (!d->partOfGroups().empty()) // write list of group to which this definition belongs
  {
    if (root)
    {
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      ol.writeString("<div class=\"ingroups\">");
    }
    bool first=true;
    for (const auto &gd : d->partOfGroups())
    {
      if (!first) { ol.writeString(" &#124; "); } else first=false;
      if (recursivelyAddGroupListToTitle(ol, gd, FALSE))
      {
        ol.writeString(" &raquo; ");
      }
      ol.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),0,gd->groupTitle());
    }
    if (root)
    {
      ol.writeString("</div>");
      ol.popGeneratorState();
    }
    return true;
  }
  return false;
}

void addGroupListToTitle(OutputList &ol,const Definition *d)
{
  recursivelyAddGroupListToTitle(ol,d,TRUE);
}

void filterLatexString(FTextStream &t,const char *str,
    bool insideTabbing,bool insidePre,bool insideItem,bool insideTable,bool keepSpaces)
{
  if (str==0) return;
  //if (strlen(str)<2) stackTrace();
  const unsigned char *p=(const unsigned char *)str;
  const unsigned char *q;
  int cnt;
  unsigned char c;
  unsigned char pc='\0';
  while (*p)
  {
    c=*p++;

    if (insidePre)
    {
      switch(c)
      {
        case 0xef: // handle U+FFFD i.e. "Replacement character" caused by octal: 357 277 275 / hexadecimal 0xef 0xbf 0xbd
                   // the LaTeX command \ucr has been defined in doxygen.sty
          if ((unsigned char)*(p) == 0xbf && (unsigned char)*(p+1) == 0xbd)
          {
            t << "{\\ucr}";
            p += 2;
          }
          else
            t << (char)c;
          break;
        case '\\': t << "\\(\\backslash\\)"; break;
        case '{':  t << "\\{"; break;
        case '}':  t << "\\}"; break;
        case '_':  t << "\\_"; break;
        case '&':  t << "\\&"; break;
        case '%':  t << "\\%"; break;
        case '#':  t << "\\#"; break;
        case '$':  t << "\\$"; break;
        case '"':  t << "\"{}"; break;
        case '-':  t << "-\\/"; break;
        case '^':  insideTable ? t << "\\string^" : t << (char)c;    break;
        case '~':  t << "\\string~";    break;
        case ' ':  if (keepSpaces) t << "~"; else t << ' ';
                   break;
        default:
                   if (c<32) t << ' '; // non printable control character
                   else t << (char)c;
                   break;
      }
    }
    else
    {
      switch(c)
      {
        case 0xef: // handle U+FFFD i.e. "Replacement character" caused by octal: 357 277 275 / hexadecimal 0xef 0xbf 0xbd
                   // the LaTeX command \ucr has been defined in doxygen.sty
          if ((unsigned char)*(p) == 0xbf && (unsigned char)*(p+1) == 0xbd)
          {
            t << "{\\ucr}";
            p += 2;
          }
          else
            t << (char)c;
          break;
        case '#':  t << "\\#";           break;
        case '$':  t << "\\$";           break;
        case '%':  t << "\\%";           break;
        case '^':  t << "$^\\wedge$";    break;
        case '&':  // possibility to have a special symbol
                   q = p;
                   cnt = 2; // we have to count & and ; as well
                   while ((*q >= 'a' && *q <= 'z') || (*q >= 'A' && *q <= 'Z') || (*q >= '0' && *q <= '9'))
                   {
                     cnt++;
                     q++;
                   }
                   if (*q == ';')
                   {
                      --p; // we need & as well
                      DocSymbol::SymType res = HtmlEntityMapper::instance()->name2sym(QCString((char *)p).left(cnt));
                      if (res == DocSymbol::Sym_Unknown)
                      {
                        p++;
                        t << "\\&";
                      }
                      else
                      {
                        t << HtmlEntityMapper::instance()->latex(res);
                        q++;
                        p = q;
                      }
                   }
                   else
                   {
                     t << "\\&";
                   }
                   break;
        case '*':  t << "$\\ast$";       break;
        case '_':  if (!insideTabbing) t << "\\+";
                   t << "\\_";
                   if (!insideTabbing) t << "\\+";
                   break;
        case '{':  t << "\\{";           break;
        case '}':  t << "\\}";           break;
        case '<':  t << "$<$";           break;
        case '>':  t << "$>$";           break;
        case '|':  t << "$\\vert$";      break;
        case '~':  t << "$\\sim$";       break;
        case '[':  if (Config_getBool(PDF_HYPERLINKS) || insideItem)
                     t << "\\mbox{[}";
                   else
                     t << "[";
                   break;
        case ']':  if (pc=='[') t << "$\\,$";
                     if (Config_getBool(PDF_HYPERLINKS) || insideItem)
                       t << "\\mbox{]}";
                     else
                       t << "]";
                   break;
        case '-':  t << "-\\/";
                   break;
        case '\\': t << "\\textbackslash{}";
                   break;
        case '"':  t << "\\char`\\\"{}";
                   break;
        case '`':  t << "\\`{}";
                   break;
        case '\'': t << "\\textquotesingle{}";
                   break;
        case ' ':  if (keepSpaces) { if (insideTabbing) t << "\\>"; else t << '~'; } else t << ' ';
                   break;

        default:
                   //if (!insideTabbing && forceBreaks && c!=' ' && *p!=' ')
                   if (!insideTabbing &&
                       ((c>='A' && c<='Z' && pc!=' ' && !(pc>='A' && pc <= 'Z') && pc!='\0' && *p) || (c==':' && pc!=':') || (pc=='.' && isId(c)))
                      )
                   {
                     t << "\\+";
                   }
                   if (c<32)
                   {
                     t << ' '; // non-printable control character
                   }
                   else
                   {
                     t << (char)c;
                   }
      }
    }
    pc = c;
  }
}

QCString latexEscapeLabelName(const char *s)
{
  if (s==0) return "";
  QGString result;
  QCString tmp(qstrlen(s)+1);
  FTextStream t(&result);
  const char *p=s;
  char c;
  int i;
  while ((c=*p++))
  {
    switch (c)
    {
      case '|': t << "\\texttt{\"|}"; break;
      case '!': t << "\"!"; break;
      case '@': t << "\"@"; break;
      case '%': t << "\\%";       break;
      case '{': t << "\\lcurly{}"; break;
      case '}': t << "\\rcurly{}"; break;
      case '~': t << "````~"; break; // to get it a bit better in index together with other special characters
      // NOTE: adding a case here, means adding it to while below as well!
      default:
        i=0;
        // collect as long string as possible, before handing it to docify
        tmp[i++]=c;
        while ((c=*p) && c!='@' && c!='[' && c!=']' && c!='!' && c!='{' && c!='}' && c!='|')
        {
          tmp[i++]=c;
          p++;
        }
        tmp[i]=0;
        filterLatexString(t,tmp,
                          true,  // insideTabbing
                          false, // insidePre
                          false, // insideItem
                          false, // insideTable
                          false  // keepSpaces
                         );
        break;
    }
  }
  return result.data();
}

QCString latexEscapeIndexChars(const char *s)
{
  if (s==0) return "";
  QGString result;
  QCString tmp(qstrlen(s)+1);
  FTextStream t(&result);
  const char *p=s;
  char c;
  int i;
  while ((c=*p++))
  {
    switch (c)
    {
      case '!': t << "\"!"; break;
      case '"': t << "\"\""; break;
      case '@': t << "\"@"; break;
      case '|': t << "\\texttt{\"|}"; break;
      case '[': t << "["; break;
      case ']': t << "]"; break;
      case '{': t << "\\lcurly{}"; break;
      case '}': t << "\\rcurly{}"; break;
      // NOTE: adding a case here, means adding it to while below as well!
      default:
        i=0;
        // collect as long string as possible, before handing it to docify
        tmp[i++]=c;
        while ((c=*p) && c!='"' && c!='@' && c!='[' && c!=']' && c!='!' && c!='{' && c!='}' && c!='|')
        {
          tmp[i++]=c;
          p++;
        }
        tmp[i]=0;
        filterLatexString(t,tmp.data(),
                          true,   // insideTabbing
                          false,  // insidePre
                          false,  // insideItem
                          false,  // insideTable
                          false   // keepSpaces
                         );
        break;
    }
  }
  return result.data();
}

QCString latexEscapePDFString(const char *s)
{
  if (s==0) return "";
  QGString result;
  FTextStream t(&result);
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '\\': t << "\\textbackslash{}"; break;
      case '{':  t << "\\{"; break;
      case '}':  t << "\\}"; break;
      case '_':  t << "\\_"; break;
      case '%':  t << "\\%"; break;
      case '&':  t << "\\&"; break;
      default:
        t << c;
        break;
    }
  }
  return result.data();
}

QCString latexFilterURL(const char *s)
{
  if (s==0) return "";
  QGString result;
  FTextStream t(&result);
  const signed char *p=(const signed char*)s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '#':  t << "\\#"; break;
      case '%':  t << "\\%"; break;
      case '\\':  t << "\\\\"; break;
      default:
        if (c<0)
        {
          unsigned char id = (unsigned char)c;
          t << "\\%" << hex[id>>4] << hex[id&0xF];
        }
        else
        {
          t << c;
        }
        break;
    }
  }
  return result.data();
}

static std::mutex g_rtfFormatMutex;
static std::unordered_map<std::string,std::string> g_tagMap;
static QCString g_nextTag( "AAAAAAAAAA" );

QCString rtfFormatBmkStr(const char *name)
{
  std::lock_guard<std::mutex> lock(g_rtfFormatMutex);

  // To overcome the 40-character tag limitation, we
  // substitute a short arbitrary string for the name
  // supplied, and keep track of the correspondence
  // between names and strings.
  QCString tag = g_nextTag;
  auto result = g_tagMap.insert( std::make_pair(name, g_nextTag) );

  if (result.second) // new item was added
  {
    // increment the next tag.

    char* nxtTag = g_nextTag.rawData() + g_nextTag.length() - 1;
    for ( unsigned int i = 0; i < g_nextTag.length(); ++i, --nxtTag )
    {
      if ( ( ++(*nxtTag) ) > 'Z' )
      {
        *nxtTag = 'A';
      }
      else
      {
        // Since there was no carry, we can stop now
        break;
      }
    }
  }

  //printf("Name = %s RTF_tag = %s\n",name,(*tag).data());
  return tag;
}

bool checkExtension(const char *fName, const char *ext)
{
  return (QCString(fName).right(QCString(ext).length())==ext);
}

QCString addHtmlExtensionIfMissing(const char *fName)
{
  if (fName==0) return fName;
  const char *p = strchr(fName,'.');
  if (p==nullptr) // no extension
  {
    return QCString(fName)+Doxygen::htmlFileExtension;
  }
  return fName;
}

QCString stripExtensionGeneral(const char *fName, const char *ext)
{
  QCString result=fName;
  if (result.right(QCString(ext).length())==QCString(ext))
  {
    result=result.left(result.length()-QCString(ext).length());
  }
  return result;
}

QCString stripExtension(const char *fName)
{
  return stripExtensionGeneral(fName, Doxygen::htmlFileExtension);
}

void replaceNamespaceAliases(QCString &scope,int i)
{
  while (i>0)
  {
    QCString ns = scope.left(i);
    if (!ns.isEmpty())
    {
      auto it = Doxygen::namespaceAliasMap.find(ns.data());
      if (it!=Doxygen::namespaceAliasMap.end())
      {
        scope=it->second.data()+scope.right(scope.length()-i);
        i=static_cast<int>(it->second.length());
      }
    }
    if (i>0 && ns==scope.left(i)) break;
  }
}

QCString stripPath(const char *s)
{
  QCString result=s;
  int i=result.findRev('/');
  if (i!=-1)
  {
    result=result.mid(i+1);
  }
  i=result.findRev('\\');
  if (i!=-1)
  {
    result=result.mid(i+1);
  }
  return result;
}

/** returns \c TRUE iff string \a s contains word \a w */
bool containsWord(const char *str,const char *word)
{
  if (str==0 || word==0) return false;
  static std::regex re("[[:alpha:]_]+");
  std::string s = str;
  for (std::sregex_iterator it(s.begin(),s.end(),re) ; it!=std::sregex_iterator() ; ++it)
  {
    if (it->str()==word) return true;
  }
  return false;
}

/** removes occurrences of whole \a word from \a sentence,
 *  while keeps internal spaces and reducing multiple sequences of spaces.
 *  Example: sentence=` cat+ catfish cat cat concat cat`, word=`cat` returns: `+ catfish concat`
 */
bool findAndRemoveWord(QCString &sentence,const char *word)
{
  static std::regex re("[^[:alpha:]_]+");
  std::string s = sentence.str();
  std::sregex_token_iterator it(s.begin(),s.end(),re,{-1,0});
  std::sregex_token_iterator end;

  bool found=false;
  std::string result;
  bool keepSpaces=false; // skip leading whitespace
  for (;it!=end;it++)
  {
    std::string part = it->str();
    bool whiteSpaceOnly = std::all_of(part.begin(),part.end(),
                                      [](const auto ch) { return std::isspace(ch); });
    bool matchingWord = part==word;
    if (!matchingWord && (keepSpaces || !whiteSpaceOnly))
    {
      result+=part;
      keepSpaces=!whiteSpaceOnly; // skip sequences of spaces
    }
    else if (matchingWord)
    {
      found=true;
    }
  }

  // trim trailing whitespace
  result.erase(std::find_if(result.rbegin(), result.rend(),
        [](const auto ch) { return !std::isspace(ch); }).base(), result.end());

  sentence = result;
  return found;
}

/** Special version of QCString::stripWhiteSpace() that only strips
 *  completely blank lines.
 *  @param s the string to be stripped
 *  @param docLine the line number corresponding to the start of the
 *         string. This will be adjusted based on the number of lines stripped
 *         from the start.
 *  @returns The stripped string.
 */
QCString stripLeadingAndTrailingEmptyLines(const QCString &s,int &docLine)
{
  const char *p = s.data();
  if (p==0) return 0;

  // search for leading empty lines
  int i=0,li=-1,l=s.length();
  char c;
  while ((c=*p))
  {
    if (c==' ' || c=='\t' || c=='\r') i++,p++;
    else if (c=='\\' && qstrncmp(p,"\\ilinebr",8)==0) i+=8,li=i,p+=8;
    else if (c=='\n') i++,li=i,docLine++,p++;
    else break;
  }

  // search for trailing empty lines
  int b=l-1,bi=-1;
  p=s.data()+b;
  while (b>=0)
  {
    c=*p;
    if (c==' ' || c=='\t' || c=='\r') b--,p--;
    else if (c=='r' && b>=7 && qstrncmp(p-7,"\\ilinebr",8)==0) bi=b-7,b-=8,p-=8;
    else if (c=='\n') bi=b,b--,p--;
    else break;
  }

  // return whole string if no leading or trailing lines where found
  if (li==-1 && bi==-1) return s;

  // return substring
  if (bi==-1) bi=l;
  if (li==-1) li=0;
  if (bi<=li) return 0; // only empty lines
  //printf("docLine='%s' len=%d li=%d bi=%d\n",s.data(),s.length(),li,bi);
  return s.mid(li,bi-li);
}

//--------------------------------------------------------------------------

static std::unordered_map<std::string,int> g_extLookup;

static struct Lang2ExtMap
{
  const char *langName;
  const char *parserName;
  SrcLangExt parserId;
}
g_lang2extMap[] =
{
//  language       parser           parser option
  { "idl",         "c",             SrcLangExt_IDL      },
  { "java",        "c",             SrcLangExt_Java     },
  { "javascript",  "c",             SrcLangExt_JS       },
  { "csharp",      "c",             SrcLangExt_CSharp   },
  { "d",           "c",             SrcLangExt_D        },
  { "php",         "c",             SrcLangExt_PHP      },
  { "objective-c", "c",             SrcLangExt_ObjC     },
  { "c",           "c",             SrcLangExt_Cpp      },
  { "c++",         "c",             SrcLangExt_Cpp      },
  { "slice",       "c",             SrcLangExt_Slice    },
  { "python",      "python",        SrcLangExt_Python   },
  { "fortran",     "fortran",       SrcLangExt_Fortran  },
  { "fortranfree", "fortranfree",   SrcLangExt_Fortran  },
  { "fortranfixed", "fortranfixed", SrcLangExt_Fortran  },
  { "vhdl",        "vhdl",          SrcLangExt_VHDL     },
  { "xml",         "xml",           SrcLangExt_XML      },
  { "sql",         "sql",           SrcLangExt_SQL      },
  { "md",          "md",            SrcLangExt_Markdown },
  { 0,             0,              (SrcLangExt)0        }
};

bool updateLanguageMapping(const QCString &extension,const QCString &language)
{
  const Lang2ExtMap *p = g_lang2extMap;
  QCString langName = language.lower();
  while (p->langName)
  {
    if (langName==p->langName) break;
    p++;
  }
  if (!p->langName) return FALSE;

  // found the language
  SrcLangExt parserId = p->parserId;
  QCString extName = extension.lower();
  if (extName.isEmpty()) return FALSE;
  if (extName.at(0)!='.') extName.prepend(".");
  auto it = g_extLookup.find(extName.str());
  if (it!=g_extLookup.end())
  {
    g_extLookup.erase(it); // language was already register for this ext
  }
  //printf("registering extension %s\n",extName.data());
  g_extLookup.insert(std::make_pair(extName,parserId));
  if (!Doxygen::parserManager->registerExtension(extName,p->parserName))
  {
    err("Failed to assign extension %s to parser %s for language %s\n",
        extName.data(),p->parserName,language.data());
  }
  else
  {
    //msg("Registered extension %s to language parser %s...\n",
    //    extName.data(),language.data());
  }
  return TRUE;
}

void initDefaultExtensionMapping()
{
  // NOTE: when adding an extension, also add the extension in config.xml
  //                  extension      parser id
  updateLanguageMapping(".dox",      "c");
  updateLanguageMapping(".txt",      "c"); // see bug 760836
  updateLanguageMapping(".doc",      "c");
  updateLanguageMapping(".c",        "c");
  updateLanguageMapping(".C",        "c");
  updateLanguageMapping(".cc",       "c");
  updateLanguageMapping(".CC",       "c");
  updateLanguageMapping(".cxx",      "c");
  updateLanguageMapping(".cpp",      "c");
  updateLanguageMapping(".c++",      "c");
  updateLanguageMapping(".ii",       "c");
  updateLanguageMapping(".ixx",      "c");
  updateLanguageMapping(".ipp",      "c");
  updateLanguageMapping(".i++",      "c");
  updateLanguageMapping(".inl",      "c");
  updateLanguageMapping(".h",        "c");
  updateLanguageMapping(".H",        "c");
  updateLanguageMapping(".hh",       "c");
  updateLanguageMapping(".HH",       "c");
  updateLanguageMapping(".hxx",      "c");
  updateLanguageMapping(".hpp",      "c");
  updateLanguageMapping(".h++",      "c");
  updateLanguageMapping(".idl",      "idl");
  updateLanguageMapping(".ddl",      "idl");
  updateLanguageMapping(".odl",      "idl");
  updateLanguageMapping(".java",     "java");
  //updateLanguageMapping(".as",       "javascript"); // not officially supported
  //updateLanguageMapping(".js",       "javascript"); // not officially supported
  updateLanguageMapping(".cs",       "csharp");
  updateLanguageMapping(".d",        "d");
  updateLanguageMapping(".php",      "php");
  updateLanguageMapping(".php4",     "php");
  updateLanguageMapping(".php5",     "php");
  updateLanguageMapping(".inc",      "php");
  updateLanguageMapping(".phtml",    "php");
  updateLanguageMapping(".m",        "objective-c");
  updateLanguageMapping(".M",        "objective-c");
  updateLanguageMapping(".mm",       "c");  // see bug746361
  updateLanguageMapping(".py",       "python");
  updateLanguageMapping(".pyw",      "python");
  updateLanguageMapping(".f",        "fortran");
  updateLanguageMapping(".for",      "fortran");
  updateLanguageMapping(".f90",      "fortran");
  updateLanguageMapping(".f95",      "fortran");
  updateLanguageMapping(".f03",      "fortran");
  updateLanguageMapping(".f08",      "fortran");
  updateLanguageMapping(".f18",      "fortran");
  updateLanguageMapping(".vhd",      "vhdl");
  updateLanguageMapping(".vhdl",     "vhdl");
  updateLanguageMapping(".ucf",      "vhdl");
  updateLanguageMapping(".qsf",      "vhdl");
  updateLanguageMapping(".md",       "md");
  updateLanguageMapping(".markdown", "md");
  updateLanguageMapping(".ice",      "slice");
}

void addCodeOnlyMappings()
{
  updateLanguageMapping(".xml",   "xml");
  updateLanguageMapping(".sql",   "sql");
}

SrcLangExt getLanguageFromFileName(const QCString& fileName)
{
  QFileInfo fi(fileName);
  // we need only the part after the last ".", newer implementations of QFileInfo have 'suffix()' for this.
  QCString extName = fi.extension(FALSE).lower().data();
  if (extName.isEmpty()) extName=".no_extension";
  if (extName.at(0)!='.') extName.prepend(".");
  auto it = g_extLookup.find(extName.str());
  if (it!=g_extLookup.end()) // listed extension
  {
    //printf("getLanguageFromFileName(%s)=%x\n",fi.extension().data(),*pVal);
    return (SrcLangExt)it->second;
  }
  //printf("getLanguageFromFileName(%s) not found!\n",fileName.data());
  return SrcLangExt_Cpp; // not listed => assume C-ish language.
}

QCString getFileNameExtension(QCString fn)
{
  if (fn.isEmpty()) return "";
  int lastDot = fn.findRev('.');
  if (lastDot!=-1) return fn.mid(lastDot);
  return "";
}

//--------------------------------------------------------------------------

static MemberDef *getMemberFromSymbol(const Definition *scope,const FileDef *fileScope,
                                const char *n)
{
  if (scope==0 ||
      (scope->definitionType()!=Definition::TypeClass &&
       scope->definitionType()!=Definition::TypeNamespace
      )
     )
  {
    scope=Doxygen::globalScope;
  }

  QCString name = n;
  if (name.isEmpty())
    return 0; // no name was given

  auto range = Doxygen::symbolMap.find(name);
  if (range.first==range.second)
    return 0; // could not find any matching symbols

  // mostly copied from getResolvedClassRec()
  QCString explicitScopePart;
  int qualifierIndex = computeQualifiedIndex(name);
  if (qualifierIndex!=-1)
  {
    explicitScopePart = name.left(qualifierIndex);
    replaceNamespaceAliases(explicitScopePart,explicitScopePart.length());
    name = name.mid(qualifierIndex+2);
  }
  //printf("explicitScopePart=%s\n",explicitScopePart.data());

  int minDistance = 10000;
  MemberDef *bestMatch = 0;

  for (auto it=range.first; it!=range.second; ++it)
  {
    Definition *d = it->second;
    if (d->definitionType()==Definition::TypeMember)
    {
      SymbolResolver resolver(fileScope);
      int distance = resolver.isAccessibleFromWithExpScope(scope,d,explicitScopePart);
      if (distance!=-1 && distance<minDistance)
      {
        minDistance = distance;
        bestMatch = toMemberDef(d);
        //printf("new best match %s distance=%d\n",bestMatch->qualifiedName().data(),distance);
      }
    }
  }
  return bestMatch;
}

/*! Returns true iff the given name string appears to be a typedef in scope. */
bool checkIfTypedef(const Definition *scope,const FileDef *fileScope,const char *n)
{
  MemberDef *bestMatch = getMemberFromSymbol(scope,fileScope,n);

  if (bestMatch && bestMatch->isTypedef())
    return TRUE; // closest matching symbol is a typedef
  else
    return FALSE;
}

const char *writeUtf8Char(FTextStream &t,const char *s)
{
  uchar c=(uchar)*s++;
  t << (char)c;
  if (c>=0x80) // multibyte character
  {
    if (((uchar)c&0xE0)==0xC0)
    {
      t << *s++; // 11xx.xxxx: >=2 byte character
    }
    if (((uchar)c&0xF0)==0xE0)
    {
      t << *s++; // 111x.xxxx: >=3 byte character
    }
    if (((uchar)c&0xF8)==0xF0)
    {
      t << *s++; // 1111.xxxx: >=4 byte character
    }
    if (((uchar)c&0xFC)==0xF8)
    {
      t << *s++; // 1111.1xxx: >=5 byte character
    }
    if (((uchar)c&0xFE)==0xFC)
    {
      t << *s++; // 1111.1xxx: 6 byte character
    }
  }
  return s;
}

int nextUtf8CharPosition(const QCString &utf8Str,uint len,uint startPos)
{
  int bytes=1;
  if (startPos>=len) return len;
  uchar c = (uchar)utf8Str[startPos];
  if (c>=0x80) // multibyte utf-8 character
  {
    if (((uchar)c&0xE0)==0xC0)
    {
      bytes+=1; // 11xx.xxxx: >=2 byte character
    }
    if (((uchar)c&0xF0)==0xE0)
    {
      bytes+=2; // 111x.xxxx: >=3 byte character
    }
    if (((uchar)c&0xF8)==0xF0)
    {
      bytes+=3; // 1111.xxxx: >=4 byte character
    }
    if (((uchar)c&0xFC)==0xF8)
    {
      bytes+=4; // 1111.1xxx: >=5 byte character
    }
    if (((uchar)c&0xFE)==0xFC)
    {
      bytes+=5; // 1111.1xxx: 6 byte character
    }
  }
  else if (c=='&') // skip over character entities
  {
    int (*matcher)(int) = 0;
    c = (uchar)utf8Str[startPos+bytes];
    if (c=='#') // numerical entity?
    {
      bytes++;
      c = (uchar)utf8Str[startPos+bytes];
      if (c=='x') // hexadecimal entity?
      {
        bytes++;
        matcher = std::isxdigit;
      }
      else // decimal entity
      {
        matcher = std::isdigit;
      }
    }
    else if (std::isalnum(c)) // named entity?
    {
      bytes++;
      matcher = std::isalnum;
    }
    if (matcher)
    {
      while ((c = (uchar)utf8Str[startPos+bytes])!=0 && matcher(c))
      {
        bytes++;
      }
    }
    if (c!=';')
    {
      bytes=1; // not a valid entity, reset bytes counter
    }
  }
  return startPos+bytes;
}

QCString parseCommentAsText(const Definition *scope,const MemberDef *md,
    const QCString &doc,const QCString &fileName,int lineNr)
{
  QGString s;
  if (doc.isEmpty()) return s.data();
  //printf("parseCommentAsText(%s)\n",doc.data());
  FTextStream t(&s);
  DocNode *root = validatingParseDoc(fileName,lineNr,
      (Definition*)scope,(MemberDef*)md,doc,FALSE,FALSE,
      0,FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
  TextDocVisitor *visitor = new TextDocVisitor(t);
  root->accept(visitor);
  delete visitor;
  delete root;
  QCString result = convertCharEntitiesToUTF8(s.data()).stripWhiteSpace();
  int i=0;
  int charCnt=0;
  int l=result.length();
  while ((i=nextUtf8CharPosition(result,l,i))<l)
  {
    charCnt++;
    if (charCnt>=80) break;
  }
  if (charCnt>=80) // try to truncate the string
  {
    while ((i=nextUtf8CharPosition(result,l,i))<l && charCnt<100)
    {
      charCnt++;
      if (result.at(i)==',' ||
          result.at(i)=='.' ||
          result.at(i)=='!' ||
          result.at(i)=='?')
      {
        i++; // we want to be "behind" last inspected character
        break;
      }
    }
  }
  if ( i < l) result=result.left(i)+"...";
  return result.data();
}

//--------------------------------------------------------------------------------------

static QCString expandAliasRec(StringUnorderedSet &aliasesProcessed,
                               const std::string &s,bool allowRecursion=FALSE);

struct Marker
{
  Marker(int p, int n,int s) : pos(p),number(n),size(s) {}
  int pos; // position in the string
  int number; // argument number
  int size; // size of the marker
};

/** For a string \a s that starts with a command name, returns the character
 *  offset within that string representing the first character after the
 *  command. For an alias with argument, this is the offset to the
 *  character just after the argument list.
 *
 *  Examples:
 *  - s=="a b"      returns 1
 *  - s=="a{2,3} b" returns 6
 *  = s=="#"        returns 0
 */
static int findEndOfCommand(const char *s)
{
  const char *p = s;
  char c;
  int i=0;
  if (p)
  {
    while ((c=*p) && isId(c)) p++;
    if (c=='{')
    {
      QCString args = extractAliasArgs(p,0);
      i+=args.length();
    }
    i+=(int)(p-s);
  }
  return i;
}

/** Replaces the markers in an alias definition \a aliasValue
 *  with the corresponding values found in the comma separated argument
 *  list \a argList and the returns the result after recursive alias expansion.
 */
static QCString replaceAliasArguments(StringUnorderedSet &aliasesProcessed,
                                      const QCString &aliasValue,const QCString &argList)
{
  //printf("----- replaceAliasArguments(val=[%s],args=[%s])\n",aliasValue.data(),argList.data());

  // first make a list of arguments from the comma separated argument list
  std::vector<QCString> args;
  int i,l=(int)argList.length();
  int s=0;
  for (i=0;i<l;i++)
  {
    char c = argList.at(i);
    if (c==',' && (i==0 || argList.at(i-1)!='\\'))
    {
      args.push_back(QCString(argList.mid(s,i-s)));
      s=i+1; // start of next argument
    }
    else if (c=='@' || c=='\\')
    {
      // check if this is the start of another aliased command (see bug704172)
      i+=findEndOfCommand(argList.data()+i+1);
    }
  }
  if (l>s) args.push_back(QCString(argList.right(l-s)));
  //printf("found %d arguments\n",args.count());

  // next we look for the positions of the markers and add them to a list
  std::vector<Marker> markerList;
  l = aliasValue.length();
  char pc='\0';
  bool insideMarkerId=false;
  int markerStart=0;
  auto isDigit = [](char c) { return c>='0' && c<='9'; };
  for (i=0;i<=l;i++)
  {
    char c = i<l ? aliasValue.at(i) : '\0';
    if (insideMarkerId && !isDigit(c)) // found end of a markerId
    {
      insideMarkerId = false;
      int markerLen = i-markerStart;
      markerList.push_back(Marker(markerStart-1,
                                  atoi(aliasValue.mid(markerStart,markerLen)),
                                  markerLen+1));
    }
    if (c=='\\' && (pc=='@' || pc=='\\')) // found escaped backslash
    {
      // skip
      pc = '\0';
    }
    else
    {
      if (isDigit(c) && pc=='\\') // found start of a markerId
      {
        insideMarkerId=true;
        markerStart=i;
      }
      pc = c;
    }
  }

  // then we replace the markers with the corresponding arguments in one pass
  QCString result;
  int p=0;
  for (i=0;i<(int)markerList.size();i++)
  {
    const Marker &m = markerList.at(i);
    result+=aliasValue.mid(p,m.pos-p);
    //printf("part before marker %d: '%s'\n",i,aliasValue.mid(p,m->pos-p).data());
    if (m.number>0 && m.number<=(int)args.size()) // valid number
    {
      result+=expandAliasRec(aliasesProcessed,args.at(m.number-1).str(),TRUE);
      //printf("marker index=%d pos=%d number=%d size=%d replacement %s\n",i,m->pos,m->number,m->size,
      //    args.at(m->number-1)->data());
    }
    p=m.pos+m.size; // continue after the marker
  }
  result+=aliasValue.right(l-p); // append remainder
  //printf("string after replacement of markers: '%s'\n",result.data());

  // expand the result again
  result = substitute(result,"\\{","{");
  result = substitute(result,"\\}","}");
  result = expandAliasRec(aliasesProcessed,substitute(result,"\\,",",").str());

  return result;
}

static QCString escapeCommas(const QCString &s)
{
  QGString result;
  const char *p = s.data();
  char c,pc=0;
  while ((c=*p++))
  {
    if (c==',' && pc!='\\')
    {
      result+="\\,";
    }
    else
    {
      result+=c;
    }
    pc=c;
  }
  result+='\0';
  //printf("escapeCommas: '%s'->'%s'\n",s.data(),result.data());
  return result.data();
}

static QCString expandAliasRec(StringUnorderedSet &aliasesProcessed,const std::string &s,bool allowRecursion)
{
  //QCString result;
  std::string result;
  std::regex re("[\\\\@]([[:alpha:]_][[:alnum:]_]*)");
  std::sregex_iterator re_it(s.begin(),s.end(),re);
  std::sregex_iterator end;
  int p = 0;
  //QCString value=s;
  //int i,p=0,l;
  //while ((i=cmdPat.match(value,p,&l))!=-1)
  for ( ; re_it!=end ; ++re_it)
  {
    const auto &match = *re_it;
    int i = (int)match.position();
    int l = (int)match.length();
    if (i>p) result+=s.substr(p,i-p);
    QCString args = extractAliasArgs(s,i+l);
    bool hasArgs = !args.isEmpty();            // found directly after command
    int argsLen = args.length();
    QCString cmd = match[1].str();
    QCString cmdNoArgs = cmd;
    int numArgs=0;
    if (hasArgs)
    {
      numArgs = countAliasArguments(args);
      cmd += QCString().sprintf("{%d}",numArgs);  // alias name + {n}
    }
    auto it = Doxygen::aliasMap.find(cmd.str());

    if (numArgs>1 && it==Doxygen::aliasMap.end())
    { // in case there is no command with numArgs parameters, but there is a command with 1 parameter,
      // we also accept all text as the argument of that command (so you don't have to escape commas)
      it = Doxygen::aliasMap.find((cmdNoArgs+"{1}").str());
      if (it!=Doxygen::aliasMap.end())
      {
        cmd = cmdNoArgs+"{1}";
        args = escapeCommas(args); // escape , so that everything is seen as one argument
      }
    }
    //printf("Found command s='%s' cmd='%s' numArgs=%d args='%s' aliasText=%s\n",
    //    s.data(),cmd.data(),numArgs,args.data(),it!=Doxygen::aliasMap.end()?it->second.c_str():"<none>");
    if ((allowRecursion || aliasesProcessed.find(cmd.str())==aliasesProcessed.end()) &&
        it!=Doxygen::aliasMap.end()) // expand the alias
    {
      //printf("is an alias!\n");
      if (!allowRecursion) aliasesProcessed.insert(cmd.str());
      QCString val = it->second;
      if (hasArgs)
      {
        val = replaceAliasArguments(aliasesProcessed,val,args);
        //printf("replace '%s'->'%s' args='%s'\n",
        //       aliasText->data(),val.data(),args.data());
      }
      result+=expandAliasRec(aliasesProcessed,val.str()).str();
      if (!allowRecursion) aliasesProcessed.erase(cmd.str());
      p=i+l;
      if (hasArgs) p+=argsLen+2;
    }
    else // command is not an alias
    {
      //printf("not an alias!\n");
      result+=match.str();
      p=i+l;
    }
  }
  result+=s.substr(p);

  //printf("expandAliases '%s'->'%s'\n",s.data(),result.data());
  return result;
}


int countAliasArguments(const QCString argList)
{
  int count=1;
  int l = argList.length();
  int i;
  for (i=0;i<l;i++)
  {
    char c = argList.at(i);
    if (c==',' && (i==0 || argList.at(i-1)!='\\')) count++;
    else if (c=='@' || c=='\\')
    {
      // check if this is the start of another aliased command (see bug704172)
      i+=findEndOfCommand(argList.data()+i+1);
    }
  }
  //printf("countAliasArguments=%d\n",count);
  return count;
}

QCString extractAliasArgs(const QCString &args,int pos)
{
  int i;
  int bc=0;
  char prevChar=0;
  if (args.at(pos)=='{') // alias has argument
  {
    for (i=pos;i<(int)args.length();i++)
    {
      if (prevChar!='\\')
      {
        if (args.at(i)=='{') bc++;
        if (args.at(i)=='}') bc--;
        prevChar=args.at(i);
      }
      else
      {
        prevChar=0;
      }

      if (bc==0)
      {
        //printf("extractAliasArgs('%s')->'%s'\n",args.data(),args.mid(pos+1,i-pos-1).data());
        return args.mid(pos+1,i-pos-1);
      }
    }
  }
  return "";
}

QCString resolveAliasCmd(const QCString aliasCmd)
{
  QCString result;
  StringUnorderedSet aliasesProcessed;
  //printf("Expanding: '%s'\n",aliasCmd.data());
  result = expandAliasRec(aliasesProcessed,aliasCmd.str());
  //printf("Expanding result: '%s'->'%s'\n",aliasCmd.data(),result.data());
  return result;
}

std::string expandAlias(const std::string &aliasName,const std::string &aliasValue)
{
  QCString result;
  StringUnorderedSet aliasesProcessed;
  // avoid expanding this command recursively
  aliasesProcessed.insert(aliasName);
  // expand embedded commands
  //printf("Expanding: '%s'->'%s'\n",aliasName.data(),aliasValue.data());
  result = expandAliasRec(aliasesProcessed,aliasValue.c_str());
  //printf("Expanding result: '%s'->'%s'\n",aliasName.data(),result.data());
  return result.str();
}

void writeTypeConstraints(OutputList &ol,const Definition *d,const ArgumentList &al)
{
  if (al.empty()) return;
  ol.startConstraintList(theTranslator->trTypeConstraints());
  for (const Argument &a : al)
  {
    ol.startConstraintParam();
    ol.parseText(a.name);
    ol.endConstraintParam();
    ol.startConstraintType();
    linkifyText(TextGeneratorOLImpl(ol),d,0,0,a.type);
    ol.endConstraintType();
    ol.startConstraintDocs();
    ol.generateDoc(d->docFile(),d->docLine(),d,0,a.docs,TRUE,FALSE,
                   0,FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
    ol.endConstraintDocs();
  }
  ol.endConstraintList();
}

//----------------------------------------------------------------------------

void stackTrace()
{
#ifdef TRACINGSUPPORT
  void *backtraceFrames[128];
  int frameCount = backtrace(backtraceFrames, 128);
  static char cmd[40960];
  char *p = cmd;
  p += sprintf(p,"/usr/bin/atos -p %d ", (int)getpid());
  for (int x = 0; x < frameCount; x++)
  {
    p += sprintf(p,"%p ", backtraceFrames[x]);
  }
  fprintf(stderr,"========== STACKTRACE START ==============\n");
  if (FILE *fp = Portable::popen(cmd, "r"))
  {
    char resBuf[512];
    while (size_t len = fread(resBuf, 1, sizeof(resBuf), fp))
    {
      fwrite(resBuf, 1, len, stderr);
    }
    Portable::pclose(fp);
  }
  fprintf(stderr,"============ STACKTRACE END ==============\n");
  //fprintf(stderr,"%s\n", frameStrings[x]);
#endif
}

static int transcodeCharacterBuffer(const char *fileName,BufStr &srcBuf,int size,
           const char *inputEncoding,const char *outputEncoding)
{
  if (inputEncoding==0 || outputEncoding==0) return size;
  if (qstricmp(inputEncoding,outputEncoding)==0) return size;
  void *cd = portable_iconv_open(outputEncoding,inputEncoding);
  if (cd==(void *)(-1))
  {
    term("unsupported character conversion: '%s'->'%s': %s\n"
        "Check the INPUT_ENCODING setting in the config file!\n",
        inputEncoding,outputEncoding,strerror(errno));
  }
  int tmpBufSize=size*4+1;
  BufStr tmpBuf(tmpBufSize);
  size_t iLeft=size;
  size_t oLeft=tmpBufSize;
  char *srcPtr = srcBuf.data();
  char *dstPtr = tmpBuf.data();
  uint newSize=0;
  if (!portable_iconv(cd, &srcPtr, &iLeft, &dstPtr, &oLeft))
  {
    newSize = tmpBufSize-(int)oLeft;
    srcBuf.shrink(newSize);
    strncpy(srcBuf.data(),tmpBuf.data(),newSize);
    //printf("iconv: input size=%d output size=%d\n[%s]\n",size,newSize,srcBuf.data());
  }
  else
  {
    term("%s: failed to translate characters from %s to %s: check INPUT_ENCODING\n",
        fileName,inputEncoding,outputEncoding);
  }
  portable_iconv_close(cd);
  return newSize;
}

//! read a file name \a fileName and optionally filter and transcode it
bool readInputFile(const char *fileName,BufStr &inBuf,bool filter,bool isSourceCode)
{
  // try to open file
  int size=0;
  //uint oldPos = dest.curPos();
  //printf(".......oldPos=%d\n",oldPos);

  QFileInfo fi(fileName);
  if (!fi.exists()) return FALSE;
  QCString filterName = getFileFilter(fileName,isSourceCode);
  if (filterName.isEmpty() || !filter)
  {
    QFile f(fileName);
    if (!f.open(IO_ReadOnly))
    {
      err("could not open file %s\n",fileName);
      return FALSE;
    }
    size=fi.size();
    // read the file
    inBuf.skip(size);
    if (f.readBlock(inBuf.data()/*+oldPos*/,size)!=size)
    {
      err("problems while reading file %s\n",fileName);
      return FALSE;
    }
  }
  else
  {
    QCString cmd=filterName+" \""+fileName+"\"";
    Debug::print(Debug::ExtCmd,0,"Executing popen(`%s`)\n",qPrint(cmd));
    FILE *f=Portable::popen(cmd,"r");
    if (!f)
    {
      err("could not execute filter %s\n",filterName.data());
      return FALSE;
    }
    const int bufSize=1024;
    char buf[bufSize];
    int numRead;
    while ((numRead=(int)fread(buf,1,bufSize,f))>0)
    {
      //printf(">>>>>>>>Reading %d bytes\n",numRead);
      inBuf.addArray(buf,numRead),size+=numRead;
    }
    Portable::pclose(f);
    inBuf.at(inBuf.curPos()) ='\0';
    Debug::print(Debug::FilterOutput, 0, "Filter output\n");
    Debug::print(Debug::FilterOutput,0,"-------------\n%s\n-------------\n",qPrint(inBuf));
  }

  int start=0;
  if (size>=2 &&
      ((uchar)inBuf.at(0)==0xFF && (uchar)inBuf.at(1)==0xFE) // Little endian BOM
     ) // UCS-2LE encoded file
  {
    transcodeCharacterBuffer(fileName,inBuf,inBuf.curPos(),
        "UCS-2LE","UTF-8");
  }
  else if (size>=2 &&
           ((uchar)inBuf.at(0)==0xFE && (uchar)inBuf.at(1)==0xFF) // big endian BOM
         ) // UCS-2BE encoded file
  {
    transcodeCharacterBuffer(fileName,inBuf,inBuf.curPos(),
        "UCS-2BE","UTF-8");
  }
  else if (size>=3 &&
           (uchar)inBuf.at(0)==0xEF &&
           (uchar)inBuf.at(1)==0xBB &&
           (uchar)inBuf.at(2)==0xBF
     ) // UTF-8 encoded file
  {
    inBuf.dropFromStart(3); // remove UTF-8 BOM: no translation needed
  }
  else // transcode according to the INPUT_ENCODING setting
  {
    // do character transcoding if needed.
    transcodeCharacterBuffer(fileName,inBuf,inBuf.curPos(),
        Config_getString(INPUT_ENCODING),"UTF-8");
  }

  //inBuf.addChar('\n'); /* to prevent problems under Windows ? */

  // and translate CR's
  size=inBuf.curPos()-start;
  int newSize=filterCRLF(inBuf.data()+start,size);
  //printf("filter char at %p size=%d newSize=%d\n",dest.data()+oldPos,size,newSize);
  if (newSize!=size) // we removed chars
  {
    inBuf.shrink(newSize); // resize the array
    //printf(".......resizing from %d to %d result=[%s]\n",oldPos+size,oldPos+newSize,dest.data());
  }
  inBuf.addChar(0);
  return TRUE;
}

// Replace %word by word in title
QCString filterTitle(const std::string &title)
{
  std::string tf;
  std::regex re("%([A-Z_a-z]+)");
  std::sregex_iterator it(title.begin(),title.end(),re);
  std::sregex_iterator end;
  size_t p = 0;
  for (; it!=end ; ++it)
  {
    const auto &match = *it;
    size_t i = match.position();
    size_t l = match.length();
    if (i>p) tf+=title.substr(p,i-p);
    tf+=match[1].str(); // skip %
    p=i+l;
  }
  tf+=title.substr(p);
  return tf;
}

//----------------------------------------------------------------------------
// returns TRUE if the name of the file represented by 'fi' matches
// one of the file patterns in the 'patList' list.

bool patternMatch(const QFileInfo &fi,const StringVector &patList)
{
  bool caseSenseNames = Config_getBool(CASE_SENSE_NAMES);
  bool found = FALSE;

  // For platforms where the file system is non case sensitive overrule the setting
  if (!Portable::fileSystemIsCaseSensitive())
  {
    caseSenseNames = FALSE;
  }

  if (!patList.empty())
  {
    std::string fn = fi.fileName().data();
    std::string fp = fi.filePath().data();
    std::string afp= fi.absFilePath().data();

    for (auto pattern: patList)
    {
      if (!pattern.empty())
      {
        size_t i=pattern.find('=');
        if (i!=std::string::npos) pattern=pattern.substr(0,i); // strip of the extension specific filter name

        found = isMatchingWildcard(fn,pattern,caseSenseNames)  ||
                isMatchingWildcard(fp,pattern,caseSenseNames)  ||
                isMatchingWildcard(afp,pattern,caseSenseNames);
        if (found) break;
        //printf("Matching '%s' against pattern '%s' found=%d\n",
        //    fi->fileName().data(),pattern.data(),found);
      }
    }
  }
  return found;
}

QCString externalLinkTarget(const bool parent)
{
  static bool extLinksInWindow = Config_getBool(EXT_LINKS_IN_WINDOW);
  if (extLinksInWindow)
    return "target=\"_blank\" ";
  else if (parent)
    return "target=\"_parent\" ";
  else
    return "";
}

QCString externalRef(const QCString &relPath,const QCString &ref,bool href)
{
  QCString result;
  if (!ref.isEmpty())
  {
    auto it = Doxygen::tagDestinationMap.find(ref.str());
    if (it!=Doxygen::tagDestinationMap.end())
    {
      result = it->second;
      int l = result.length();
      if (!relPath.isEmpty() && l>0 && result.at(0)=='.')
      { // relative path -> prepend relPath.
        result.prepend(relPath);
        l+=relPath.length();
      }
      if (l>0 && result.at(l-1)!='/') result+='/';
      if (!href) result.append("\" ");
    }
  }
  else
  {
    result = relPath;
  }
  return result;
}

/** Writes the intensity only bitmap represented by \a data as an image to
 *  directory \a dir using the colors defined by HTML_COLORSTYLE_*.
 */
void writeColoredImgData(const char *dir,ColoredImgDataItem data[])
{
  static int hue   = Config_getInt(HTML_COLORSTYLE_HUE);
  static int sat   = Config_getInt(HTML_COLORSTYLE_SAT);
  static int gamma = Config_getInt(HTML_COLORSTYLE_GAMMA);
  while (data->name)
  {
    QCString fileName;
    fileName=(QCString)dir+"/"+data->name;
    QFile f(fileName);
    if (f.open(IO_WriteOnly))
    {
      ColoredImage img(data->width,data->height,data->content,data->alpha,
                       sat,hue,gamma);
      img.save(fileName);
    }
    else
    {
      fprintf(stderr,"Warning: Cannot open file %s for writing\n",data->name);
    }
    Doxygen::indexList->addImageFile(data->name);
    data++;
  }
}

/** Replaces any markers of the form \#\#AA in input string \a str
 *  by new markers of the form \#AABBCC, where \#AABBCC represents a
 *  valid color, based on the intensity represented by hex number AA
 *  and the current HTML_COLORSTYLE_* settings.
 */
QCString replaceColorMarkers(const char *str)
{
  if (str==0) return QCString();
  std::string result;
  std::string s=str;
  static std::regex re("##([0-9A-Fa-f][0-9A-Fa-f])");
  std::sregex_iterator it(s.begin(),s.end(),re);
  std::sregex_iterator end;
  static int hue   = Config_getInt(HTML_COLORSTYLE_HUE);
  static int sat   = Config_getInt(HTML_COLORSTYLE_SAT);
  static int gamma = Config_getInt(HTML_COLORSTYLE_GAMMA);
  size_t sl=s.length();
  size_t p=0;
  for (; it!=end ; ++it)
  {
    const auto &match = *it;
    size_t i = match.position();
    size_t l = match.length();
    if (i>p) result+=s.substr(p,i-p);
    std::string lumStr = match[1].str();
#define HEXTONUM(x) (((x)>='0' && (x)<='9') ? ((x)-'0') :       \
                     ((x)>='a' && (x)<='f') ? ((x)-'a'+10) :    \
                     ((x)>='A' && (x)<='F') ? ((x)-'A'+10) : 0)

    double r,g,b;
    int red,green,blue;
    int level = HEXTONUM(lumStr[0])*16+HEXTONUM(lumStr[1]);
    ColoredImage::hsl2rgb(hue/360.0,sat/255.0,
                          pow(level/255.0,gamma/100.0),&r,&g,&b);
    red   = (int)(r*255.0);
    green = (int)(g*255.0);
    blue  = (int)(b*255.0);
    char colStr[8];
    colStr[0]='#';
    colStr[1]=hex[red>>4];
    colStr[2]=hex[red&0xf];
    colStr[3]=hex[green>>4];
    colStr[4]=hex[green&0xf];
    colStr[5]=hex[blue>>4];
    colStr[6]=hex[blue&0xf];
    colStr[7]=0;
    //printf("replacing %s->%s (level=%d)\n",lumStr.data(),colStr,level);
    result+=colStr;
    p=i+l;
  }
  if (p<sl) result+=s.substr(p);
  return result;
}

/** Copies the contents of file with name \a src to the newly created
 *  file with name \a dest. Returns TRUE if successful.
 */
bool copyFile(const QCString &src,const QCString &dest)
{
  QFile sf(src);
  if (sf.open(IO_ReadOnly))
  {
    QFileInfo fi(src);
    QFile df(dest);
    if (df.open(IO_WriteOnly))
    {
      char *buffer = new char[fi.size()];
      sf.readBlock(buffer,fi.size());
      df.writeBlock(buffer,fi.size());
      df.flush();
      delete[] buffer;
    }
    else
    {
      err("could not write to file %s\n",dest.data());
      return FALSE;
    }
  }
  else
  {
    err("could not open user specified file %s\n",src.data());
    return FALSE;
  }
  return TRUE;
}

/** Returns the section of text, in between a pair of markers.
 *  Full lines are returned, excluding the lines on which the markers appear.
 *  \sa routine lineBlock
 */
QCString extractBlock(const QCString text,const QCString marker)
{
  QCString result;
  int p=0,i;
  bool found=FALSE;

  // find the character positions of the markers
  int m1 = text.find(marker);
  if (m1==-1) return result;
  int m2 = text.find(marker,m1+marker.length());
  if (m2==-1) return result;

  // find start and end line positions for the markers
  int l1=-1,l2=-1;
  while (!found && (i=text.find('\n',p))!=-1)
  {
    found = (p<=m1 && m1<i); // found the line with the start marker
    p=i+1;
  }
  l1=p;
  int lp=i;
  if (found)
  {
    while ((i=text.find('\n',p))!=-1)
    {
      if (p<=m2 && m2<i) // found the line with the end marker
      {
        l2=p;
        break;
      }
      p=i+1;
      lp=i;
    }
  }
  if (l2==-1) // marker at last line without newline (see bug706874)
  {
    l2=lp;
  }
  //printf("text=[%s]\n",text.mid(l1,l2-l1).data());
  return l2>l1 ? text.mid(l1,l2-l1) : QCString();
}

/** Returns the line number of the line following the line with the marker.
 *  \sa routine extractBlock
 */
int lineBlock(const QCString text,const QCString marker)
{
  int result = 1;
  int p=0,i;
  bool found=FALSE;

  // find the character positions of the first marker
  int m1 = text.find(marker);
  if (m1==-1) return result;

  // find start line positions for the markers
  while (!found && (i=text.find('\n',p))!=-1)
  {
    found = (p<=m1 && m1<i); // found the line with the start marker
    p=i+1;
    result++;
  }
  return result;
}

/** Returns a string representation of \a lang. */
QCString langToString(SrcLangExt lang)
{
  switch(lang)
  {
    case SrcLangExt_Unknown:  return "Unknown";
    case SrcLangExt_IDL:      return "IDL";
    case SrcLangExt_Java:     return "Java";
    case SrcLangExt_CSharp:   return "C#";
    case SrcLangExt_D:        return "D";
    case SrcLangExt_PHP:      return "PHP";
    case SrcLangExt_ObjC:     return "Objective-C";
    case SrcLangExt_Cpp:      return "C++";
    case SrcLangExt_JS:       return "JavaScript";
    case SrcLangExt_Python:   return "Python";
    case SrcLangExt_Fortran:  return "Fortran";
    case SrcLangExt_VHDL:     return "VHDL";
    case SrcLangExt_XML:      return "XML";
    case SrcLangExt_SQL:      return "SQL";
    case SrcLangExt_Markdown: return "Markdown";
    case SrcLangExt_Slice:    return "Slice";
  }
  return "Unknown";
}

/** Returns the scope separator to use given the programming language \a lang */
QCString getLanguageSpecificSeparator(SrcLangExt lang,bool classScope)
{
  if (lang==SrcLangExt_Java || lang==SrcLangExt_CSharp || lang==SrcLangExt_VHDL || lang==SrcLangExt_Python)
  {
    return ".";
  }
  else if (lang==SrcLangExt_PHP && !classScope)
  {
    return "\\";
  }
  else
  {
    return "::";
  }
}
/** Checks whether the given url starts with a supported protocol */
bool isURL(const QCString &url)
{
  QCString loc_url = url.stripWhiteSpace();
  return loc_url.left(5)=="http:" || loc_url.left(6)=="https:" ||
         loc_url.left(4)=="ftp:"  || loc_url.left(5)=="ftps:"  ||
         loc_url.left(5)=="sftp:" || loc_url.left(5)=="file:"  ||
         loc_url.left(5)=="news:" || loc_url.left(4)=="irc:"   ||
         loc_url.left(5)=="ircs:";
}
/** Corrects URL \a url according to the relative path \a relPath.
 *  Returns the corrected URL. For absolute URLs no correction will be done.
 */
QCString correctURL(const QCString &url,const QCString &relPath)
{
  QCString result = url;
  if (!relPath.isEmpty() && !isURL(url))
  {
    result.prepend(relPath);
  }
  return result;
}

//---------------------------------------------------------------------------

bool protectionLevelVisible(Protection prot)
{
  static bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
  static bool extractPackage = Config_getBool(EXTRACT_PACKAGE);

  return (prot!=Private && prot!=Package)  ||
         (prot==Private && extractPrivate) ||
         (prot==Package && extractPackage);
}

//---------------------------------------------------------------------------

QCString stripIndentation(const QCString &s)
{
  if (s.isEmpty()) return s; // empty string -> we're done

  //printf("stripIndentation:\n%s\n------\n",s.data());
  // compute minimum indentation over all lines
  const char *p=s.data();
  char c;
  int indent=0;
  int minIndent=1000000; // "infinite"
  bool searchIndent=TRUE;
  static int tabSize=Config_getInt(TAB_SIZE);
  while ((c=*p++))
  {
    if      (c=='\t') indent+=tabSize - (indent%tabSize);
    else if (c=='\n') indent=0,searchIndent=TRUE;
    else if (c==' ')  indent++;
    else if (searchIndent)
    {
      searchIndent=FALSE;
      if (indent<minIndent) minIndent=indent;
    }
  }

  // no indent to remove -> we're done
  if (minIndent==0) return s;

  // remove minimum indentation for each line
  QGString result;
  p=s.data();
  indent=0;
  while ((c=*p++))
  {
    if (c=='\n') // start of new line
    {
      indent=0;
      result+=c;
    }
    else if (indent<minIndent) // skip until we reach minIndent
    {
      if (c=='\t')
      {
        int newIndent = indent+tabSize-(indent%tabSize);
        int i=newIndent;
        while (i>minIndent) // if a tab crosses the minIndent boundary fill the rest with spaces
        {
          result+=' ';
          i--;
        }
        indent=newIndent;
      }
      else // space
      {
        indent++;
      }
    }
    else // copy anything until the end of the line
    {
      result+=c;
    }
  }

  result+='\0';
  return result.data();
}

// strip up to \a indentationLevel spaces from each line in \a doc (excluding the first line)
void stripIndentation(QCString &doc,const int indentationLevel)
{
  if (indentationLevel <= 0 || doc.isEmpty()) return; // nothing to strip

  // by stripping content the string will only become shorter so we write the results
  // back into the input string and then resize it at the end.
  char c;
  const char *src = doc.data();
  char *dst = doc.rawData();
  bool insideIndent = false; // skip the initial line from stripping
  int cnt = 0;
  while ((c=*src++)!=0)
  {
    // invariant: dst<=src
    switch(c)
    {
      case '\n':
        *dst++ = c;
        insideIndent = true;
        cnt = indentationLevel;
        break;
      case ' ':
        if (insideIndent)
        {
          if (cnt>0) // count down the spacing until the end of the indent
          {
            cnt--;
          }
          else // reached the end of the indent, start of the part of the line to keep
          {
            insideIndent = false;
            *dst++ = c;
          }
        }
        else // part after indent, copy to the output
        {
          *dst++ = c;
        }
        break;
      default:
        insideIndent = false;
        *dst++ = c;
        break;
    }
  }
  doc.resize(dst-doc.data()+1);
}


bool fileVisibleInIndex(const FileDef *fd,bool &genSourceFile)
{
  static bool allExternals = Config_getBool(ALLEXTERNALS);
  bool isDocFile = fd->isDocumentationFile();
  genSourceFile = !isDocFile && fd->generateSourceFile();
  return ( ((allExternals && fd->isLinkable()) ||
            fd->isLinkableInProject()
           ) &&
           !isDocFile
         );
}

//--------------------------------------------------------------------------------------

/*! @brief Get one unicode character as an unsigned integer from utf-8 string
 *
 * @param s utf-8 encoded string
 * @param idx byte position of given string \a s.
 * @return the unicode codepoint, 0 - MAX_UNICODE_CODEPOINT
 * @see getNextUtf8OrToLower()
 * @see getNextUtf8OrToUpper()
 */
uint getUtf8Code( const QCString& s, int idx )
{
  const int length = s.length();
  if (idx >= length) { return 0; }
  const uint c0 = (uchar)s.at(idx);
  if ( c0 < 0xC2 || c0 >= 0xF8 ) // 1 byte character
  {
    return c0;
  }
  if (idx+1 >= length) { return 0; }
  const uint c1 = ((uchar)s.at(idx+1)) & 0x3f;
  if ( c0 < 0xE0 ) // 2 byte character
  {
    return ((c0 & 0x1f) << 6) | c1;
  }
  if (idx+2 >= length) { return 0; }
  const uint c2 = ((uchar)s.at(idx+2)) & 0x3f;
  if ( c0 < 0xF0 ) // 3 byte character
  {
    return ((c0 & 0x0f) << 12) | (c1 << 6) | c2;
  }
  if (idx+3 >= length) { return 0; }
  // 4 byte character
  const uint c3 = ((uchar)s.at(idx+3)) & 0x3f;
  return ((c0 & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
}


/*! @brief Returns one unicode character as an unsigned integer
 *  from utf-8 string, making the character lower case if it was upper case.
 *
 * @param s utf-8 encoded string
 * @param idx byte position of given string \a s.
 * @return the unicode codepoint, 0 - MAX_UNICODE_CODEPOINT, excludes 'A'-'Z'
 * @see getNextUtf8Code()
*/
uint getUtf8CodeToLower( const QCString& s, int idx )
{
  const uint v = getUtf8Code( s, idx );
  return v < 0x7f ? tolower( v ) : v;
}


/*! @brief Returns one unicode character as an unsigned integer
 *  from utf-8 string, making the character upper case if it was lower case.
 *
 * @param s utf-8 encoded string
 * @param idx byte position of given string \a s.
 * @return the unicode codepoint, 0 - MAX_UNICODE_CODEPOINT, excludes 'A'-'Z'
 * @see getNextUtf8Code()
 */
uint getUtf8CodeToUpper( const QCString& s, int idx )
{
  const uint v = getUtf8Code( s, idx );
  return v < 0x7f ? toupper( v ) : v;
}

//--------------------------------------------------------------------------------------
//
bool namespaceHasNestedNamespace(const NamespaceDef *nd)
{
  for (const auto &cnd : nd->getNamespaces())
  {
    if (cnd->isLinkableInProject() && !cnd->isAnonymous())
    {
      return true;
    }
  }
  return false;
}

bool namespaceHasNestedClass(const NamespaceDef *nd,bool filterClasses,ClassDef::CompoundType ct)
{
  //printf(">namespaceHasVisibleChild(%s,includeClasses=%d)\n",nd->name().data(),includeClasses);
  for (const auto &cnd : nd->getNamespaces())
  {
    if (namespaceHasNestedClass(cnd,filterClasses,ct))
    {
      //printf("<namespaceHasVisibleChild(%s,includeClasses=%d): case2\n",nd->name().data(),includeClasses);
      return TRUE;
    }
  }

  ClassLinkedRefMap list = nd->getClasses();
  if (filterClasses)
  {
    if (ct == ClassDef::Interface)
    {
      list = nd->getInterfaces();
    }
    else if (ct == ClassDef::Struct)
    {
      list = nd->getStructs();
    }
    else if (ct == ClassDef::Exception)
    {
      list = nd->getExceptions();
    }
  }

  for (const auto &cd : list)
  {
    if (cd->isLinkableInProject() && cd->templateMaster()==0)
    {
      //printf("<namespaceHasVisibleChild(%s,includeClasses=%d): case3\n",nd->name().data(),includeClasses);
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------

bool classVisibleInIndex(const ClassDef *cd)
{
  static bool allExternals = Config_getBool(ALLEXTERNALS);
  return (allExternals && cd->isLinkable()) || cd->isLinkableInProject();
}

//----------------------------------------------------------------------------

/** Strip the direction part from docs and return it as a string in canonical form
 *  The input \a docs string can start with e.g. "[in]", "[in, out]", "[inout]", "[out,in]"...
 *  @returns either "[in,out]", "[in]", or "[out]" or the empty string.
 */
QCString extractDirection(QCString &docs)
{
  std::regex re("\\[([ inout,]+)\\]");
  std::string s = docs.str();
  std::sregex_iterator it(s.begin(),s.end(),re);
  std::sregex_iterator end;
  if (it!=end)
  {
    const auto &match = *it;
    size_t p = match.position();
    size_t l = match.length();
    if (p==0 && l>2)
    {
      // make dir the part inside [...] without separators
      std::string dir = match[1].str();
      // strip , and ' ' from dir
      dir.erase(std::remove_if(dir.begin(),dir.end(),
                               [](const char c) { return c==' ' || c==','; }
                              ),dir.end());
      size_t inIndex, outIndex;
      unsigned char ioMask=0;
      if (( inIndex=dir.find( "in"))!=std::string::npos) dir.erase( inIndex,2),ioMask|=(1<<0);
      if ((outIndex=dir.find("out"))!=std::string::npos) dir.erase(outIndex,3),ioMask|=(1<<1);
      if (dir.empty() && ioMask!=0) // only in and/or out attributes found
      {
        docs = s.substr(l); // strip attributes
        if (ioMask==((1<<0)|(1<<1))) return "[in,out]";
        else if (ioMask==(1<<0))     return "[in]";
        else if (ioMask==(1<<1))     return "[out]";
      }
    }
  }
  return "";
}

//-----------------------------------------------------------

/** Computes for a given list type \a inListType, which are the
 *  the corresponding list type(s) in the base class that are to be
 *  added to this list.
 *
 *  So for public inheritance, the mapping is 1-1, so outListType1=inListType
 *  Private members are to be hidden completely.
 *
 *  For protected inheritance, both protected and public members of the
 *  base class should be joined in the protected member section.
 *
 *  For private inheritance, both protected and public members of the
 *  base class should be joined in the private member section.
 */
void convertProtectionLevel(
                   MemberListType inListType,
                   Protection inProt,
                   int *outListType1,
                   int *outListType2
                  )
{
  static bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
  // default representing 1-1 mapping
  *outListType1=inListType;
  *outListType2=-1;
  if (inProt==Public)
  {
    switch (inListType) // in the private section of the derived class,
                        // the private section of the base class should not
                        // be visible
    {
      case MemberListType_priMethods:
      case MemberListType_priStaticMethods:
      case MemberListType_priSlots:
      case MemberListType_priAttribs:
      case MemberListType_priStaticAttribs:
      case MemberListType_priTypes:
        *outListType1=-1;
        *outListType2=-1;
        break;
      default:
        break;
    }
  }
  else if (inProt==Protected) // Protected inheritance
  {
    switch (inListType) // in the protected section of the derived class,
                        // both the public and protected members are shown
                        // as protected
    {
      case MemberListType_pubMethods:
      case MemberListType_pubStaticMethods:
      case MemberListType_pubSlots:
      case MemberListType_pubAttribs:
      case MemberListType_pubStaticAttribs:
      case MemberListType_pubTypes:
      case MemberListType_priMethods:
      case MemberListType_priStaticMethods:
      case MemberListType_priSlots:
      case MemberListType_priAttribs:
      case MemberListType_priStaticAttribs:
      case MemberListType_priTypes:
        *outListType1=-1;
        *outListType2=-1;
        break;

      case MemberListType_proMethods:
        *outListType2=MemberListType_pubMethods;
        break;
      case MemberListType_proStaticMethods:
        *outListType2=MemberListType_pubStaticMethods;
        break;
      case MemberListType_proSlots:
        *outListType2=MemberListType_pubSlots;
        break;
      case MemberListType_proAttribs:
        *outListType2=MemberListType_pubAttribs;
        break;
      case MemberListType_proStaticAttribs:
        *outListType2=MemberListType_pubStaticAttribs;
        break;
      case MemberListType_proTypes:
        *outListType2=MemberListType_pubTypes;
        break;
      default:
        break;
    }
  }
  else if (inProt==Private)
  {
    switch (inListType) // in the private section of the derived class,
                        // both the public and protected members are shown
                        // as private
    {
      case MemberListType_pubMethods:
      case MemberListType_pubStaticMethods:
      case MemberListType_pubSlots:
      case MemberListType_pubAttribs:
      case MemberListType_pubStaticAttribs:
      case MemberListType_pubTypes:
      case MemberListType_proMethods:
      case MemberListType_proStaticMethods:
      case MemberListType_proSlots:
      case MemberListType_proAttribs:
      case MemberListType_proStaticAttribs:
      case MemberListType_proTypes:
        *outListType1=-1;
        *outListType2=-1;
        break;

      case MemberListType_priMethods:
        if (extractPrivate)
        {
          *outListType1=MemberListType_pubMethods;
          *outListType2=MemberListType_proMethods;
        }
        else
        {
          *outListType1=-1;
          *outListType2=-1;
        }
        break;
      case MemberListType_priStaticMethods:
        if (extractPrivate)
        {
          *outListType1=MemberListType_pubStaticMethods;
          *outListType2=MemberListType_proStaticMethods;
        }
        else
        {
          *outListType1=-1;
          *outListType2=-1;
        }
        break;
      case MemberListType_priSlots:
        if (extractPrivate)
        {
          *outListType1=MemberListType_pubSlots;
          *outListType2=MemberListType_proSlots;
        }
        else
        {
          *outListType1=-1;
          *outListType2=-1;
        }
        break;
      case MemberListType_priAttribs:
        if (extractPrivate)
        {
          *outListType1=MemberListType_pubAttribs;
          *outListType2=MemberListType_proAttribs;
        }
        else
        {
          *outListType1=-1;
          *outListType2=-1;
        }
        break;
      case MemberListType_priStaticAttribs:
        if (extractPrivate)
        {
          *outListType1=MemberListType_pubStaticAttribs;
          *outListType2=MemberListType_proStaticAttribs;
        }
        else
        {
          *outListType1=-1;
          *outListType2=-1;
        }
        break;
      case MemberListType_priTypes:
        if (extractPrivate)
        {
          *outListType1=MemberListType_pubTypes;
          *outListType2=MemberListType_proTypes;
        }
        else
        {
          *outListType1=-1;
          *outListType2=-1;
        }
        break;
      default:
        break;
    }
  }
  //printf("convertProtectionLevel(type=%d prot=%d): %d,%d\n",
  //    inListType,inProt,*outListType1,*outListType2);
}

bool mainPageHasTitle()
{
  return Doxygen::mainPage!=0 && Doxygen::mainPage->hasTitle();
}

QCString getDotImageExtension()
{
  QCString imgExt = Config_getEnum(DOT_IMAGE_FORMAT);
  int i= imgExt.find(':'); // strip renderer part when using e.g. 'png:cairo:gd' as format
  return i==-1 ? imgExt : imgExt.left(i);
}

bool openOutputFile(const char *outFile,QFile &f)
{
  bool fileOpened=FALSE;
  bool writeToStdout=(outFile[0]=='-' && outFile[1]=='\0');
  if (writeToStdout) // write to stdout
  {
    fileOpened = f.open(IO_WriteOnly,stdout);
  }
  else // write to file
  {
    QFileInfo fi(outFile);
    if (fi.exists()) // create a backup
    {
      QDir dir=fi.dir();
      QFileInfo backup(fi.fileName()+".bak");
      if (backup.exists()) // remove existing backup
        dir.remove(backup.fileName());
      dir.rename(fi.fileName(),fi.fileName()+".bak");
    }
    f.setName(outFile);
    fileOpened = f.open(IO_WriteOnly|IO_Translate);
  }
  return fileOpened;
}

void writeExtraLatexPackages(FTextStream &t)
{
  // User-specified packages
  const StringVector &extraPackages = Config_getList(EXTRA_PACKAGES);
  if (!extraPackages.empty())
  {
    t << "% Packages requested by user\n";
    for (const auto &pkgName : extraPackages)
    {
      if ((pkgName[0] == '[') || (pkgName[0] == '{'))
        t << "\\usepackage" << pkgName.c_str() << "\n";
      else
        t << "\\usepackage{" << pkgName.c_str() << "}\n";
    }
    t << "\n";
  }
}

void writeLatexSpecialFormulaChars(FTextStream &t)
{
    unsigned char minus[4]; // Superscript minus
    char *pminus = (char *)minus;
    unsigned char sup2[3]; // Superscript two
    char *psup2 = (char *)sup2;
    unsigned char sup3[3];
    char *psup3 = (char *)sup3; // Superscript three
    minus[0]= 0xE2;
    minus[1]= 0x81;
    minus[2]= 0xBB;
    minus[3]= 0;
    sup2[0]= 0xC2;
    sup2[1]= 0xB2;
    sup2[2]= 0;
    sup3[0]= 0xC2;
    sup3[1]= 0xB3;
    sup3[2]= 0;

    t << "\\usepackage{newunicodechar}\n"
         "  \\newunicodechar{" << pminus << "}{${}^{-}$}% Superscript minus\n"
         "  \\newunicodechar{" << psup2  << "}{${}^{2}$}% Superscript two\n"
         "  \\newunicodechar{" << psup3  << "}{${}^{3}$}% Superscript three\n"
         "\n";
}

//------------------------------------------------------
// simplified way to know if this is fixed form
bool recognizeFixedForm(const char* contents, FortranFormat format)
{
  int column=0;
  bool skipLine=FALSE;

  if (format == FortranFormat_Fixed) return TRUE;
  if (format == FortranFormat_Free)  return FALSE;

  for(int i=0;;i++) {
    column++;

    switch(contents[i]) {
      case '\n':
        column=0;
        skipLine=FALSE;
        break;
      case ' ':
        break;
      case '\000':
        return FALSE;
      case '#':
        skipLine=TRUE;
        break;
      case 'C':
      case 'c':
      case '*':
        if (column==1) return TRUE;
        if (skipLine) break;
        return FALSE;
      case '!':
        if (column>1 && column<7) return FALSE;
        skipLine=TRUE;
        break;
      default:
        if (skipLine) break;
        if (column>=7) return TRUE;
        return FALSE;
    }
  }
  return FALSE;
}

FortranFormat convertFileNameFortranParserCode(QCString fn)
{
  QCString ext = getFileNameExtension(fn);
  QCString parserName = Doxygen::parserManager->getParserName(ext.data());

  if (parserName == "fortranfixed") return FortranFormat_Fixed;
  else if (parserName == "fortranfree") return FortranFormat_Free;

  return FortranFormat_Unknown;
}
//------------------------------------------------------------------------

/// Clear a text block \a s from \a begin to \a end markers
QCString clearBlock(const char *s,const char *begin,const char *end)
{
  if (s==0 || begin==0 || end==0) return s;
  const char *p, *q;
  int beginLen = qstrlen(begin);
  int endLen = qstrlen(end);
  int resLen = 0;
  for (p=s; (q=strstr(p,begin))!=0; p=q+endLen)
  {
    resLen+=(int)(q-p);
    p=q+beginLen;
    if ((q=strstr(p,end))==0)
    {
      resLen+=beginLen;
      break;
    }
  }
  resLen+=qstrlen(p);
  // resLen is the length of the string without the marked block

  QCString result(resLen+1);
  char *r;
  for (r=result.rawData(), p=s; (q=strstr(p,begin))!=0; p=q+endLen)
  {
    int l = (int)(q-p);
    memcpy(r,p,l);
    r+=l;
    p=q+beginLen;
    if ((q=strstr(p,end))==0)
    {
      memcpy(r,begin,beginLen);
      r+=beginLen;
      break;
    }
  }
  qstrcpy(r,p);
  return result;
}
//----------------------------------------------------------------------

QCString selectBlock(const QCString& s,const QCString &name,bool enable, OutputGenerator::OutputType o)
{
  // TODO: this is an expensive function that is called a lot -> optimize it
  QCString begin;
  QCString end;
  QCString nobegin;
  QCString noend;
  switch (o)
  {
    case OutputGenerator::Html:
      begin = "<!--BEGIN " + name + "-->";
      end = "<!--END " + name + "-->";
      nobegin = "<!--BEGIN !" + name + "-->";
      noend = "<!--END !" + name + "-->";
      break;
    case OutputGenerator::Latex:
      begin = "%%BEGIN " + name;
      end = "%%END " + name;
      nobegin = "%%BEGIN !" + name;
      noend = "%%END !" + name;
      break;
    default:
      break;
  }

  QCString result = s;
  if (enable)
  {
    result = substitute(result, begin, "");
    result = substitute(result, end, "");
    result = clearBlock(result, nobegin, noend);
  }
  else
  {
    result = substitute(result, nobegin, "");
    result = substitute(result, noend, "");
    result = clearBlock(result, begin, end);
  }

  return result;
}

QCString removeEmptyLines(const QCString &s)
{
  BufStr out(s.length()+1);
  const char *p=s.data();
  if (p)
  {
    char c;
    while ((c=*p++))
    {
      if (c=='\n')
      {
        const char *e = p;
        while (*e==' ' || *e=='\t') e++;
        if (*e=='\n')
        {
          p=e;
        }
        else out.addChar(c);
      }
      else
      {
        out.addChar(c);
      }
    }
  }
  out.addChar('\0');
  //printf("removeEmptyLines(%s)=%s\n",s.data(),out.data());
  return out.data();
}

/// split input string \a s by string delimiter \a delimiter.
/// returns a vector of non-empty strings that are between the delimiters
StringVector split(const std::string &s,const std::string &delimiter)
{
  StringVector result;
  size_t prev = 0, pos = 0, len = s.length();
  do
  {
    pos = s.find(delimiter, prev);
    if (pos == std::string::npos) pos = len;
    if (pos>prev) result.push_back(s.substr(prev,pos-prev));
    prev = pos + delimiter.length();
  }
  while (pos<len && prev<len);
  return result;
}

/// split input string \a s by regular expression delimiter \a delimiter.
/// returns a vector of non-empty strings that are between the delimiters
StringVector split(const std::string &s,const std::regex &delimiter)
{
  StringVector result;
  std::sregex_token_iterator iter(s.begin(), s.end(), delimiter, -1);
  std::sregex_token_iterator end;
  for ( ; iter != end; ++iter)
  {
    result.push_back(*iter);
  }
  return result;
}

/// find the index of a string in a vector of strings, returns -1 if the string could not be found
int findIndex(const StringVector &sv,const std::string &s)
{
  auto it = std::find(sv.begin(),sv.end(),s);
  return it!=sv.end() ? (int)(it-sv.begin()) : -1;
}

/// find the index of the first occurrence of pattern \a re in a string \a s
/// returns -1 if the pattern could not be found
int findIndex(const std::string &s,const std::regex &re)
{
  std::smatch match;
  return (std::regex_search(s,match,re)) ? (int)match.position() : -1;
}

