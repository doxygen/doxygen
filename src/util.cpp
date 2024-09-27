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
#include <assert.h>

#include <mutex>
#include <unordered_set>
#include <codecvt>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <cinttypes>
#include <sstream>

#include "md5.h"

#include "regex.h"
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
#include "fileinfo.h"
#include "dir.h"
#include "utf8.h"
#include "textstream.h"
#include "indexlist.h"
#include "datetime.h"
#include "moduledef.h"
#include "trace.h"
#include "stringutil.h"

#define ENABLE_TRACINGSUPPORT 0

#if defined(__APPLE__) && ENABLE_TRACINGSUPPORT
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

TextGeneratorOLImpl::TextGeneratorOLImpl(OutputList &ol) : m_ol(ol)
{
}

void TextGeneratorOLImpl::writeString(std::string_view s,bool keepSpaces) const
{
  if (s.empty()) return;
  //printf("TextGeneratorOlImpl::writeString('%s',%d)\n",s,keepSpaces);
  if (keepSpaces)
  {
    char cs[2];
    cs[1]='\0';
    for (size_t i=0;i<s.length();i++)
    {
      char c = s[i];
      if (c==' ')
      {
        m_ol.writeNonBreakableSpace(1);
      }
      else
      {
        cs[0]=c;
        m_ol.docify(cs);
      }
    }
  }
  else
  {
    m_ol.docify(QCString(s));
  }
}

void TextGeneratorOLImpl::writeBreak(int indent) const
{
  m_ol.lineBreak("typebreak");
  for (int i=0;i<indent;i++)
  {
    m_ol.writeNonBreakableSpace(3);
  }
}

void TextGeneratorOLImpl::writeLink(const QCString &extRef,const QCString &file,
                                    const QCString &anchor,std::string_view text
                                   ) const
{
  //printf("TextGeneratorOlImpl::writeLink('%s')\n",text);
  m_ol.writeObjectLink(extRef,file,anchor,QCString(text));
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
QCString removeAnonymousScopes(const QCString &str)
{
  std::string result;
  if (str.isEmpty()) return QCString(result);

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
    for (int i=static_cast<int>(del.size())-1;i>=0;i--)
    {
      if (del[i]=='@') return false;
      else if (del[i]==':') return true;
    }
    return false;
  };

  static const reg::Ex re(R"([\s:]*@\d+[\s:]*)");
  std::string s = str.str();
  reg::Iterator iter(s,re);
  reg::Iterator end;
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
QCString replaceAnonymousScopes(const QCString &s,const QCString &replacement)
{
  if (s.isEmpty()) return s;
  static const reg::Ex marker(R"(@\d+)");
  std::string result = reg::replace(s.str(),marker,
                                    replacement.isEmpty() ? replacement.data() : "__anonymous__");
  //printf("replaceAnonymousScopes('%s')='%s'\n",qPrint(s),qPrint(result));
  return QCString(result);
}


// strip anonymous left hand side part of the scope
QCString stripAnonymousNamespaceScope(const QCString &s)
{
  int i=0,p=0,l=0;
  QCString newScope;
  int sl = static_cast<int>(s.length());
  while ((i=getScopeFragment(s,p,&l))!=-1)
  {
    //printf("Scope fragment %s\n",qPrint(s.mid(i,l)));
    if (Doxygen::namespaceLinkedMap->find(s.left(i+l))!=nullptr)
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
  //printf("stripAnonymousNamespaceScope('%s')='%s'\n",qPrint(s),qPrint(newScope));
  return newScope;
}

void writePageRef(OutputList &ol,const QCString &cn,const QCString &mn)
{
  ol.pushGeneratorState();

  ol.disable(OutputType::Html);
  ol.disable(OutputType::Man);
  ol.disable(OutputType::Docbook);
  if (Config_getBool(PDF_HYPERLINKS)) ol.disable(OutputType::Latex);
  if (Config_getBool(RTF_HYPERLINKS)) ol.disable(OutputType::RTF);
  ol.startPageRef();
  ol.docify(theTranslator->trPageAbbreviation());
  ol.endPageRef(cn,mn);

  ol.popGeneratorState();
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

QCString removeLongPathMarker(QCString path)
{
#if defined(_WIN32)
  if (path.startsWith("//?/")) // strip leading "\\?\" part from path
  {
    path=path.mid(4);
  }
#endif
  return path;
}

static QCString stripFromPath(const QCString &p,const StringVector &l)
{
  // look at all the strings in the list and strip the longest match
  QCString potential;
  QCString path=removeLongPathMarker(p);
  size_t length = 0;
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
  if (length>0) return potential;
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
EntryType guessSection(const QCString &name)
{
  QCString n=name.lower();
  static const std::unordered_set<std::string> sourceExt = {
     "c","cc","cxx","cpp","c++","cppm","ccm","cxxm","c++m",   // C/C++
     "java",                       // Java
     "cs",                         // C#
     "m","mm",                     // Objective-C
     "ii","ixx","ipp","i++","inl", // C/C++ inline
     "xml","lex","sql"             // others
  };
  static const std::unordered_set<std::string> headerExt = {
     "h", "hh", "hxx", "hpp", "h++", "ixx", // C/C++ header
     "idl", "ddl", "pidl", "ice"    // IDL like
  };
  int lastDot = n.findRev('.');
  if (lastDot!=-1)
  {
    QCString extension = n.mid(lastDot+1); // part after the last dot
    if (sourceExt.find(extension.str())!=sourceExt.end())
    {
      return EntryType::makeSource();
    }
    if (headerExt.find(extension.str())!=headerExt.end())
    {
      return EntryType::makeHeader();
    }
  }
  else
  {
    if (getLanguageFromFileName(name,SrcLangExt::Unknown) == SrcLangExt::Cpp) return EntryType::makeHeader();
  }
  return EntryType::makeEmpty();
}

QCString resolveTypeDef(const Definition *context,const QCString &qualifiedName,
                        const Definition **typedefContext)
{
  AUTO_TRACE("context='{}' qualifiedName='{}'",context?context->name():"",qualifiedName);
  QCString result;
  if (qualifiedName.isEmpty())
  {
    AUTO_TRACE_EXIT("empty name");
    return result;
  }

  const Definition *mContext=context;
  if (typedefContext) *typedefContext=context;

  // see if the qualified name has a scope part
  if (qualifiedName.find('<')!=-1)
  {
    AUTO_TRACE_EXIT("template");
    return result;
  }
  int scopeIndex = qualifiedName.findRev("::");
  QCString resName=qualifiedName;
  if (scopeIndex!=-1) // strip scope part for the name
  {
    resName=qualifiedName.right(qualifiedName.length()-scopeIndex-2);
    if (resName.isEmpty())
    {
      AUTO_TRACE_EXIT("invalid format");
      return result;
    }
  }
  const MemberDef *md=nullptr;
  while (mContext && md==nullptr)
  {
    // step 1: get the right scope
    const Definition *resScope=mContext;
    if (scopeIndex!=-1)
    {
      // split-off scope part
      QCString resScopeName = qualifiedName.left(scopeIndex);
      //printf("resScopeName='%s'\n",qPrint(resScopeName));

      // look-up scope in context
      int is=0,ps=0,l=0;
      while ((is=getScopeFragment(resScopeName,ps,&l))!=-1)
      {
        QCString qualScopePart = resScopeName.mid(is,l);
        QCString tmp = resolveTypeDef(mContext,qualScopePart);
        if (!tmp.isEmpty()) qualScopePart=tmp;
        resScope = resScope->findInnerCompound(qualScopePart);
        //printf("qualScopePart='%s' resScope=%p\n",qPrint(qualScopePart),resScope);
        if (resScope==nullptr) break;
        ps=is+l;
      }
    }
    AUTO_TRACE_ADD("resScope='{}' resName='{}'",resScope?resScope->name():"",resName);

    // step 2: get the member
    if (resScope) // no scope or scope found in the current context
    {
      //printf("scope found: %s, look for typedef %s\n",
      //     qPrint(resScope->qualifiedName()),qPrint(resName));
      MemberNameLinkedMap *mnd=nullptr;
      bool searchRelated=false;
      bool mustBeRelated=false;
      if (resScope->definitionType()==Definition::TypeClass)
      {
        mnd=Doxygen::memberNameLinkedMap;
      }
      else
      {
        mnd=Doxygen::functionNameLinkedMap;
        searchRelated=true;
      }
      MemberName *mn=mnd->find(resName);
      if (mn==0 && searchRelated)
      {
        mn=Doxygen::memberNameLinkedMap->find(resName);
        mustBeRelated=true;
      }
      if (mn)
      {
        int minDist=-1;
        for (const auto &tmd_p : *mn)
        {
          const MemberDef *tmd = tmd_p.get();
          AUTO_TRACE_ADD("found candidate member '{}' isTypeDef={}' isRelated={} mustBeRelated={}",
              tmd->name(),tmd->isTypedef(),tmd->isRelated(),mustBeRelated);
          //printf("Found member %s resScope=%s outerScope=%s mContext=%p\n",
          //    qPrint(tmd->name()),qPrint( resScope->name()),
          //    qPrint(tmd->getOuterScope()->name()), mContext);
          if (tmd->isTypedef())
          {
            if (resScope==Doxygen::globalScope && tmd->isRelated() && mustBeRelated)
            {
              md = tmd;
            }
            else
            {
              SymbolResolver resolver;
              int dist=resolver.isAccessibleFrom(resScope,tmd);
              if (dist!=-1 && (md==nullptr || dist<minDist))
              {
                md = tmd;
                minDist = dist;
              }
            }
          }
        }
      }
    }
    mContext=mContext->getOuterScope();
  }

  AUTO_TRACE_ADD("md='{}'",md?md->name():"");
  // step 3: get the member's type
  if (md)
  {
    //printf(">>resolveTypeDef: Found typedef name '%s' in scope '%s' value='%s' args='%s'\n",
    //    qPrint(qualifiedName),qPrint(context->name()),qPrint(md->typeString()),qPrint(md->argsString())
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
    //    qPrint(qualifiedName),context ? qPrint(context->name()) : "<global>");
  }
  AUTO_TRACE_EXIT("result='{}'",result);
  return result;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

static const char constScope[]    = { 'c', 'o', 'n', 's', 't', ':' };
static const char volatileScope[] = { 'v', 'o', 'l', 'a', 't', 'i', 'l', 'e', ':' };
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
  static THREAD_LOCAL char *growBuf = nullptr;
  static THREAD_LOCAL size_t growBufLen = 0;
  if (s.length()*3>growBufLen) // For input character we produce at most 3 output characters,
  {
    growBufLen = s.length()*3;
    growBuf = static_cast<char *>(realloc(growBuf,growBufLen+1)); // add 1 for 0-terminator
  }
  if (growBuf==nullptr) return s; // should not happen, only we run out of memory

  const char *src=s.data();
  char *dst=growBuf;

  size_t i=0;
  size_t l=s.length();
  size_t csp=0;
  size_t vosp=0;
  size_t vsp=0;
  size_t osp=0;
  char pc=0;
  // skip leading whitespace
  while (i<l && isspace(static_cast<uint8_t>(src[i])))
  {
    i++;
  }
  for (;i<l;i++)
  {
    char c=src[i];
    char nc=i+1<l ? src[i+1] : ' ';

    auto searchForKeyword = [&](const char *kw,size_t &matchLen,size_t totalLen)
    {
      if (matchLen<=totalLen && c==kw[matchLen] && // character matches substring kw
          (matchLen>0 ||                           // inside search string
           i==0  ||                                // if it is the first character
           !isId(pc)                               // the previous may not be a digit
          )
         )
        matchLen++;
      else // reset counter
        matchLen=0;
    };
    searchForKeyword(constScope,    csp,  5); // keyword: const
    searchForKeyword(volatileScope, vosp, 8); // keyword: volatile
    searchForKeyword(virtualScope,  vsp,  7); // keyword: virtual

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
          i++;
          for (;i<l;i++) // find end of string
          {
            c = src[i];
            *dst++=c;
            if (c=='\\' && i+1<l)
            {
              i++;
              c = src[i];
              *dst++=c;
            }
            else if (c=='"')
            {
              break;
            }
          }
        }
        break;
      case '<': // current char is a <
        *dst++=c;
        if (i+1<l &&
            (isId(nc)) && // next char is an id char
            (osp<8) // string in front is not "operator"
           )
        {
          *dst++=' '; // add extra space
        }
        break;
      case '>': // current char is a >
        if (i>0 && !isspace(static_cast<uint8_t>(pc)) &&
            (isId(pc) || pc=='*' || pc=='&' || pc=='.' || pc=='>') && // prev char is an id char or space or *&.
            (osp<8 || (osp==8 && pc!='-')) // string in front is not "operator>" or "operator->"
           )
        {
          *dst++=' '; // add extra space in front
        }
        *dst++=c;
        if (i+1<l && (nc=='-' || nc=='&')) // '>-' -> '> -'
        {
          *dst++=' '; // add extra space after
        }
        break;
      case ',': // current char is a ,
        *dst++=c;
        if (i>0 && !isspace(static_cast<uint8_t>(pc)) &&
            ((i+1<l && (isId(nc) || nc=='[')) || // the [ is for attributes (see bug702170)
             (i+2<l && nc=='$' && isId(src[i+2])) ||   // for PHP: ',$name' -> ', $name'
             (i+3<l && nc=='&' && src[i+2]=='$' && isId(src[i+3])) // for PHP: ',&$name' -> ', &$name'
            )
           )
        {
          *dst++=' '; // add extra space after
        }
        break;
      case '^':  // CLI 'Type^name' -> 'Type^ name'
      case '%':  // CLI 'Type%name' -> 'Type% name'
        *dst++=c;
        if (cliSupport && i+1<l && (isId(nc) || nc=='-'))
        {
          *dst++=' '; // add extra space after
        }
        break;
      case ')':  // current char is a )  -> ')name' -> ') name'
        *dst++=c;
        if (i+1<l && (isId(nc) || nc=='-'))
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
      case '$':  // '$name' -> ' $name'
                 // 'name$name' -> 'name$name'
        if (isId(pc))
        {
          *dst++=c;
          break;
        }
        // else fallthrough
      case '@':  // '@name' -> ' @name'
      case '\'': // ''name' -> '' name'
        if (i>0 && i+1<l && pc!='=' && pc!=':' && !isspace(static_cast<uint8_t>(pc)) &&
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
        else if (vosp==9) // replace volatile::A by volatile ::A
        {
          *dst++=' ';
          vosp=0;
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
          if (g_charAroundSpace.charMap[static_cast<uint8_t>(pc)].before &&
              g_charAroundSpace.charMap[static_cast<uint8_t>(nc)].after  &&
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
        auto correctKeywordAllowedInsideScope = [&](char cc,size_t &matchLen,size_t totalLen) {
          if (c==cc && matchLen==totalLen)
          {
            if ((i+2<l && src[i+1] == ':' && src[i+2] == ':') ||                     // keyword::
                ((i>matchLen && src[i-matchLen] == ':' && src[i-matchLen-1] == ':')) // ::keyword
               ) matchLen = 0;
          };
        };
        correctKeywordAllowedInsideScope('t',csp, 5); // keyword: const
        correctKeywordAllowedInsideScope('e',vosp,8); // keyword: volatile
        correctKeywordAllowedInsideScope('l',vsp, 7); // keyword: virtual

        auto correctKeywordNotPartOfScope = [&](char cc,size_t &matchLen,size_t totalLen)
        {
          if (c==cc && matchLen==totalLen && i+1<l && // found matching keyword
              !(isId(nc) || nc==')' || nc==',' || qisspace(nc))
             ) // prevent keyword ::A from being converted to keyword::A
          {
            *dst++=' ';
            matchLen=0;
          }
        };
        correctKeywordNotPartOfScope('t',csp, 5); // keyword: const
        correctKeywordNotPartOfScope('e',vosp,8); // keyword: volatile
        correctKeywordNotPartOfScope('l',vsp, 7); // keyword: virtual
        break;
    }
    pc=c;
  }
  *dst++='\0';
  //printf("removeRedundantWhitespace(%s)->%s\n",qPrint(s),growBuf);
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
  size_t sl=scope.length();
  size_t nl=name.length();
  return (name==scope || // equal
          (scope.right(nl)==name && // substring
           sl>1+nl && scope.at(sl-nl-1)==':' && scope.at(sl-nl-2)==':' // scope
          )
         );
}

bool leftScopeMatch(const QCString &scope, const QCString &name)
{
  size_t sl=scope.length();
  size_t nl=name.length();
  return (name==scope || // equal
          (name.left(sl)==scope && // substring
           nl>sl+1 && name.at(sl)==':' && name.at(sl+1)==':' // scope
          )
         );
}


void linkifyText(const TextGeneratorIntf &out, const Definition *scope,
    const FileDef *fileScope,const Definition *self,
    const QCString &text, bool autoBreak,bool external,
    bool keepSpaces,int indentLevel)
{
  if (text.isEmpty()) return;
  //printf("linkify='%s'\n",qPrint(text));
  std::string_view txtStr=text.view();
  size_t strLen = txtStr.length();
  if (strLen==0) return;

  static const reg::Ex regExp(R"((::)?\a[\w~!\\.:$"]*)");
  reg::Iterator it(txtStr,regExp);
  reg::Iterator end;

  //printf("linkifyText scope=%s fileScope=%s strtxt=%s strlen=%zu external=%d\n",
  //    scope ? qPrint(scope->name()):"<none>",
  //    fileScope ? qPrint(fileScope->name()) : "<none>",
  //    qPrint(txtStr),strLen,external);
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
      std::string_view part = txtStr.substr(skipIndex,newIndex+matchLen-skipIndex);
      out.writeString(part,keepSpaces);
      skipIndex=index=newIndex+matchLen;
      continue;
    }

    // add non-word part to the result
    bool insideString=FALSE;
    for (size_t i=index;i<newIndex;i++)
    {
      if (txtStr.at(i)=='"') insideString=!insideString;
      if (txtStr.at(i)=='\\') i++; // skip next character it is escaped
    }

    //printf("floatingIndex=%d strlen=%d autoBreak=%d\n",floatingIndex,strLen,autoBreak);
    if (strLen>35 && floatingIndex>30 && autoBreak) // try to insert a split point
    {
      std::string_view splitText = txtStr.substr(skipIndex,newIndex-skipIndex);
      size_t splitLength = splitText.length();
      size_t offset=1;
      size_t i = splitText.find(',');
      if (i==std::string::npos) { i=splitText.find('<'); if (i!=std::string::npos) offset=0; }
      if (i==std::string::npos) i=splitText.find('>');
      if (i==std::string::npos) i=splitText.find(' ');
      //printf("splitText=[%s] len=%d i=%d offset=%d\n",qPrint(splitText),splitLength,i,offset);
      if (i!=std::string::npos) // add a link-break at i in case of Html output
      {
        std::string_view part1 = splitText.substr(0,i+offset);
        out.writeString(part1,keepSpaces);
        out.writeBreak(indentLevel==0 ? 0 : indentLevel+1);
        std::string_view part2 = splitText.substr(i+offset);
        out.writeString(part2,keepSpaces);
        floatingIndex=splitLength-i-offset+matchLen;
      }
      else
      {
        out.writeString(splitText,keepSpaces);
      }
    }
    else
    {
      //ol.docify(txtStr.mid(skipIndex,newIndex-skipIndex));
      std::string_view part = txtStr.substr(skipIndex,newIndex-skipIndex);
      out.writeString(part,keepSpaces);
    }
    // get word from string
    std::string_view word=txtStr.substr(newIndex,matchLen);
    QCString matchWord = substitute(substitute(QCString(word),"\\","::"),".","::");
    //printf("linkifyText word=%s matchWord=%s scope=%s\n",
    //    qPrint(word),qPrint(matchWord),scope ? qPrint(scope->name()) : "<none>");
    bool found=FALSE;
    if (!insideString)
    {
      const ClassDef     *cd=nullptr;
      const ConceptDef   *cnd=nullptr;
      //printf("** Match word '%s'\n",qPrint(matchWord));

      SymbolResolver resolver(fileScope);
      cd=resolver.resolveClass(scope,matchWord);
      const MemberDef *typeDef = resolver.getTypedef();
      if (typeDef) // First look at typedef then class, see bug 584184.
      {
        if (external ? typeDef->isLinkable() : typeDef->isLinkableInProject())
        {
          if (typeDef->getOuterScope()!=self)
          {
            //printf("Found typedef %s word='%s'\n",qPrint(typeDef->name()),qPrint(word));
            out.writeLink(typeDef->getReference(),
                typeDef->getOutputFileBase(),
                typeDef->anchor(),
                word);
            found=TRUE;
          }
        }
      }
      auto writeCompoundName = [&](const auto *cd_) {
        if (external ? cd_->isLinkable() : cd_->isLinkableInProject())
        {
          if (self==nullptr || cd_->qualifiedName()!=self->qualifiedName())
          {
            //printf("Found compound %s word='%s'\n",qPrint(cd->name()),qPrint(word));
            out.writeLink(cd_->getReference(),cd_->getOutputFileBase(),cd_->anchor(),word);
            found=TRUE;
          }
        }
      };
      if (!found && (cd || (cd=getClass(matchWord))))
      {
        writeCompoundName(cd);
      }
      else if ((cd=getClass(matchWord+"-p"))) // search for Obj-C protocols as well
      {
        writeCompoundName(cd);
      }
      else if ((cnd=getConcept(matchWord))) // search for concepts
      {
        writeCompoundName(cnd);
      }
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

      //printf("ScopeName=%s\n",qPrint(scopeName));
      //if (!found) printf("Trying to link '%s' in '%s'\n",qPrint(word),qPrint(scopeName));
      if (!found)
      {
        GetDefInput input(scopeName,matchWord,QCString());
        GetDefResult result = getDefs(input);
        if (result.found &&
            (external ? result.md->isLinkable() : result.md->isLinkableInProject())
           )
        {
          //printf("Found ref scope=%s\n",d ? qPrint(d->name()) : "<global>");
          //ol.writeObjectLink(d->getReference(),d->getOutputFileBase(),
          //                       md->anchor(),word);
          if (result.md!=self && (self==nullptr || result.md->name()!=self->name()))
            // name check is needed for overloaded members, where getDefs just returns one
          {
            /* in case of Fortran scope and the variable is a non Fortran variable: don't link,
             * see also getLink in fortrancode.l
             */
            if (!(scope &&
                 (scope->getLanguage() == SrcLangExt::Fortran) &&
                 result.md->isVariable() &&
                 (result.md->getLanguage() != SrcLangExt::Fortran)
                )
               )
            {
              //printf("found symbol %s word='%s'\n",qPrint(result.md->name()),qPrint(word));
              out.writeLink(result.md->getReference(),result.md->getOutputFileBase(),
                  result.md->anchor(),word);
              found=TRUE;
            }
          }
        }
      }
    }

    if (!found) // add word to the result
    {
      out.writeString(word,keepSpaces);
    }
    // set next start point in the string
    //printf("index=%d/%d\n",index,txtStr.length());
    skipIndex=index=newIndex+matchLen;
  }
  // add last part of the string to the result.
  //ol.docify(txtStr.right(txtStr.length()-skipIndex));
  std::string_view lastPart = txtStr.substr(skipIndex);
  out.writeString(lastPart,keepSpaces);
}

void writeMarkerList(OutputList &ol,const std::string &markerText,size_t numMarkers,
                     std::function<void(size_t)> replaceFunc)
{
  static const reg::Ex marker(R"(@(\d+))");
  reg::Iterator it(markerText,marker);
  reg::Iterator end;
  size_t index=0;
  // now replace all markers in inheritLine with links to the classes
  for ( ; it!=end ; ++it)
  {
    const auto &match = *it;
    size_t newIndex = match.position();
    size_t matchLen = match.length();
    ol.parseText(markerText.substr(index,newIndex-index));
    unsigned long entryIndex = std::stoul(match[1].str());
    if (entryIndex<static_cast<unsigned long>(numMarkers))
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
    ol.disable(OutputType::Latex);
    ol.disable(OutputType::RTF);
    ol.disable(OutputType::Docbook);
    // link for Html / man
    //printf("writeObjectLink(file=%s)\n",qPrint(e->file));
    ol.writeObjectLink(QCString(),e.file,e.anchor,e.name);
    ol.popGeneratorState();

    ol.pushGeneratorState();
    ol.disable(OutputType::Man);
    ol.disable(OutputType::Html);
    // link for Latex / pdf with anchor because the sources
    // are not hyperlinked (not possible with a verbatim environment).
    ol.writeObjectLink(QCString(),e.file,QCString(),e.name);
    ol.popGeneratorState();
  };

  writeMarkerList(ol, theTranslator->trWriteList(static_cast<int>(list.size())).str(), list.size(), replaceFunc);

  ol.writeString(".");
}


QCString inlineArgListToDoc(const ArgumentList &al)
{
  QCString paramDocs;
  if (al.hasDocumentation())
  {
    for (const Argument &a : al)
    {
      if (a.hasDocumentation())
      {
        QCString docsWithoutDir = a.docs;
        QCString direction = extractDirection(docsWithoutDir);
        paramDocs+=" \\ilinebr @param"+direction+" "+a.name+" "+docsWithoutDir;
      }
    }
  }
  return paramDocs;
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
  if (al.refQualifier()==RefQualifierType::LValue) result+=" &";
  else if (al.refQualifier()==RefQualifierType::RValue) result+=" &&";
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
        if (lang==SrcLangExt::Java || lang==SrcLangExt::CSharp)
        {
          result+=a.type+" ";
        }
        result+=a.name;
      }
      else // extract name from type
      {
        int i = static_cast<int>(a.type.length())-1;
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
      if (!a.typeConstraint.isEmpty() && lang==SrcLangExt::Java)
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
static void filterCRLF(std::string &contents)
{
  size_t src  = 0;    // source index
  size_t dest = 0;   // destination index
  size_t len  = contents.length();

  while (src<len)
  {
    char c = contents[src++];  // Remember the processed character.
    if (c == '\r')             // CR to be solved (MAC, DOS)
    {
      c = '\n';                // each CR to LF
      if (src<len && contents[src] == '\n')
      {
        ++src;                 // skip LF just after CR (DOS)
      }
    }
    else if ( c == '\0' && src<len-1) // filter out internal \0 characters, as it will confuse the parser
    {
      c = ' ';                 // turn into a space
    }
    contents[dest++] = c;      // copy the (modified) character to dest
  }
  contents.resize(dest);
}

static QCString getFilterFromList(const QCString &name,const StringVector &filterList,bool &found)
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
      QCString input = name;
      if (!Portable::fileSystemIsCaseSensitive())
      {
        filterPattern = filterPattern.lower();
        input = input.lower();
      }
      reg::Ex re(filterPattern.str(),reg::Ex::Mode::Wildcard);
      if (re.isValid() && reg::match(input.str(),re))
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
QCString getFileFilter(const QCString &name,bool isSourceCode)
{
  // sanity check
  if (name.isEmpty()) return "";

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
    if (filterName.length()>=2 && filterName[0]=='"' && filterName[static_cast<int>(filterName.length())-1]=='"')
    {
       filterName = filterName.mid(1,filterName.length()-2);
    }
    return filterName;
  }
}


bool transcodeCharacterStringToUTF8(std::string &input, const char *inputEncoding)
{
  const char *outputEncoding = "UTF-8";
  if (inputEncoding==nullptr || qstricmp(inputEncoding,outputEncoding)==0) return true;
  size_t inputSize=input.length();
  size_t outputSize=inputSize*4;
  QCString output(outputSize, QCString::ExplicitSize);
  void *cd = portable_iconv_open(outputEncoding,inputEncoding);
  if (cd==reinterpret_cast<void *>(-1))
  {
    return false;
  }
  bool ok=true;
  size_t iLeft=inputSize;
  size_t oLeft=outputSize;
  const char *inputPtr = input.data();
  char *outputPtr = output.rawData();
  if (!portable_iconv(cd, &inputPtr, &iLeft, &outputPtr, &oLeft))
  {
    outputSize-=static_cast<int>(oLeft);
    output.resize(outputSize);
    output.at(outputSize)='\0';
    // replace input
    input=output.str();
    //printf("iconv: input size=%d output size=%d\n[%s]\n",size,newSize,qPrint(srcBuf));
  }
  else
  {
    ok=false;
  }
  portable_iconv_close(cd);
  return ok;
}

/*! reads a file with name \a name and returns it as a string. If \a filter
 *  is TRUE the file will be filtered by any user specified input filter.
 *  If \a name is "-" the string will be read from standard input.
 */
QCString fileToString(const QCString &name,bool filter,bool isSourceCode)
{
  if (name.isEmpty()) return QCString();
  bool fileOpened=false;
  if (name[0]=='-' && name[1]==0) // read from stdin
  {
    std::string contents;
    std::string line;
    while (getline(std::cin,line))
    {
      contents+=line+'\n';
    }
    return QCString(contents);
  }
  else // read from file
  {
    FileInfo fi(name.str());
    if (!fi.exists() || !fi.isFile())
    {
      err("file '%s' not found\n",qPrint(name));
      return "";
    }
    std::string buf;
    fileOpened=readInputFile(name,buf,filter,isSourceCode);
    if (fileOpened)
    {
      addTerminalCharIfMissing(buf,'\n');
      return buf;
    }
  }
  if (!fileOpened)
  {
    err("cannot open file '%s' for reading\n",qPrint(name));
  }
  return "";
}

void trimBaseClassScope(const BaseClassList &bcl,QCString &s,int level=0)
{
  //printf("trimBaseClassScope level=%d '%s'\n",level,qPrint(s));
  for (const auto &bcd : bcl)
  {
    ClassDef *cd=bcd.classDef;
    //printf("Trying class %s\n",qPrint(cd->name()));
    int spos=s.find(cd->name()+"::");
    if (spos!=-1)
    {
      s = s.left(spos)+s.right(
          s.length()-spos-cd->name().length()-2
          );
    }
    //printf("base class '%s'\n",qPrint(cd->name()));
    if (!cd->baseClasses().empty())
    {
      trimBaseClassScope(cd->baseClasses(),s,level+1);
    }
  }
}

static void stripIrrelevantString(QCString &target,const QCString &str)
{
  if (target==str) { target.clear(); return; }
  int i=0,p=0;
  int l=static_cast<int>(str.length());
  bool changed=FALSE;
  while ((i=target.find(str,p))!=-1)
  {
    bool isMatch = (i==0 || !isId(target.at(i-1))) && // not a character before str
      (i+l==static_cast<int>(target.length()) || !isId(target.at(i+l))); // not a character after str
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
  //printf("stripIrrelevantConstVolatile(%s)=",qPrint(s));
  stripIrrelevantString(s,"const");
  stripIrrelevantString(s,"volatile");
  stripIrrelevantString(s,"final");
  //printf("%s\n",qPrint(s));
}


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
static QCString extractCanonicalType(const Definition *d,const FileDef *fs,QCString type,SrcLangExt lang);

static QCString getCanonicalTemplateSpec(const Definition *d,const FileDef *fs,const QCString& spec,SrcLangExt lang)
{

  QCString templSpec = spec.stripWhiteSpace();
  // this part had been commented out before... but it is needed to match for instance
  // std::list<std::string> against list<string> so it is now back again!
  if (!templSpec.isEmpty() && templSpec.at(0) == '<')
  {
    templSpec = "< " + extractCanonicalType(d,fs,templSpec.right(templSpec.length()-1).stripWhiteSpace(),lang);
  }
  QCString resolvedType = lang==SrcLangExt::Java ? templSpec : resolveTypeDef(d,templSpec);
  if (!resolvedType.isEmpty()) // not known as a typedef either
  {
    templSpec = resolvedType;
  }
  //printf("getCanonicalTemplateSpec(%s)=%s\n",qPrint(spec),qPrint(templSpec));
  return templSpec;
}


static QCString getCanonicalTypeForIdentifier(
    const Definition *d,const FileDef *fs,const QCString &word,SrcLangExt lang,
    QCString *tSpec,int count=0)
{
  if (count>10) return word; // oops recursion

  QCString symName,result,templSpec,tmpName;
  if (tSpec && !tSpec->isEmpty())
    templSpec = stripDeclKeywords(getCanonicalTemplateSpec(d,fs,*tSpec,lang));

  AUTO_TRACE("d='{}' fs='{}' word='{}' templSpec='{}'",d?d->name():"",fs?fs->name():"",word,templSpec);

  if (word.findRev("::")!=-1 && !(tmpName=stripScope(word)).isEmpty())
  {
    symName=tmpName; // name without scope
  }
  else
  {
    symName=word;
  }

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
  if (cd && cd->isUsedOnly()) cd=nullptr; // ignore types introduced by usage relations

  AUTO_TRACE_ADD("cd='{}' mType='{}' ts='{}' resolvedType='{}'",
      cd?cd->name():"",mType?mType->name():"",ts,resolvedType);
  //printf("cd=%p mtype=%p\n",cd,mType);
  //printf("  getCanonicalTypeForIdentifier: symbol=%s word=%s cd=%s d=%s fs=%s cd->isTemplate=%d\n",
  //    qPrint(symName),
  //    qPrint(word),
  //    cd ? qPrint(cd->name()) : "<none>",
  //    d  ? qPrint( d->name()) : "<none>",
  //    fs ? qPrint(fs->name()) : "<none>",
  //    cd ? cd->isTemplate():-1
  //   );

  //printf("  >>>> word '%s' => '%s' templSpec=%s ts=%s tSpec=%s isTemplate=%d resolvedType=%s\n",
  //    qPrint((word+templSpec)),
  //    cd ? qPrint(cd->qualifiedName()) : "<none>",
  //    qPrint(templSpec), qPrint(ts),
  //    tSpec ? qPrint(tSpec) : "<null>",
  //    cd ? cd->isTemplate():FALSE,
  //    qPrint(resolvedType));

  //printf("  mtype=%s\n",mType ? qPrint(mType->name()) : "<none>");

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
        templSpec = stripDeclKeywords(getCanonicalTemplateSpec(d,fs,ts,lang));
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
    //printf("word=%s typeString=%s\n",qPrint(word),mType->typeString());
    if (word!=mType->typeString())
    {
      QCString type = mType->typeString();
      if (type.startsWith("typename "))
      {
        type.stripPrefix("typename ");
        type = stripTemplateSpecifiersFromScope(type,FALSE);
      }
      if (!type.isEmpty()) // see issue #11065
      {
        result = getCanonicalTypeForIdentifier(d,fs,type,mType->getLanguage(),tSpec,count+1);
      }
      else
      {
        result = word;
      }
    }
    else
    {
      result = mType->typeString();
    }
  }
  else // fallback
  {
    resolvedType = lang==SrcLangExt::Java ? word : resolveTypeDef(d,word);
    AUTO_TRACE_ADD("fallback resolvedType='{}'",resolvedType);
    if (resolvedType.isEmpty()) // not known as a typedef either
    {
      result = word;
    }
    else
    {
      result = resolvedType;
    }
  }
  AUTO_TRACE_EXIT("result='{}'",result);
  return result;
}

static QCString extractCanonicalType(const Definition *d,const FileDef *fs,QCString type,SrcLangExt lang)
{
  AUTO_TRACE("d={} fs={} type='{}'",d?d->name():"",fs?fs->name():"",type);
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
  //printf("extractCanonicalType(type=%s) start: def=%s file=%s\n",qPrint(type),
  //    d ? qPrint(d->name()) : "<null>", fs ? qPrint(fs->name()) : "<null>");

  QCString canType;
  QCString templSpec,word;
  int i=0,p=0,pp=0;
  while ((i=extractClassNameFromType(type,p,word,templSpec))!=-1)
    // foreach identifier in the type
  {
    //printf("     i=%d p=%d\n",i,p);
    if (i>pp) canType += type.mid(pp,i-pp);

    QCString ct = getCanonicalTypeForIdentifier(d,fs,word,lang,&templSpec);

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
    //    qPrint(word), qPrint(templSpec), qPrint(canType), qPrint(ct));
    if (!templSpec.isEmpty()) // if we didn't use up the templSpec already
                              // (i.e. type is not a template specialization)
                              // then resolve any identifiers inside.
    {
      std::string ts = templSpec.str();
      static const reg::Ex re(R"(\a\w*)");
      reg::Iterator it(ts,re);
      reg::Iterator end;

      size_t tp=0;
      // for each identifier template specifier
      //printf("adding resolved %s to %s\n",qPrint(templSpec),qPrint(canType));
      for (; it!=end ; ++it)
      {
        const auto &match = *it;
        size_t ti = match.position();
        size_t tl = match.length();
        std::string matchStr = match.str();
        canType += ts.substr(tp,ti-tp);
        canType += getCanonicalTypeForIdentifier(d,fs,matchStr.c_str(),lang,nullptr);
        tp=ti+tl;
      }
      canType+=ts.substr(tp);
    }

    pp=p;
  }
  canType += type.right(type.length()-pp);
  AUTO_TRACE_EXIT("canType='{}'",canType);

  return removeRedundantWhiteSpace(canType);
}

static QCString extractCanonicalArgType(const Definition *d,const FileDef *fs,const Argument &arg,SrcLangExt lang)
{
  QCString type = arg.type.stripWhiteSpace();
  QCString name = arg.name;
  //printf("----- extractCanonicalArgType(type=%s,name=%s)\n",qPrint(type),qPrint(name));
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

  return extractCanonicalType(d,fs,type,lang);
}

static std::mutex g_matchArgsMutex;

// a bit of debug support for matchArguments
#define MATCH
#define NOMATCH
//#define MATCH   printf("Match at line %d\n",__LINE__);
//#define NOMATCH printf("Nomatch at line %d\n",__LINE__);
//#define MATCH   AUTO_TRACE_EXIT("match at line {}",__LINE__);
//#define NOMATCH AUTO_TRACE_EXIT("no match at line {}",__LINE__);

static bool matchCanonicalTypes(
    const Definition *srcScope,const FileDef *srcFileScope,const QCString &srcType,
    const Definition *dstScope,const FileDef *dstFileScope,const QCString &dstType,
    SrcLangExt lang)
{
  AUTO_TRACE("srcType='{}' dstType='{}'",srcType,dstType);
  if (srcType==dstType) return true;

  // check if the types are function pointers
  int i1=srcType.find(")(");
  if (i1==-1) return false;
  int i2=dstType.find(")(");
  if (i1!=i2) return false;

  // check if the result part of the function pointer types matches
  int j1=srcType.find("(");
  if (j1==-1 || j1>i1) return false;
  int j2=dstType.find("(");
  if (j2!=j1) return false;
  if (srcType.left(j1)!=dstType.left(j2)) return false; // different return types

  // if srcType and dstType are both function pointers with the same return type,
  // then match against the parameter lists.
  // This way srcType='void (*fptr)(int x)' will match against `void (*fptr)(int y)' because
  // 'int x' matches 'int y'. A simple literal string match would treat these as different.
  auto srcAl = stringToArgumentList(lang,srcType.mid(i1+1));
  auto dstAl = stringToArgumentList(lang,dstType.mid(i2+1));
  return matchArguments2(srcScope,srcFileScope,srcAl.get(),
                         dstScope,dstFileScope,dstAl.get(),
                         true,lang);
}

static bool matchArgument2(
    const Definition *srcScope,const FileDef *srcFileScope,Argument &srcA,
    const Definition *dstScope,const FileDef *dstFileScope,Argument &dstA,
    SrcLangExt lang
    )
{
  AUTO_TRACE("src: scope={} type={} name={} canType={}, dst: scope={} type={} name={} canType={}",
      srcScope?srcScope->name():"",srcA.type,srcA.name,srcA.canType,
      dstScope?dstScope->name():"",dstA.type,dstA.name,dstA.canType);
  //printf(">> match argument: %s::'%s|%s' (%s) <-> %s::'%s|%s' (%s)\n",
  //    srcScope ? qPrint(srcScope->name()) : "",
  //    qPrint(srcA.type), qPrint(srcA.name), qPrint(srcA.canType),
  //    dstScope ? qPrint(dstScope->name()) : "",
  //    qPrint(dstA.type), qPrint(dstA.name), qPrint(dstA.canType));

  QCString sSrcName = " "+srcA.name;
  QCString sDstName = " "+dstA.name;
  QCString srcType  = srcA.type;
  QCString dstType  = dstA.type;
  stripIrrelevantConstVolatile(srcType);
  stripIrrelevantConstVolatile(dstType);
  //printf("'%s'<->'%s'\n",qPrint(sSrcName),qPrint(dstType.right(sSrcName.length())));
  //printf("'%s'<->'%s'\n",qPrint(sDstName),qPrint(srcType.right(sDstName.length())));
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

  {
    std::lock_guard lock(g_matchArgsMutex);
    if (srcA.canType.isEmpty() || dstA.canType.isEmpty())
    {
      // need to re-evaluate both see issue #8370
      srcA.canType = extractCanonicalArgType(srcScope,srcFileScope,srcA,lang);
      dstA.canType = extractCanonicalArgType(dstScope,dstFileScope,dstA,lang);
    }
  }

  if (matchCanonicalTypes(srcScope,srcFileScope,srcA.canType,
                          dstScope,dstFileScope,dstA.canType,
                          lang))
  {
    MATCH
    AUTO_TRACE_EXIT("true");
    return TRUE;
  }
  else
  {
    //printf("   Canonical types do not match [%s]<->[%s]\n",
    //    qPrint(srcA->canType),qPrint(dstA->canType));
    NOMATCH
    AUTO_TRACE_EXIT("false");
    return FALSE;
  }
}


// new algorithm for argument matching
bool matchArguments2(const Definition *srcScope,const FileDef *srcFileScope,const ArgumentList *srcAl,
                               const Definition *dstScope,const FileDef *dstFileScope,const ArgumentList *dstAl,
                               bool checkCV,SrcLangExt lang)
{
  ASSERT(srcScope!=nullptr && dstScope!=nullptr);

  AUTO_TRACE("srcScope='{}' dstScope='{}' srcArgs='{}' dstArgs='{}' checkCV={} lang={}",
      srcScope->name(),dstScope->name(),srcAl?argListToString(*srcAl):"",dstAl?argListToString(*dstAl):"",checkCV,lang);

  if (srcAl==nullptr || dstAl==nullptr)
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
                        dstScope,dstFileScope,dstA,
                        lang)
       )
    {
      NOMATCH
      return FALSE;
    }
  }
  MATCH
  return TRUE; // all arguments match
}

#undef MATCH
#undef NOMATCH

// merges the initializer of two argument lists
// pre:  the types of the arguments in the list should match.
void mergeArguments(ArgumentList &srcAl,ArgumentList &dstAl,bool forceNameOverwrite)
{
  AUTO_TRACE("srcAl='{}',dstAl='{}',forceNameOverwrite={}",
             qPrint(argListToString(srcAl)),qPrint(argListToString(dstAl)),forceNameOverwrite);

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

    AUTO_TRACE_ADD("before merge: src=[type='{}',name='{}',def='{}'] dst=[type='{}',name='{}',def='{}']",
        srcA.type,srcA.name,srcA.defval,
        dstA.type,dstA.name,dstA.defval);
    if (srcA.defval.isEmpty() && !dstA.defval.isEmpty())
    {
      //printf("Defval changing '%s'->'%s'\n",qPrint(srcA.defval),qPrint(dstA.defval));
      srcA.defval=dstA.defval;
    }
    else if (!srcA.defval.isEmpty() && dstA.defval.isEmpty())
    {
      //printf("Defval changing '%s'->'%s'\n",qPrint(dstA.defval),qPrint(srcA.defval));
      dstA.defval=srcA.defval;
    }

    // fix wrongly detected const or volatile specifiers before merging.
    // example: "const A *const" is detected as type="const A *" name="const"
    if (srcA.name=="const" || srcA.name=="volatile")
    {
      srcA.type+=" "+srcA.name;
      srcA.name.clear();
    }
    if (dstA.name=="const" || dstA.name=="volatile")
    {
      dstA.type+=" "+dstA.name;
      dstA.name.clear();
    }

    if (srcA.type==dstA.type)
    {
      //printf("1. merging %s:%s <-> %s:%s\n",qPrint(srcA.type),qPrint(srcA.name),qPrint(dstA.type),qPrint(dstA.name));
      if (srcA.name.isEmpty() && !dstA.name.isEmpty())
      {
        //printf("type: '%s':='%s'\n",qPrint(srcA.type),qPrint(dstA.type));
        //printf("name: '%s':='%s'\n",qPrint(srcA.name),qPrint(dstA.name));
        srcA.type = dstA.type;
        srcA.name = dstA.name;
      }
      else if (!srcA.name.isEmpty() && dstA.name.isEmpty())
      {
        //printf("type: '%s':='%s'\n",qPrint(dstA.type),qPrint(srcA.type));
        //printf("name: '%s':='%s'\n",qPrint(dstA.name),qPrint(srcA.name));
        dstA.type = srcA.type;
        dstA.name = srcA.name;
      }
      else if (!srcA.name.isEmpty() && !dstA.name.isEmpty())
      {
        //printf("srcA.name=%s dstA.name=%s\n",qPrint(srcA.name),qPrint(dstA.name));
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
      //printf("2. merging '%s':'%s' <-> '%s':'%s'\n",qPrint(srcA.type),qPrint(srcA.name),qPrint(dstA.type),qPrint(dstA.name));
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
        j1=static_cast<int>(srcA.type.length())-i1-2,
        j2=static_cast<int>(dstA.type.length())-i2-2;
    if (i1!=-1 && i2==-1 && srcA.type.right(j1)==dstA.type)
    {
      //printf("type: '%s':='%s'\n",qPrint(dstA.type),qPrint(srcA.type));
      //printf("name: '%s':='%s'\n",qPrint(dstA.name),qPrint(srcA.name));
      dstA.type = srcA.type.left(i1+2)+dstA.type;
      dstA.name = srcA.name;
    }
    else if (i1==-1 && i2!=-1 && dstA.type.right(j2)==srcA.type)
    {
      //printf("type: '%s':='%s'\n",qPrint(srcA.type),qPrint(dstA.type));
      //printf("name: '%s':='%s'\n",qPrint(dstA.name),qPrint(srcA.name));
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
    //  qPrint(srcA.type), qPrint(srcA.name),
    //  qPrint(dstA.type), qPrint(dstA.name));
    ++srcIt;
    ++dstIt;
    AUTO_TRACE_ADD("after merge: src=[type='{}',name='{}',def='{}'] dst=[type='{}',name='{}',def='{}']",
        srcA.type,srcA.name,srcA.defval,
        dstA.type,dstA.name,dstA.defval);
  }
}

//---------------------------------------------------------------------------------------

static void findMembersWithSpecificName(const MemberName *mn,
                                        const QCString &args,
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
    //    qPrint(md->name()),args,fd,gd,currentFile,qPrint(md->getReference()));
    if (
        ((gd && gd->isLinkable()) || (fd && fd->isLinkable()) || md->isReference()) &&
        md->getNamespaceDef()==nullptr && md->isLinkable() &&
        (!checkStatics || (!md->isStatic() && !md->isDefine()) ||
         currentFile==nullptr || fd==currentFile) // statics must appear in the same file
       )
    {
      bool match=TRUE;
      if (!args.isEmpty() && !md->isDefine() && args!="()")
      {
        const ArgumentList &mdAl = md->argumentList();
        auto argList_p = stringToArgumentList(md->getLanguage(),args);
        match=matchArguments2(
            md->getOuterScope(),fd,&mdAl,
            Doxygen::globalScope,fd,argList_p.get(),
            checkCV,md->getLanguage());
      }
      if (match)
      {
        //printf("Found match!\n");
        members.push_back(md);
      }
    }
  }
}

//---------------------------------------------------------------------------------------

GetDefResult getDefsNew(const GetDefInput &input)
{
  GetDefResult result;
  if (input.memberName.isEmpty()) return result;

  //printf("@@ --- getDefsNew(%s,%s)-----------\n",qPrint(scName),qPrint(mbName));
  const Definition *scope = Doxygen::globalScope;
  SymbolResolver resolver;
  if (input.currentFile) resolver.setFileScope(input.currentFile);
  if (!input.scopeName.isEmpty())
  {
    scope = resolver.resolveSymbol(scope,input.scopeName);
  }
  if (scope==Doxygen::globalScope)
  {
    scope = input.currentFile;
  }
  //printf("@@  -> found scope scope=%s member=%s out=%s\n",qPrint(input.scopeName),qPrint(input.memberName),qPrint(scope?scope->name():""));
  //
  const Definition *symbol = resolver.resolveSymbol(scope,input.memberName,input.args,input.checkCV,input.insideCode);
  //printf("@@  -> found symbol in=%s out=%s\n",qPrint(input.memberName),qPrint(symbol?symbol->qualifiedName():QCString()));
  if (symbol && symbol->definitionType()==Definition::TypeMember)
  {
    result.md = toMemberDef(symbol);
    result.cd = result.md->getClassDef();
    if (result.cd==nullptr) result.nd = result.md->getNamespaceDef();
    if (result.cd==nullptr && result.nd==nullptr) result.fd = result.md->getFileDef();
    result.gd = result.md->getGroupDef();
    result.found = true;
  }
  return result;
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
GetDefResult getDefsOld(const GetDefInput &input)
{
  GetDefResult result;
  QCString scopeName;
  QCString memberName;
  QCString mName;
  QCString mScope;
  MemberName *mn = nullptr;
  int is=0,im=0,pm=0;

  if (input.memberName.isEmpty()) goto exit; /* empty name => nothing to link */

  scopeName  = input.scopeName;
  scopeName  = substitute(scopeName,"\\","::"); // for PHP
  memberName = input.memberName;
  memberName = substitute(memberName,"\\","::"); // for PHP
  //printf("Search for name=%s args=%s in scope=%s forceEmpty=%d\n",
  //          qPrint(memberName),qPrint(args),qPrint(scopeName),forceEmptyScope);

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
  //          qPrint(scopeName), qPrint(memberName));

  mName=memberName;
  if (!memberName.startsWith("operator ") && // treat operator conversion methods
      // as a special case
      (im=memberName.findRev("::"))!=-1 &&
      im<static_cast<int>(memberName.length())-2 // not A::
     )
  {
    mScope=memberName.left(im);
    mName=memberName.right(memberName.length()-im-2);
  }

  // handle special the case where both scope name and member scope are equal
  if (mScope==scopeName) scopeName.clear();

  //printf("mScope='%s' mName='%s'\n",qPrint(mScope),qPrint(mName));

  mn = Doxygen::memberNameLinkedMap->find(mName);
  //printf("mName=%s mn=%p\n",qPrint(mName),mn);

  if ((!input.forceEmptyScope || input.scopeName.isEmpty()) && // this was changed for bug638856, forceEmptyScope => empty scopeName
      mn && !(input.scopeName.isEmpty() && mScope.isEmpty()))
  {
    //printf("  >member name '%s' found\n",qPrint(mName));
    int scopeOffset = static_cast<int>(scopeName.length());
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

      if (fcd==nullptr && className.find('<')!=-1) // try without template specifiers as well
      {
         QCString nameWithoutTemplates = stripTemplateSpecifiersFromScope(className,FALSE);
         fcd=resolver.resolveClass(Doxygen::globalScope,nameWithoutTemplates);
         tmd=resolver.getTypedef();
      }
      //printf("Trying class scope %s: fcd=%p tmd=%p\n",qPrint(className),fcd,tmd);
      // todo: fill in correct fileScope!
      if (fcd &&  // is it a documented class
          fcd->isLinkable()
         )
      {
        //printf("  Found fcd=%p\n",fcd);
        int mdist=maxInheritanceDepth;
        std::unique_ptr<ArgumentList> argList;
        if (!input.args.isEmpty())
        {
          argList = stringToArgumentList(fcd->getLanguage(),input.args);
        }
        for (const auto &mmd_p : *mn)
        {
          MemberDef *mmd = mmd_p.get();
          if (!mmd->isStrongEnumValue())
          {
            const ArgumentList &mmdAl = mmd->argumentList();
            bool match = input.args.isEmpty() ||
              matchArguments2(mmd->getOuterScope(),mmd->getFileDef(),&mmdAl,
                             fcd,                  fcd->getFileDef(),argList.get(),
                             input.checkCV,mmd->getLanguage());
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
                  result.cd=mcd;
                  result.md=mmd;
                }
              }
            }
          }
        }
        if (mdist==maxInheritanceDepth && input.args=="()")
          // no exact match found, but if args="()" an arbitrary member will do
        {
          //printf("  >Searching for arbitrary member\n");
          for (const auto &mmd_p : *mn)
          {
            MemberDef *mmd = mmd_p.get();
            //if (mmd->isLinkable())
            //{
            const ClassDef *mcd=mmd->getClassDef();
            //printf("  >Class %s found\n",qPrint(mcd->name()));
            if (mcd)
            {
              int m=minClassDistance(fcd,mcd);
              if (m<mdist /* && mcd->isLinkable()*/ )
              {
                //printf("Class distance %d\n",m);
                mdist = m;
                result.cd = mcd;
                result.md = mmd;
              }
            }
            //}
          }
        }
        //printf("  >Success=%d\n",mdist<maxInheritanceDepth);
        if (mdist<maxInheritanceDepth)
        {
          if (!result.md->isLinkable() || result.md->isStrongEnumValue())
          {
            result.md = nullptr; // avoid returning things we cannot link to
            result.cd = nullptr;
            result.found = false; // match found, but was not linkable
            goto exit;
          }
          else
          {
            result.gd = result.md->getGroupDef();
            if (result.gd) result.cd=nullptr;
            result.found=true; /* found match */
            goto exit;
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
              result.cd = tmd->getClassDef();
              result.md = emd;
              result.found = true;
              goto exit;
            }
            else
            {
              result.cd = nullptr;
              result.md = nullptr;
              result.found = false;
              goto exit;
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
  if (mn && input.scopeName.isEmpty() && mScope.isEmpty()) // Maybe a related function?
  {
    //printf("Global symbol\n");
    const MemberDef *fuzzy_mmd = nullptr;
    std::unique_ptr<ArgumentList> argList;
    bool hasEmptyArgs = input.args=="()";

    if (!input.args.isEmpty())
    {
      argList = stringToArgumentList(SrcLangExt::Cpp, input.args);
    }

    for (const auto &mmd_p : *mn)
    {
      const MemberDef *mmd = mmd_p.get();
      if (!mmd->isLinkable() || (!mmd->isRelated() && !mmd->isForeign()) ||
           !mmd->getClassDef())
      {
        continue;
      }

      if (input.args.isEmpty())
      {
        fuzzy_mmd = mmd;
        break;
      }

      const ArgumentList &mmdAl = mmd->argumentList();
      if (matchArguments2(mmd->getOuterScope(),mmd->getFileDef(),&mmdAl,
                          Doxygen::globalScope,mmd->getFileDef(),argList.get(),
                          input.checkCV,mmd->getLanguage())
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
      result.md = fuzzy_mmd;
      result.cd = fuzzy_mmd->getClassDef();
      result.found = true;
      goto exit;
    }
  }


  // maybe an namespace, file or group member ?
  //printf("Testing for global symbol scopeName='%s' mScope='%s' :: mName='%s'\n",
  //              qPrint(scopeName), qPrint(mScope), qPrint(mName));
  if ((mn=Doxygen::functionNameLinkedMap->find(mName))) // name is known
  {
    //printf("  >symbol name found\n");
    NamespaceDef *fnd=nullptr;
    int scopeOffset = static_cast<int>(scopeName.length());
    do
    {
      QCString namespaceName = scopeName.left(scopeOffset);
      if (!namespaceName.isEmpty() && !mScope.isEmpty())
      {
        namespaceName+="::"+mScope;
      }
      else if (!mScope.isEmpty())
      {
        namespaceName=mScope;
      }
      //printf("Trying namespace %s\n",qPrint(namespaceName));
      if (!namespaceName.isEmpty() &&
          (fnd=Doxygen::namespaceLinkedMap->find(namespaceName)) &&
          fnd->isLinkable()
         )
      {
        //printf("Symbol inside existing namespace '%s' count=%d\n",
        //    qPrint(namespaceName),mn->count());
        bool found=FALSE;
        for (const auto &mmd_p : *mn)
        {
          const MemberDef *mmd = mmd_p.get();
          //printf("mmd->getNamespaceDef()=%p fnd=%p\n",
          //    mmd->getNamespaceDef(),fnd);
          const MemberDef *emd = mmd->getEnumScope();
          if (emd && emd->isStrong())
          {
            //printf("yes match %s<->%s!\n",qPrint(mScope),qPrint(emd->localName()));
            if (emd->getNamespaceDef()==fnd &&
                rightScopeMatch(mScope,emd->localName()))
            {
              //printf("found it!\n");
              result.nd=fnd;
              result.md=mmd;
              found=TRUE;
              break;
            }
            else
            {
              result.md=nullptr;
              result.cd=nullptr;
              result.found=false;
              goto exit;
            }
          }
          else if (mmd->getOuterScope()==fnd /* && mmd->isLinkable() */ )
          { // namespace is found
            bool match=TRUE;
            if (!input.args.isEmpty() && input.args!="()")
            {
              const ArgumentList &mmdAl = mmd->argumentList();
              auto argList_p = stringToArgumentList(mmd->getLanguage(),input.args);
              match=matchArguments2(
                  mmd->getOuterScope(),mmd->getFileDef(),&mmdAl,
                  fnd,mmd->getFileDef(),argList_p.get(),
                  input.checkCV,mmd->getLanguage());
            }
            if (match)
            {
              result.nd=fnd;
              result.md=mmd;
              found=TRUE;
              break;
            }
          }
        }
        if (!found && input.args=="()")
          // no exact match found, but if args="()" an arbitrary
          // member will do
        {
          for (const auto &mmd_p : *mn)
          {
            const MemberDef *mmd = mmd_p.get();
            if (mmd->getNamespaceDef()==fnd /*&& mmd->isLinkable() */ )
            {
              result.nd=fnd;
              result.md=mmd;
              found=TRUE;
              break;
            }
          }
        }
        if (found)
        {
          if (!result.md->isLinkable())
          {
            result.md=nullptr; // avoid returning things we cannot link to
            result.nd=nullptr;
            result.found=false; // match found but not linkable
            goto exit;
          }
          else
          {
            result.gd=result.md->resolveAlias()->getGroupDef();
            if (result.gd && result.gd->isLinkable()) result.nd=nullptr; else result.gd=nullptr;
            result.found = true;
            goto exit;
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
          //printf("try member %s tmd=%s\n",qPrint(mmd->name()),tmd ? qPrint(tmd->name()) : "<none>");
          int ni=namespaceName.findRev("::");
          //printf("namespaceName=%s ni=%d\n",qPrint(namespaceName),ni);
          bool notInNS = tmd && ni==-1 && tmd->getNamespaceDef()==nullptr && (mScope.isEmpty() || mScope==tmd->name());
          bool sameNS  = tmd && ni>=0 && tmd->getNamespaceDef() && namespaceName.left(ni)==tmd->getNamespaceDef()->name() && namespaceName.mid(ni+2)==tmd->name();
          //printf("notInNS=%d sameNS=%d\n",notInNS,sameNS);
          if (tmd && tmd->isStrong() && // C++11 enum class
              (notInNS || sameNS) &&
              namespaceName.length()>0  // enum is part of namespace so this should not be empty
             )
          {
            result.md=mmd;
            result.fd=mmd->getFileDef();
            result.gd=mmd->getGroupDef();
            if (result.gd && result.gd->isLinkable()) result.fd=nullptr; else result.gd=nullptr;
            //printf("Found scoped enum %s fd=%p gd=%p\n",
            //    qPrint(mmd->name()),result.fd,result.gd);
            result.found = true;
            goto exit;
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
      findMembersWithSpecificName(mn,input.args,true,input.currentFile,input.checkCV,members);
      if (members.empty()) // nothing found
      {
        // search again without strict static checking
        findMembersWithSpecificName(mn,input.args,false,input.currentFile,input.checkCV,members);
      }
      //printf("found %d members\n",members.count());
      if (members.size()!=1 && input.args=="()")
      {
        // no exact match found, but if args="()" an arbitrary
        // member will do
        //MemberNameIterator mni(*mn);
        //for (mni.toLast();(md=mni.current());--mni)
        for (auto it = mn->rbegin(); it!=mn->rend(); ++it)
        {
          const auto &mmd_p = *it;
          const MemberDef *mmd = mmd_p.get();
          //printf("Found member '%s'\n",qPrint(mmd->name()));
          //printf("member is linkable mmd->name()='%s'\n",qPrint(mmd->name()));
          result.fd = mmd->getFileDef();
          result.gd = mmd->getGroupDef();
          const MemberDef *tmd = mmd->getEnumScope();
          if (
              (result.gd && result.gd->isLinkable()) || (result.fd && result.fd->isLinkable()) ||
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
        if (input.currentFile)
        {
          //printf("multiple results; pick one from file:%s\n",qPrint( currentFile->name()));
          for (const auto &rmd : members)
          {
            if (rmd->getFileDef() && rmd->getFileDef()->name() == input.currentFile->name())
            {
              result.md = rmd;
              break; // found match in the current file
            }
          }
          if (!result.md) // member not in the current file
          {
            result.md = members.back();
          }
        }
        else
        {
          result.md = members.back();
        }
      }
      if (result.md && (result.md->getEnumScope()==nullptr || !result.md->getEnumScope()->isStrong()))
           // found a matching global member, that is not a scoped enum value (or uniquely matches)
      {
        result.fd = result.md->getFileDef();
        result.gd = result.md->getGroupDef();
        //printf("fd=%p gd=%p gd->isLinkable()=%d\n",fd,gd,gd->isLinkable());
        if (result.gd && result.gd->isLinkable()) result.fd=nullptr; else result.gd=nullptr;
        result.found = true;
        goto exit;
      }
    }
  }

exit:
  return result;
}

GetDefResult getDefs(const GetDefInput &input)
{
  if (false) // set this to true to try the old and new routine side-by-side and compare the results
  {
    printf("@@ ------ getDefsOld start\n");
    GetDefResult result = getDefsOld(input);
    printf("@@ ------ getDefsOld end\n");
    printf("@@ ------ getDefsNew start\n");
    GetDefResult newResult = getDefsNew(input);
    printf("@@ ------ getDefsNew end\n");
    if (result.found!=newResult.found ||
        result.md!=newResult.md ||
        result.cd!=newResult.cd ||
        result.fd!=newResult.fd ||
        result.nd!=newResult.nd ||
        result.gd!=newResult.gd
       )
    {
      printf("@@ getDefsOld(scName=%s, mbName=%s, args=%s, forceEmptyScope=%d "
                      "currentFile=%s checkCV=%d)=%d md=%s (%p) cd=%s fd=%s nd=%s gd=%s\n",
                qPrint(input.scopeName), qPrint(input.memberName), qPrint(input.args),
                input.forceEmptyScope, qPrint(input.currentFile?input.currentFile->name():QCString()),
                input.checkCV,
                result.found,
                qPrint(result.md ? result.md->name() : QCString()),
                (void*)result.md,
                qPrint(result.cd ? result.cd->name() : QCString()),
                qPrint(result.fd ? result.fd->name() : QCString()),
                qPrint(result.nd ? result.nd->name() : QCString()),
                qPrint(result.gd ? result.gd->name() : QCString())
             );
      printf("@@ ------ getDefsOld start\n");
      printf("@@ getDefsNew(scName=%s, mbName=%s, args=%s, forceEmptyScope=%d "
                      "currentFile=%s checkCV=%d)=%d md=%s (%p) cd=%s fd=%s nd=%s gd=%s\n",
                qPrint(input.scopeName), qPrint(input.memberName), qPrint(input.args),
                input.forceEmptyScope, qPrint(input.currentFile?input.currentFile->name():QCString()),
                input.checkCV,
                newResult.found,
                qPrint(newResult.md ? newResult.md->name() : QCString()),
                (void*)newResult.md,
                qPrint(newResult.cd ? newResult.cd->name() : QCString()),
                qPrint(newResult.fd ? newResult.fd->name() : QCString()),
                qPrint(newResult.nd ? newResult.nd->name() : QCString()),
                qPrint(newResult.gd ? newResult.gd->name() : QCString())
             );
    }
    return result; // use return newResult to use the result of the new routine
  }
  else // do one of the two getDefs routines (comment out the other one)
  {
    return getDefsNew(input);
  }
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
static bool getScopeDefs(const QCString &docScope,const QCString &scope,
    ClassDef *&cd, ConceptDef *&cnd, NamespaceDef *&nd)
{
  cd=nullptr;nd=nullptr;

  QCString scopeName=scope;
  //printf("getScopeDefs: docScope='%s' scope='%s'\n",qPrint(docScope),qPrint(scope));
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
  int scopeOffset=explicitGlobalScope ? 0 : static_cast<int>(docScopeName.length());

  do // for each possible docScope (from largest to and including empty)
  {
    QCString fullName=scopeName;
    if (scopeOffset>0) fullName.prepend(docScopeName.left(scopeOffset)+"::");

    if (((cd=getClass(fullName)) ||         // normal class
         (cd=getClass(fullName+"-p"))       // ObjC protocol
        ) && cd->isLinkable())
    {
      return TRUE; // class link written => quit
    }
    else if ((nd=Doxygen::namespaceLinkedMap->find(fullName)) && nd->isLinkable())
    {
      return TRUE; // namespace link written => quit
    }
    else if ((cnd=Doxygen::conceptLinkedMap->find(fullName)) && cnd->isLinkable())
    {
      return TRUE; // concept link written => quit
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
  if (s.isEmpty()) return true;
  const char *p=s.data();
  int c=0;
  while ((c=static_cast<uint8_t>(*p++))) if (!islower(c)) return false;
  return true;
}

/*! Returns an object to reference to given its name and context
 *  @post return value TRUE implies *resContext!=0 or *resMember!=0
 */
bool resolveRef(/* in */  const QCString &scName,
    /* in */  const QCString &name,
    /* in */  bool inSeeBlock,
    /* out */ const Definition **resContext,
    /* out */ const MemberDef  **resMember,
    bool lookForSpecialization,
    const FileDef *currentFile,
    bool checkScope
    )
{
  AUTO_TRACE("scope={} name={} inSeeBlock={}",scName,name,inSeeBlock);
  //printf("resolveRef(scope=%s,name=%s,inSeeBlock=%d)\n",qPrint(scName),qPrint(name),inSeeBlock);
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
  int endNamePos=bracePos!=-1 ? bracePos : static_cast<int>(fullName.length());
  int scopePos=fullName.findRev("::",endNamePos);
  bool explicitScope = fullName.startsWith("::") &&   // ::scope or #scope
                       (scopePos>2 ||                 // ::N::A
                        tsName.startsWith("::") ||    // ::foo in local scope
                        scName==nullptr                     // #foo  in global scope
                       );
  bool allowTypeOnly=false;

  // default result values
  *resContext=nullptr;
  *resMember=nullptr;

  if (bracePos==-1) // simple name
  {
    // the following if() was commented out for releases in the range
    // 1.5.2 to 1.6.1, but has been restored as a result of bug report 594787.
    if (!inSeeBlock && scopePos==-1 && isLowerCase(tsName))
    { // link to lower case only name => do not try to autolink
      AUTO_TRACE_ADD("false");
      return FALSE;
    }

    ClassDef *cd=nullptr;
    NamespaceDef *nd=nullptr;
    ConceptDef *cnd=nullptr;

    //printf("scName=%s fullName=%s\n",qPrint(scName),qPrint(fullName));

    // check if this is a class or namespace reference
    if (scName!=fullName && getScopeDefs(scName,fullName,cd,cnd,nd))
    {
      //printf("found scopeDef\n");
      if (cd) // scope matches that of a class
      {
        *resContext = cd;
      }
      else if (cnd)
      {
        *resContext = cnd;
      }
      else // scope matches that of a namespace
      {
        ASSERT(nd!=nullptr);
        *resContext = nd;
      }
      AUTO_TRACE_ADD("true");
      return TRUE;
    }
    else if (scName==fullName || (!inSeeBlock && scopePos==-1))
      // nothing to link => output plain text
    {
      //printf("found scName=%s fullName=%s scName==fullName=%d "
      //    "inSeeBlock=%d scopePos=%d!\n",
      //    qPrint(scName),qPrint(fullName),scName==fullName,inSeeBlock,scopePos);

      // at this point we have a bare word that is not a class or namespace
      // we should also allow typedefs or enums to be linked, but not for instance member
      // functions, otherwise 'Foo' would always link to the 'Foo()' constructor instead of the
      // 'Foo' class. So we use this flag as a filter.
      allowTypeOnly=true;
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
  if (nameStr.length()>scopeStr.length() && leftScopeMatch(scopeStr,nameStr))
  {
    nameStr=nameStr.mid(scopeStr.length()+2);
  }

  const GroupDef     *gd = nullptr;
  const ConceptDef   *cnd = nullptr;

  // check if nameStr is a member or global.
  //printf("getDefs(scope=%s,name=%s,args=%s checkScope=%d)\n",
  //    qPrint(scopeStr), qPrint(nameStr), qPrint(argsStr),checkScope);
  GetDefInput input(scopeStr,nameStr,argsStr);
  input.forceEmptyScope = explicitScope;
  input.currentFile = currentFile;
  input.checkCV = true;
  GetDefResult result = getDefs(input);
  if (result.found)
  {
    //printf("after getDefs checkScope=%d nameStr=%s\n",checkScope,qPrint(nameStr));
    if (checkScope && result.md && result.md->getOuterScope()==Doxygen::globalScope &&
        !result.md->isStrongEnumValue() &&
        (!scopeStr.isEmpty() || nameStr.find("::")>0))
    {
      // we did find a member, but it is a global one while we were explicitly
      // looking for a scoped variable. See bug 616387 for an example why this check is needed.
      // note we do need to support autolinking to "::symbol" hence the >0
      //printf("not global member!\n");
      *resContext=nullptr;
      *resMember=nullptr;
      AUTO_TRACE_ADD("false");
      return FALSE;
    }
    //printf("after getDefs md=%p cd=%p fd=%p nd=%p gd=%p\n",md,cd,fd,nd,gd);
    if      (result.md) {
      if (!allowTypeOnly || result.md->isTypedef() || result.md->isEnumerate())
      {
        *resMember=result.md;
        *resContext=result.md;
      }
      else // md is not a type, but we explicitly expect one
      {
        *resContext=nullptr;
        *resMember=nullptr;
        AUTO_TRACE_ADD("false");
        return FALSE;
      }
    }
    else if (result.cd) *resContext=result.cd;
    else if (result.nd) *resContext=result.nd;
    else if (result.fd) *resContext=result.fd;
    else if (result.gd) *resContext=result.gd;
    else         { *resContext=nullptr; *resMember=nullptr; return FALSE; }
    //printf("member=%s (md=%p) anchor=%s linkable()=%d context=%s\n",
    //    qPrint(md->name()), md, qPrint(md->anchor()), md->isLinkable(), qPrint((*resContext)->name()));
    AUTO_TRACE_ADD("true");
    return TRUE;
  }
  else if (inSeeBlock && !nameStr.isEmpty() && (gd=Doxygen::groupLinkedMap->find(nameStr)))
  { // group link
    *resContext=gd;
    AUTO_TRACE_ADD("true");
    return TRUE;
  }
  else if ((cnd=Doxygen::conceptLinkedMap->find(nameStr)))
  {
    *resContext=cnd;
    AUTO_TRACE_ADD("true");
    return TRUE;
  }
  else if (tsName.find('.')!=-1) // maybe a link to a file
  {
    bool ambig = false;
    const FileDef *fd=findFileDef(Doxygen::inputNameLinkedMap,tsName,ambig);
    if (fd && !ambig)
    {
      *resContext=fd;
      AUTO_TRACE_ADD("true");
      return TRUE;
    }
  }

  if (tryUnspecializedVersion)
  {
    bool b = resolveRef(scName,name,inSeeBlock,resContext,resMember,FALSE,nullptr,checkScope);
    AUTO_TRACE_ADD("{}",b);
    return b;
  }
  if (bracePos!=-1) // Try without parameters as well, could be a constructor invocation
  {
    *resContext=getClass(fullName.left(bracePos));
    if (*resContext)
    {
      AUTO_TRACE_ADD("true");
      return TRUE;
    }
  }
  //printf("resolveRef: %s not found!\n",qPrint(name));

  AUTO_TRACE_ADD("false");
  return FALSE;
}

QCString linkToText(SrcLangExt lang,const QCString &link,bool isFileName)
{
  //bool optimizeOutputJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
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
  //printf("linkToText(%s,lang=%d)=%s\n",qPrint(link),lang,qPrint(result));
  return result;
}


bool resolveLink(/* in */ const QCString &scName,
    /* in */ const QCString &lr,
    /* in */ bool /*inSeeBlock*/,
    /* out */ const Definition **resContext,
    /* out */ QCString &resAnchor,
    /* in */ const QCString &prefix
    )
{
  *resContext=nullptr;

  QCString linkRef=lr;
  QCString linkRefWithoutTemplates = stripTemplateSpecifiersFromScope(linkRef,FALSE);
  //printf("ResolveLink linkRef=%s\n",qPrint(lr));
  const FileDef  *fd = nullptr;
  const GroupDef *gd = nullptr;
  const PageDef  *pd = nullptr;
  const ClassDef *cd = nullptr;
  const DirDef   *dir = nullptr;
  const ConceptDef *cnd = nullptr;
  const NamespaceDef *nd = nullptr;
  const SectionInfo *si = nullptr;
  bool ambig = false;
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
  else if ((si=SectionManager::instance().find(prefix+linkRef)))
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
  else if ((cnd=getConcept(linkRef))) // C++20 concept definition
  {
    *resContext=cnd;
    resAnchor=cnd->anchor();
    return TRUE;
  }
  else if ((nd=Doxygen::namespaceLinkedMap->find(linkRef)))
  {
    *resContext=nd;
    return TRUE;
  }
  else if ((dir=Doxygen::dirLinkedMap->find(FileInfo(linkRef.str()).absFilePath()+"/"))
      && dir->isLinkable()) // TODO: make this location independent like filedefs
  {
    *resContext=dir;
    return TRUE;
  }
  else // probably a member reference
  {
    const MemberDef *md = nullptr;
    bool res = resolveRef(scName,lr,TRUE,resContext,&md);
    if (md) resAnchor=md->anchor();
    return res;
  }
}


void generateFileRef(OutputList &ol,const QCString &name,const QCString &text)
{
  //printf("generateFileRef(%s,%s)\n",name,text);
  QCString linkText = text.isEmpty() ? text : name;
  //FileInfo *fi;
  bool ambig = false;
  FileDef *fd = findFileDef(Doxygen::inputNameLinkedMap,name,ambig);
  if (fd && fd->isLinkable())
    // link to documented input file
    ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),QCString(),linkText);
  else
    ol.docify(linkText);
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

FileDef *findFileDef(const FileNameLinkedMap *fnMap,const QCString &n,bool &ambig)
{
  ambig=FALSE;
  if (n.isEmpty()) return nullptr;


  const int maxAddrSize = 20;
  char addr[maxAddrSize];
  qsnprintf(addr,maxAddrSize,"%p:",reinterpret_cast<const void*>(fnMap));
  QCString key = addr;
  key+=n;

  std::lock_guard<std::mutex> lock(g_findFileDefMutex);
  FindFileCacheElem *cachedResult = g_findFileDefCache.find(key.str());
  //printf("key=%s cachedResult=%p\n",qPrint(key),cachedResult);
  if (cachedResult)
  {
    ambig = cachedResult->isAmbig;
    //printf("cached: fileDef=%p\n",cachedResult->fileDef);
    return cachedResult->fileDef;
  }
  else
  {
    cachedResult = g_findFileDefCache.insert(key.str(),FindFileCacheElem(nullptr,FALSE));
  }

  QCString name=Dir::cleanDirPath(n.str());
  QCString path;
  if (name.isEmpty()) return nullptr;
  int slashPos=std::max(name.findRev('/'),name.findRev('\\'));
  if (slashPos!=-1)
  {
    path=removeLongPathMarker(name.left(slashPos+1));
    name=name.right(name.length()-slashPos-1);
  }
  if (name.isEmpty()) return nullptr;
  const FileName *fn = fnMap->find(name);
  if (fn)
  {
    //printf("fn->size()=%zu\n",fn->size());
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
      FileDef *lastMatch=nullptr;
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
  return nullptr;
}

//----------------------------------------------------------------------

QCString findFilePath(const QCString &file,bool &ambig)
{
  ambig=false;
  QCString result;
  bool found=false;
  if (!found)
  {
    FileInfo fi(file.str());
    if (fi.exists())
    {
      result=fi.absFilePath();
      found=true;
    }
  }
  if (!found)
  {
    const StringVector &examplePathList = Config_getList(EXAMPLE_PATH);
    for (const auto &s : examplePathList)
    {
      std::string absFileName = s+(Portable::pathSeparator()+file).str();
      FileInfo fi(absFileName);
      if (fi.exists())
      {
        result=fi.absFilePath();
        found=true;
      }
    }
  }

  if (!found)
  {
    // as a fallback we also look in the exampleNameDict
    FileDef *fd = findFileDef(Doxygen::exampleNameLinkedMap,file,ambig);
    if (fd && !ambig)
    {
      result=fd->absFilePath();
    }
  }
  return result;
}

//----------------------------------------------------------------------

QCString showFileDefMatches(const FileNameLinkedMap *fnMap,const QCString &n)
{
  QCString result;
  QCString name=n;
  QCString path;
  int slashPos=std::max(name.findRev('/'),name.findRev('\\'));
  if (slashPos!=-1)
  {
    path=name.left(slashPos+1);
    name=name.right(name.length()-slashPos-1);
  }
  const FileName *fn=fnMap->find(name);
  if (fn)
  {
    bool first = true;
    for (const auto &fd : *fn)
    {
      if (path.isEmpty() || fd->getPath().right(path.length())==path)
      {
        if (!first) result += "\n";
        else first = false;
        result+="  "+fd->absFilePath();
      }
    }
  }
  return result;
}

//----------------------------------------------------------------------

QCString substituteKeywords(const QCString &s,const KeywordSubstitutionList &keywords)
{
  std::string substRes;
  const char *p = s.data();
  if (p)
  {
    // reserve some room for expansion
    substRes.reserve(s.length()+1024);
    char c = 0;
    while ((c=*p))
    {
      bool found = false;
      if (c=='$')
      {
        for (const auto &kw : keywords)
        {
          size_t keyLen = qstrlen(kw.keyword);
          if (qstrncmp(p,kw.keyword,keyLen)==0)
          {
            const char *startArg = p+keyLen;
            bool expectParam = std::holds_alternative<KeywordSubstitution::GetValueWithParam>(kw.getValueVariant);
            //printf("%s: expectParam=%d *startArg=%c\n",kw.keyword,expectParam,*startArg);
            if (expectParam && *startArg=='(') // $key(value)
            {
              size_t j=1;
              const char *endArg = nullptr;
              while ((c=*(startArg+j)) && c!=')' && c!='\n' && c!=0) j++;
              if (c==')') endArg=startArg+j;
              if (endArg)
              {
                QCString value = QCString(startArg+1).left(endArg-startArg-1);
                auto &&getValue = std::get<KeywordSubstitution::GetValueWithParam>(kw.getValueVariant);
                substRes+=getValue(value).str();
                p=endArg+1;
                //printf("found '%s'->'%s'\n",kw.keyword,qPrint(getValue(value)));
              }
              else
              {
                //printf("missing argument\n");
                p+=keyLen;
              }
            }
            else if (!expectParam) // $key
            {
              auto &&getValue = std::get<KeywordSubstitution::GetValue>(kw.getValueVariant);
              substRes+=getValue().str();
              //printf("found '%s'->'%s'\n",kw.keyword,qPrint(getValue()));
              p+=keyLen;
            }
            found = true;
            break;
          }
        }
      }
      if (!found) // copy
      {
        substRes+=c;
        p++;
      }
    }
  }
  return substRes;
}

static QCString showDate(const QCString &fmt)
{
   // get the current date and time
  std::tm dat{};
  int specFormat=0;
  QCString specDate = "";
  QCString err = dateTimeFromString(specDate,dat,specFormat);

  // do the conversion
  int usedFormat=0;
  return formatDateTime(fmt,dat,usedFormat);
}

QCString projectLogoFile()
{
  QCString projectLogo = Config_getString(PROJECT_LOGO);
  if (!projectLogo.isEmpty())
  {
    // check for optional width= and height= specifier
    int wi = projectLogo.find(" width=");
    if (wi!=-1) // and strip them
    {
      projectLogo = projectLogo.left(wi);
    }
    int hi = projectLogo.find(" height=");
    if (hi!=-1)
    {
      projectLogo = projectLogo.left(hi);
    }
  }
  //printf("projectlogo='%s'\n",qPrint(projectLogo));
  return projectLogo;
}

static QCString projectLogoSize()
{
  QCString sizeVal;
  QCString projectLogo = Config_getString(PROJECT_LOGO);
  if (!projectLogo.isEmpty())
  {
    auto extractDimension = [&projectLogo](const char *startMarker,size_t startPos,size_t endPos) -> QCString
    {
      QCString result = projectLogo.mid(startPos,endPos-startPos).stripWhiteSpace().quoted();
      if (result.length()>=2 && result.at(0)!='"' && result.at(result.length()-1)!='"')
      {
        result="\""+result+"\"";
      }
      result.prepend(startMarker);
      return result;
    };
    // check for optional width= and height= specifier
    int wi = projectLogo.find(" width=");
    int hi = projectLogo.find(" height=");
    if (wi!=-1 && hi!=-1)
    {
      if (wi<hi) // "... width=x height=y..."
      {
        sizeVal = extractDimension(" width=",  wi+7, hi) + " "
                + extractDimension(" height=", hi+8, projectLogo.length());
      }
      else // "... height=y width=x..."
      {
        sizeVal = extractDimension(" height=", hi+8, wi) + " "
                + extractDimension(" width=",  wi+7, projectLogo.length());
      }
    }
    else if (wi!=-1) // ... width=x..."
    {
      sizeVal = extractDimension(" width=", wi+7, projectLogo.length());
    }
    else if (hi!=-1) // ... height=x..."
    {
      sizeVal = extractDimension(" height=", hi+8, projectLogo.length());
    }
  }
  //printf("projectsize='%s'\n",qPrint(sizeVal));
  return sizeVal;
}

QCString substituteKeywords(const QCString &s,const QCString &title,
         const QCString &projName,const QCString &projNum,const QCString &projBrief)
{
  return substituteKeywords(s,
  {
    // keyword          value getter
    { "$title",           [&]() { return !title.isEmpty() ? title : projName;       } },
    { "$datetime",        [&]() { return dateToString(DateTimeType::DateTime);      } },
    { "$date",            [&]() { return dateToString(DateTimeType::Date);          } },
    { "$time",            [&]() { return dateToString(DateTimeType::Time);          } },
    { "$year",            [&]() { return yearToString();                            } },
    { "$doxygenversion",  [&]() { return getDoxygenVersion();                       } },
    { "$projectname",     [&]() { return projName;                                  } },
    { "$projectnumber",   [&]() { return projNum;                                   } },
    { "$projectbrief",    [&]() { return projBrief;                                 } },
    { "$projectlogo",     [&]() { return stripPath(projectLogoFile());              } },
    { "$logosize",        [&]() { return projectLogoSize();                         } },
    { "$projecticon",     [&]() { return stripPath(Config_getString(PROJECT_ICON)); } },
    { "$langISO",         [&]() { return theTranslator->trISOLang();                } },
    { "$showdate",        [&](const QCString &fmt) { return showDate(fmt);          } }
  });
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

//----------------------------------------------------------------------

#if 0
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
#endif

bool getCaseSenseNames()
{
  auto caseSenseNames = Config_getEnum(CASE_SENSE_NAMES);

  if (caseSenseNames == CASE_SENSE_NAMES_t::YES) return true;
  else if (caseSenseNames == CASE_SENSE_NAMES_t::NO) return false;
  else return Portable::fileSystemIsCaseSensitive();
}

// note that this function is not reentrant due to the use of static growBuf!
QCString escapeCharsInString(const QCString &name,bool allowDots,bool allowUnderscore)
{
  if (name.isEmpty()) return name;
  bool caseSenseNames = getCaseSenseNames();
  bool allowUnicodeNames = Config_getBool(ALLOW_UNICODE_NAMES);
  GrowBuf growBuf;
  signed char c = 0;
  const char *p=name.data();
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
      case '"': growBuf.addStr("_0h"); break;
      case '~': growBuf.addStr("_0i"); break;
      case '\'': growBuf.addStr("_0j"); break;
      case ';': growBuf.addStr("_0k"); break;
      case '`': growBuf.addStr("_0l"); break;
      default:
                if (c<0)
                {
                  bool doEscape = true;
                  if (allowUnicodeNames)
                  {
                    int charLen = getUTF8CharNumBytes(c);
                    if (charLen>0)
                    {
                      growBuf.addStr(p-1,charLen);
                      p+=charLen;
                      doEscape = false;
                    }
                  }
                  if (doEscape) // not a valid unicode char or escaping needed
                  {
                    char ids[5];
                    unsigned char id = static_cast<unsigned char>(c);
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
                  growBuf.addChar(static_cast<char>(tolower(c)));
                }
                break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}

QCString unescapeCharsInString(const QCString &s)
{
  if (s.isEmpty()) return s;
  bool caseSenseNames = getCaseSenseNames();
  QCString result;
  const char *p = s.data();
  if (p)
  {
    char c = 0;
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
               case 'h': result+='"'; p+=2; break; // _0h -> '"'
               case 'i': result+='~'; p+=2; break; // _0i -> '~'
               case 'j': result+='\''; p+=2; break;// _0j -> '\'
               case 'k': result+=';'; p+=2; break; // _0k -> ';'
               case 'l': result+='`'; p+=2; break; // _0l -> '`'
               default: // unknown escape, just pass underscore character as-is
                 result+=c;
                 break;
             }
             break;
          default:
            if (!caseSenseNames && c>='a' && c<='z') // lower to upper case escape, _a -> 'A'
            {
              result+=static_cast<char>(toupper(*p));
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
QCString convertNameToFile(const QCString &name,bool allowDots,bool allowUnderscore)
{
  if (name.isEmpty()) return name;
  bool shortNames = Config_getBool(SHORT_NAMES);
  bool createSubdirs = Config_getBool(CREATE_SUBDIRS);
  QCString result;
  if (shortNames) // use short names only
  {
    std::lock_guard<std::mutex> lock(g_usedNamesMutex);
    auto kv = g_usedNames.find(name.str());
    uint32_t num=0;
    if (kv!=g_usedNames.end())
    {
      num = kv->second;
    }
    else
    {
      num = g_usedNamesCount;
      g_usedNames.emplace(name.str(),g_usedNamesCount++);
    }
    result.sprintf("a%05d",num);
  }
  else // long names
  {
    result=escapeCharsInString(name,allowDots,allowUnderscore);
    size_t resultLen = result.length();
    if (resultLen>=128) // prevent names that cannot be created!
    {
      // third algorithm based on MD5 hash
      uint8_t md5_sig[16];
      char sigStr[33];
      MD5Buffer(result.data(),static_cast<unsigned int>(resultLen),md5_sig);
      MD5SigToString(md5_sig,sigStr);
      result=result.left(128-32)+sigStr;
    }
  }
  if (createSubdirs)
  {
    int l1Dir=0,l2Dir=0;
    int createSubdirsLevel = Config_getInt(CREATE_SUBDIRS_LEVEL);
    int createSubdirsBitmaskL2 = (1<<createSubdirsLevel)-1;

    // compute md5 hash to determine sub directory to use
    uint8_t md5_sig[16];
    MD5Buffer(result.data(),static_cast<unsigned int>(result.length()),md5_sig);
    l1Dir = md5_sig[14] & 0xf;
    l2Dir = md5_sig[15] & createSubdirsBitmaskL2;

    result.prepend(QCString().sprintf("d%x/d%02x/",l1Dir,l2Dir));
  }
  //printf("*** convertNameToFile(%s)->%s\n",qPrint(name),qPrint(result));
  return result;
}

QCString generateAnonymousAnchor(const QCString &fileName,int count)
{
  QCString fn = stripFromPath(fileName)+":"+QCString().setNum(count);
  const int sig_size=16;
  uint8_t md5_sig[sig_size];
  MD5Buffer(fn.data(),static_cast<unsigned int>(fn.length()),md5_sig);
  char result[sig_size*3+2];
  char *p = result;
  *p++='@';
  for (int i=0;i<sig_size;i++)
  {
    static const char oct[]="01234567";
    uint8_t byte = md5_sig[i];
    *p++=oct[(byte>>6)&7];
    *p++=oct[(byte>>3)&7];
    *p++=oct[(byte>>0)&7];
  }
  *p='\0';
  return result;
}

QCString relativePathToRoot(const QCString &name)
{
  QCString result;
  if (Config_getBool(CREATE_SUBDIRS))
  {
    if (name.isEmpty())
    {
      return REL_PATH_TO_ROOT;
    }
    else
    {
      int i = name.findRev('/');
      if (i!=-1)
      {
        result=REL_PATH_TO_ROOT;
      }
    }
  }
  return result;
}

QCString determineAbsoluteIncludeName(const QCString &curFile,const QCString &incFileName)
{
  bool searchIncludes = Config_getBool(SEARCH_INCLUDES);
  QCString absIncFileName = incFileName;
  FileInfo fi(curFile.str());
  if (fi.exists())
  {
    QCString absName = QCString(fi.dirPath(TRUE))+"/"+incFileName;
    FileInfo fi2(absName.str());
    if (fi2.exists())
    {
      absIncFileName=fi2.absFilePath();
    }
    else if (searchIncludes) // search in INCLUDE_PATH as well
    {
      const StringVector &includePath = Config_getList(INCLUDE_PATH);
      for (const auto &incPath : includePath)
      {
        FileInfo fi3(incPath);
        if (fi3.exists() && fi3.isDir())
        {
          absName = QCString(fi3.absFilePath())+"/"+incFileName;
          //printf("trying absName=%s\n",qPrint(absName));
          FileInfo fi4(absName.str());
          if (fi4.exists())
          {
            absIncFileName=fi4.absFilePath();
            break;
          }
          //printf( "absIncFileName = %s\n", qPrint(absIncFileName) );
        }
      }
    }
    //printf( "absIncFileName = %s\n", qPrint(absIncFileName) );
  }
  return absIncFileName;
}



void createSubDirs(const Dir &d)
{
  if (Config_getBool(CREATE_SUBDIRS))
  {
    // create up to 4096 subdirectories
    int createSubdirsLevelPow2 = 1 << Config_getInt(CREATE_SUBDIRS_LEVEL);
    for (int l1=0; l1<16; l1++)
    {
      QCString subdir;
      subdir.sprintf("d%x",l1);
      if (!d.exists(subdir.str()) && !d.mkdir(subdir.str()))
      {
        term("Failed to create output directory '%s'\n",qPrint(subdir));
      }
      for (int l2=0; l2<createSubdirsLevelPow2; l2++)
      {
        QCString subsubdir;
        subsubdir.sprintf("d%x/d%02x",l1,l2);
        if (!d.exists(subsubdir.str()) && !d.mkdir(subsubdir.str()))
        {
          term("Failed to create output directory '%s'\n",qPrint(subsubdir));
        }
      }
    }
  }
}

void clearSubDirs(const Dir &d)
{
  if (Config_getBool(CREATE_SUBDIRS))
  {
    // remove empty subdirectories
    int createSubdirsLevelPow2 = 1 << Config_getInt(CREATE_SUBDIRS_LEVEL);
    for (int l1=0;l1<16;l1++)
    {
      QCString subdir;
      subdir.sprintf("d%x",l1);
      for (int l2=0; l2 < createSubdirsLevelPow2; l2++)
      {
        QCString subsubdir;
        subsubdir.sprintf("d%x/d%02x",l1,l2);
        if (d.exists(subsubdir.str()) && d.isEmpty(subsubdir.str()))
        {
          d.rmdir(subsubdir.str());
        }
      }
      if (d.exists(subdir.str()) && d.isEmpty(subdir.str()))
      {
        d.rmdir(subdir.str());
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
  int i=0, p=0;
  QCString clName=scopeName;
  NamespaceDef *nd = nullptr;
  if (!clName.isEmpty() && (nd=getResolvedNamespace(clName)) && getClass(clName)==nullptr)
  { // the whole name is a namespace (and not a class)
    namespaceName=nd->name();
    className.clear();
    goto done;
  }
  p=static_cast<int>(clName.length())-2;
  while (p>=0 && (i=clName.findRev("::",p))!=-1)
    // see if the first part is a namespace (and not a class)
  {
    //printf("Trying %s\n",qPrint(clName.left(i)));
    if (i>0 && (nd=getResolvedNamespace(clName.left(i))) && getClass(clName.left(i))==nullptr)
    {
      //printf("found!\n");
      namespaceName=nd->name();
      className=clName.right(clName.length()-i-2);
      goto done;
    }
    p=i-2; // try a smaller piece of the scope
  }
  //printf("not found!\n");

  // not found, so we just have to guess.
  className=scopeName;
  namespaceName.clear();

done:
  if (className.isEmpty() && !namespaceName.isEmpty() && !allowEmptyClass)
  {
    // class and namespace with the same name, correct to return the class.
    className=namespaceName;
    namespaceName.clear();
  }
  //printf("extractNamespace '%s' => '%s|%s'\n",qPrint(scopeName),
  //       qPrint(className),qPrint(namespaceName));
  if (className.endsWith("-p"))
  {
    className = className.left(className.length()-2);
  }
  return;
}

QCString insertTemplateSpecifierInScope(const QCString &scope,const QCString &templ)
{
  QCString result=scope;
  if (!templ.isEmpty() && scope.find('<')==-1)
  {
    int si=0, pi=0;
    ClassDef *cd=nullptr;
    while (
        (si=scope.find("::",pi))!=-1 && !getClass(scope.left(si)+templ) &&
        ((cd=getClass(scope.left(si)))==nullptr || cd->templateArguments().empty())
        )
    {
      //printf("Tried '%s'\n",qPrint((scope.left(si)+templ)));
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
  //    qPrint(scope),qPrint(templ),qPrint(result));
  return result;
}


/*! Strips the scope from a name. Examples: A::B will return A
 *  and A<T>::B<N::C<D> > will return A<T>.
 */
QCString stripScope(const QCString &name)
{
  QCString result = name;
  int l = static_cast<int>(result.length());
  int p = 0;
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
          //printf("stripScope(%s)=%s\n",name,qPrint(result.right(l-p-1)));
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
QCString convertToId(const QCString &s)
{
  if (s.isEmpty()) return s;
  GrowBuf growBuf;
  const char *p = s.data();
  char c        = 0;
  bool first    = true;
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
      encChar[1]=hex[static_cast<unsigned char>(c)>>4];
      encChar[2]=hex[static_cast<unsigned char>(c)&0xF];
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
QCString correctId(const QCString &s)
{
  if (s.isEmpty()) return s;
  return "a" + s;
}

/*! Converts a string to an XML-encoded string */
QCString convertToXML(const QCString &s, bool keepEntities)
{
  if (s.isEmpty()) return s;
  GrowBuf growBuf;
  const char *p = s.data();
  char c = 0;
  while ((c=*p++))
  {
    switch (c)
    {
      case '<':  growBuf.addStr("&lt;");   break;
      case '>':  growBuf.addStr("&gt;");   break;
      case '&':  if (keepEntities)
                 {
                   const char *e=p;
                   char ce = 0;
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

/*! Converts a string to a HTML-encoded string */
QCString convertToHtml(const QCString &s,bool keepEntities)
{
  if (s.isEmpty()) return s;
  GrowBuf growBuf;
  const char *p=s.data();
  char c = 0;
  while ((c=*p++))
  {
    switch (c)
    {
      case '<':  growBuf.addStr("&lt;");   break;
      case '>':  growBuf.addStr("&gt;");   break;
      case '&':  if (keepEntities)
                 {
                   const char *e=p;
                   char ce = 0;
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
          uint8_t uc = static_cast<uint8_t>(c);
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

QCString convertToJSString(const QCString &s)
{
  if (s.isEmpty()) return s;
  GrowBuf growBuf;
  const char *p=s.data();
  char c = 0;
  while ((c=*p++))
  {
    switch (c)
    {
      case '"':  growBuf.addStr("\\\""); break;
      case '\\': if (*p=='u' && *(p+1)=='{') growBuf.addStr("\\");
                 else growBuf.addStr("\\\\");
                 break;
      default:   growBuf.addChar(c);   break;
    }
  }
  growBuf.addChar(0);
  return convertCharEntitiesToUTF8(growBuf.get());
}

QCString convertCharEntitiesToUTF8(const QCString &str)
{
  if (str.isEmpty()) return QCString();

  std::string s = str.data();
  static const reg::Ex re(R"(&\a\w*;)");
  reg::Iterator it(s,re);
  reg::Iterator end;

  GrowBuf growBuf;
  size_t p=0, i=0, l=0;
  for (; it!=end ; ++it)
  {
    const auto &match = *it;
    p = match.position();
    l = match.length();
    if (p>i)
    {
      growBuf.addStr(s.substr(i,p-i));
    }
    QCString entity(match.str());
    HtmlEntityMapper::SymType symType = HtmlEntityMapper::instance().name2sym(entity);
    const char *code=nullptr;
    if (symType!=HtmlEntityMapper::Sym_Unknown && (code=HtmlEntityMapper::instance().utf8(symType)))
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
  //printf("convertCharEntitiesToUTF8(%s)->%s\n",qPrint(s),growBuf.get());
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
  ASSERT(context!=nullptr);
  //printf("addMemberToMemberGroup() context=%s\n",qPrint(context->name()));
  if (ml==nullptr) return;

  struct MoveMemberInfo
  {
    MoveMemberInfo(MemberDef *md,MemberGroup *mg,const RefItemVector &rv)
      : memberDef(md), memberGroup(mg), sli(rv) {}
    MemberDef *memberDef;
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
            const auto &info = it->second;
            auto mg_it = std::find_if(pMemberGroups->begin(),
                                      pMemberGroups->end(),
                                      [&groupId](const auto &g)
                                      { return g->groupId()==groupId; }
                                     );
            MemberGroup *mg_ptr = nullptr;
            if (mg_it==pMemberGroups->end())
            {
              auto mg = std::make_unique<MemberGroup>(
                        context,
                        groupId,
                        info->header,
                        info->doc,
                        info->docFile,
                        info->docLine,
                        ml->container());
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
        const auto &info = it->second;
        auto mg_it = std::find_if(pMemberGroups->begin(),
                                  pMemberGroups->end(),
                                  [&groupId](const auto &g)
                                  { return g->groupId()==groupId; }
                                 );
        MemberGroup *mg_ptr = nullptr;
        if (mg_it==pMemberGroups->end())
        {
          auto mg = std::make_unique<MemberGroup>(
                    context,
                    groupId,
                    info->header,
                    info->doc,
                    info->docFile,
                    info->docLine,
                    ml->container());
          mg_ptr = mg.get();
          pMemberGroups->push_back(std::move(mg));
        }
        else
        {
          mg_ptr = (*mg_it).get();
        }
        movedMembers.emplace_back(md,mg_ptr,info->m_sli);
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
int extractClassNameFromType(const QCString &type,int &pos,QCString &name,QCString &templSpec,SrcLangExt lang)
{
  static const reg::Ex re_norm(R"(\a[\w:]*)");
  static const reg::Ex re_fortran(R"(\a[\w:()=]*)");
  const reg::Ex *re = &re_norm;

  name.clear();
  templSpec.clear();
  if (type.isEmpty()) return -1;
  size_t typeLen=type.length();
  if (typeLen>0)
  {
    if (lang == SrcLangExt::Fortran)
    {
      if (type[pos]==',') return -1;
      if (!type.lower().startsWith("type"))
      {
        re = &re_fortran;
      }
    }
    std::string s = type.str();
    reg::Iterator it(s,*re,static_cast<int>(pos));
    reg::Iterator end;

    if (it!=end)
    {
      const auto &match = *it;
      size_t i = match.position();
      size_t l = match.length();
      size_t ts = i+l;
      size_t te = ts;
      size_t tl = 0;

      while (ts<typeLen && type[static_cast<uint32_t>(ts)]==' ') ts++,tl++; // skip any whitespace
      if (ts<typeLen && type[static_cast<uint32_t>(ts)]=='<') // assume template instance
      {
        // locate end of template
        te=ts+1;
        int brCount=1;
        while (te<typeLen && brCount!=0)
        {
          if (type[static_cast<uint32_t>(te)]=='<')
          {
            if (te<typeLen-1 && type[static_cast<uint32_t>(te)+1]=='<') te++; else brCount++;
          }
          if (type[static_cast<uint32_t>(te)]=='>')
          {
            if (te<typeLen-1 && type[static_cast<uint32_t>(te)+1]=='>') te++; else brCount--;
          }
          te++;
        }
      }
      name = match.str();
      if (te>ts)
      {
        templSpec = QCString(type).mid(ts,te-ts);
        tl+=te-ts;
        pos=static_cast<int>(i+l+tl);
      }
      else // no template part
      {
        pos=static_cast<int>(i+l);
      }
      //printf("extractClassNameFromType([in] type=%s,[out] pos=%d,[out] name=%s,[out] templ=%s)=TRUE i=%d\n",
      //    qPrint(type),pos,qPrint(name),qPrint(templSpec),i);
      return static_cast<int>(i);
    }
  }
  pos = static_cast<int>(typeLen);
  //printf("extractClassNameFromType([in] type=%s,[out] pos=%d,[out] name=%s,[out] templ=%s)=FALSE\n",
  //       qPrint(type),pos,qPrint(name),qPrint(templSpec));
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

  std::string s = name.mid(p).str();
  static const reg::Ex re(R"([\a:][\w:]*)");
  reg::Iterator it(s,re);
  reg::Iterator end;
  size_t pi=0;
  // for each identifier in the template part (e.g. B<T> -> T)
  for (; it!=end ; ++it)
  {
    const auto &match = *it;
    size_t i = match.position();
    size_t l = match.length();
    result += s.substr(pi,i-pi);
    QCString n(match.str());
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
      const ClassDef *cd = resolver.resolveClass(context,n);
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
  //printf("normalizeNonTemplateArgumentInString(%s)=%s\n",qPrint(name),qPrint(result));
  return removeRedundantWhiteSpace(result);
}


/*! Substitutes any occurrence of a formal argument from argument list
 *  \a formalArgs in \a name by the corresponding actual argument in
 *  argument list \a actualArgs. The result after substitution
 *  is returned as a string. The argument \a name is used to
 *  prevent recursive substitution.
 */
QCString substituteTemplateArgumentsInString(
    const QCString &nm,
    const ArgumentList &formalArgs,
    const ArgumentList *actualArgs)
{
  AUTO_TRACE("name={} formalArgs={} actualArgs={}",nm,argListToString(formalArgs),actualArgs ? argListToString(*actualArgs) : QCString());
  if (formalArgs.empty()) return nm;
  QCString result;

  static const reg::Ex re(R"(\a\w*)");
  std::string name = nm.str();
  reg::Iterator it(name,re);
  reg::Iterator end;
  size_t p=0;

  for (; it!=end ; ++it)
  {
    const auto &match = *it;
    size_t i = match.position();
    size_t l = match.length();
    if (i>p) result += name.substr(p,i-p);
    QCString n(match.str());
    ArgumentList::const_iterator actIt;
    if (actualArgs)
    {
      actIt = actualArgs->begin();
    }
    //printf(": name=%s\n",qPrint(name));

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
      if (formArg.type.startsWith("class ") && formArg.name.isEmpty())
      {
        formArg.name = formArg.type.mid(6);
        formArg.type = "class";
      }
      else if (formArg.type.startsWith("typename ") && formArg.name.isEmpty())
      {
        formArg.name = formArg.type.mid(9);
        formArg.type = "typename";
      }
      else if (formArg.type.startsWith("class...")) // match 'class... name' to 'name...'
      {
        formArg.name += "...";
        formArg.type = formArg.type.left(5)+formArg.type.mid(8);
      }
      else if (formArg.type.startsWith("typename...")) // match 'typename... name' to 'name...'
      {
        formArg.name += "...";
        formArg.type = formArg.type.left(8)+formArg.type.mid(11);
      }
      //printf(": n=%s formArg->type='%s' formArg->name='%s' formArg->defval='%s' actArg->type='%s' actArg->name='%s' \n",
      //  qPrint(n),qPrint(formArg.type),qPrint(formArg.name),qPrint(formArg.defval),qPrint(actArg.type),qPrint(actArg.name));
      if (formArg.type=="class" || formArg.type=="typename" || formArg.type.startsWith("template"))
      {
        if (formArg.name==n && actualArgs && actIt!=actualArgs->end() && !actArg.type.isEmpty()) // base class is a template argument
        {
          static constexpr auto hasRecursion = [](const QCString &prefix,const QCString &nameArg,const QCString &subst) -> bool
          {
            int ii=0;
            int pp=0;

            ii = subst.find('<');
            //printf("prefix='%s' subst='%s'\n",qPrint(prefix.mid(prefix.length()-ii-2,ii+1)),qPrint(subst.left(ii+1)));
            if (ii!=-1 && static_cast<int>(prefix.length())>=ii+2 && prefix.mid(prefix.length()-ii-2,ii+1)==subst.left(ii+1))
            {
              return true; // don't replace 'A< ' with 'A< A<...', see issue #10951
            }

            while ((ii=subst.find(nameArg,pp))!=-1)
            {
              bool beforeNonWord = ii==0 || !isId(subst.at(ii-1));
              bool afterNonWord  = subst.length()==ii+nameArg.length() || !isId(subst.at(ii+nameArg.length()));
              if (beforeNonWord && afterNonWord)
              {
                return true; // if nameArg=='A' then subst=='A::Z' or 'S<A>' or 'Z::A' should return true, but 'AA::ZZ' or 'BAH' should not match
              }
              pp=ii+static_cast<int>(nameArg.length());
            }
            return false;
          };
          // replace formal argument with the actual argument of the instance
          AUTO_TRACE_ADD("result={} n={} type={} hasRecursion={}",result,n,actArg.type,hasRecursion(result,n,actArg.type));
          if (!hasRecursion(result,n,actArg.type))
            // the scope guard is to prevent recursive lockup for
            // template<class A> class C : public<A::T>,
            // where A::T would become A::T::T here,
            // since n==A and actArg->type==A::T
            // see bug595833 for an example
            //
            // Also prevent recursive substitution if n is part of actArg.type, i.e.
            // n='A' in argType='S< A >' would produce 'S< S< A > >'
          {
            if (actArg.name.isEmpty())
            {
              result += actArg.type;
            }
            else
              // for case where the actual arg is something like "unsigned int"
              // the "int" part is in actArg->name.
            {
              result += actArg.type+" "+actArg.name;
            }
            found=TRUE;
          }
        }
        else if (formArg.name==n &&
                 (actualArgs==nullptr || actIt==actualArgs->end()) &&
                 !formArg.defval.isEmpty() &&
                 formArg.defval!=nm /* to prevent recursion */
            )
        {
          result += substituteTemplateArgumentsInString(formArg.defval,formalArgs,actualArgs);
          found=TRUE;
        }
      }
      else if (formArg.name==n &&
               (actualArgs==nullptr || actIt==actualArgs->end()) &&
               !formArg.defval.isEmpty() &&
               formArg.defval!=nm /* to prevent recursion */
              )
      {
        result += substituteTemplateArgumentsInString(formArg.defval,formalArgs,actualArgs);
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
  result=result.simplifyWhiteSpace();
  AUTO_TRACE_EXIT("result={}",result);
  return result.stripWhiteSpace();
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
    QCString *pLastScopeStripped,
    QCString scopeName,
    bool allowArtificial)
{
  //printf("stripTemplateSpecifiersFromScope(name=%s,scopeName=%s)\n",qPrint(fullName),qPrint(scopeName));
  int i=fullName.find('<');
  if (i==-1) return fullName;
  QCString result;
  int p=0;
  int l=static_cast<int>(fullName.length());
  while (i!=-1)
  {
    //printf("1:result+=%s\n",qPrint(fullName.mid(p,i-p)));
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
    //printf("  trying %s\n",qPrint(mergeScopes(scopeName,result+fullName.mid(i,e-i))));
    ClassDef *cd = getClass(mergeScopes(scopeName,result+fullName.mid(i,e-i)));
    if (cd!=nullptr && (allowArtificial || !cd->isArtificial()))
    {
      result+=fullName.mid(i,e-i);
      //printf("  2:result+=%s\n",qPrint(fullName.mid(i,e-i-1)));
    }
    else if (pLastScopeStripped)
    {
      //printf("  last stripped scope '%s'\n",qPrint(fullName.mid(i,e-i)));
      *pLastScopeStripped=fullName.mid(i,e-i);
    }
    p=e;
    i=fullName.find('<',p);
  }
  result+=fullName.right(l-p);
  //printf("3:result+=%s\n",qPrint(fullName.right(l-p)));
  //printf("end result=%s\n",qPrint(result));
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
  int i=0,p=static_cast<int>(leftScope.length());

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
  result=leftScope;
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
  int sl=static_cast<int>(s.length());
  int sp=p;
  int count=0;
  bool done=false;
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
        done=false;
        while (sp<sl && !done)
        {
          // TODO: deal with << and >> operators!
          c=s.at(sp++);
          switch(c)
          {
            case '<': count++; break;
            case '>': count--; if (count==0) done=true; break;
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
  //printf("getScopeFragment(%s,%d)=%s\n",qPrint(s),p,qPrint(s.mid(p,*l)));
  return p;
}

//----------------------------------------------------------------------------

PageDef *addRelatedPage(const QCString &name,const QCString &ptitle,
    const QCString &doc,
    const QCString &fileName,
    int docLine,
    int startLine,
    const RefItemVector &sli,
    GroupDef *gd,
    const TagInfo *tagInfo,
    bool xref,
    SrcLangExt lang
    )
{
  PageDef *pd=nullptr;
  //printf("addRelatedPage(name=%s gd=%p)\n",qPrint(name),gd);
  QCString title=ptitle.stripWhiteSpace();
  bool newPage = true;
  if ((pd=Doxygen::pageLinkedMap->find(name)) && !pd->isReference())
  {
    if (!xref && !title.isEmpty() && pd->title()!=pd->name() && pd->title()!=title)
    {
      warn(fileName,startLine,"multiple use of page label '%s' with different titles, (other occurrence: %s, line: %d)",
         qPrint(name),qPrint(pd->docFile()),pd->getStartBodyLine());
    }
    if (!title.isEmpty() && pd->title()==pd->name()) // pd has no real title yet
    {
      pd->setTitle(title);
      SectionInfo *si = SectionManager::instance().find(pd->name());
      if (si)
      {
        si->setTitle(title);
      }
    }
    // append documentation block to the page.
    pd->setDocumentation(doc,fileName,docLine);
    //printf("Adding page docs '%s' pi=%p name=%s\n",qPrint(doc),pd,name);
    // append (x)refitems to the page.
    pd->setRefItems(sli);
    newPage = false;
  }

  if (newPage) // new page
  {
    QCString baseName=name;
    if (baseName.endsWith(".tex"))
      baseName=baseName.left(baseName.length()-4);
    else if (baseName.right(Doxygen::htmlFileExtension.length())==Doxygen::htmlFileExtension)
      baseName=baseName.left(baseName.length()-Doxygen::htmlFileExtension.length());

    //printf("Appending page '%s'\n",qPrint(baseName));
    if (pd) // replace existing page
    {
      pd->setDocumentation(doc,fileName,docLine);
      pd->setFileName(::convertNameToFile(baseName,FALSE,TRUE));
      pd->setShowLineNo(FALSE);
      pd->setNestingLevel(0);
      pd->setPageScope(nullptr);
      pd->setTitle(title);
      pd->setReference(QCString());
    }
    else // newPage
    {
      pd = Doxygen::pageLinkedMap->add(baseName,
             createPageDef(fileName,docLine,baseName,doc,title));
    }
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
        if (!si->ref().isEmpty()) // we are from a tag file
        {
          SectionManager::instance().replace(pd->name(),
              file,-1,pd->title(),SectionType::Page,0,pd->getReference());
        }
        else if (si->lineNr() != -1)
        {
          warn(orgFile,line,"multiple use of section label '%s', (first occurrence: %s, line %d)",qPrint(pd->name()),qPrint(si->fileName()),si->lineNr());
        }
        else
        {
          warn(orgFile,line,"multiple use of section label '%s', (first occurrence: %s)",qPrint(pd->name()),qPrint(si->fileName()));
        }
      }
      else
      {
        SectionManager::instance().add(pd->name(),
            file,-1,pd->title(),SectionType::Page,0,pd->getReference());
        //printf("si->label='%s' si->definition=%s si->fileName='%s'\n",
        //      qPrint(si->label),si->definition?si->definition->name().data():"<none>",
        //      qPrint(si->fileName));
        //printf("  SectionInfo: sec=%p sec->fileName=%s\n",si,qPrint(si->fileName));
        //printf("Adding section key=%s si->fileName=%s\n",qPrint(pageName),qPrint(si->fileName));
      }
    }
  }
  return pd;
}

//----------------------------------------------------------------------------

void addRefItem(const RefItemVector &sli,
    const QCString &key, const QCString &prefix, const QCString &name,
    const QCString &title, const QCString &args, const Definition *scope)
{
  //printf("addRefItem(sli=%d,key=%s,prefix=%s,name=%s,title=%s,args=%s)\n",(int)sli.size(),key,prefix,name,title,args);
  if (!key.isEmpty() && key[0]!='@') // check for @ to skip anonymous stuff (see bug427012)
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

static ModuleDef *findModuleDef(const Definition *d)
{
  ModuleDef *mod = nullptr;
  if (d->definitionType()==Definition::TypeFile)
  {
    const FileDef *fd = toFileDef(d);
    if (fd) mod = fd->getModuleDef();
  }
  else if (d->definitionType()==Definition::TypeClass)
  {
    const ClassDef *cd = toClassDef(d);
    if (cd)
    {
      const FileDef *fd = cd->getFileDef();
      if (fd) mod = fd->getModuleDef();
    }
  }
  else if (d->definitionType()==Definition::TypeConcept)
  {
    const ConceptDef *cd = toConceptDef(d);
    if (cd)
    {
      const FileDef *fd = cd->getFileDef();
      if (fd) mod = fd->getModuleDef();
    }
  }
  return mod;
}

static bool recursivelyAddGroupListToTitle(OutputList &ol,const Definition *d,bool root)
{
  ModuleDef *mod = root ? findModuleDef(d) : nullptr;
  if (!d->partOfGroups().empty() || mod!=nullptr) // write list of group to which this definition belongs
  {
    if (root)
    {
      ol.pushGeneratorState();
      ol.disableAllBut(OutputType::Html);
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
      ol.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),QCString(),gd->groupTitle());
    }
    if (root)
    {
      // add module as a group to the file as well
      if (mod)
      {
        if (!first) { ol.writeString(" &#124; "); } else first=false;
        ol.writeString(theTranslator->trModule(false,true)+" ");
        ol.writeObjectLink(mod->getReference(),mod->getOutputFileBase(),QCString(),
                           mod->displayName());
      }
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

bool checkExtension(const QCString &fName, const QCString &ext)
{
  return fName.right(ext.length())==ext;
}

void addHtmlExtensionIfMissing(QCString &fName)
{
  if (fName.isEmpty()) return;
  int i_fs = fName.findRev('/');
  int i_bs = fName.findRev('\\');
  int i    = fName.find('.',std::max({ i_fs, i_bs ,0})); // search for . after path part
  if (i==-1)
  {
    fName+=Doxygen::htmlFileExtension;
  }
}

QCString stripExtensionGeneral(const QCString &fName, const QCString &ext)
{
  QCString result=fName;
  if (result.right(ext.length())==ext)
  {
    result=result.left(result.length()-ext.length());
  }
  return result;
}

QCString stripExtension(const QCString &fName)
{
  return stripExtensionGeneral(fName, Doxygen::htmlFileExtension);
}

#if 0
void replaceNamespaceAliases(QCString &scope,size_t i)
{
  printf("replaceNamespaceAliases(%s,%zu)\n",qPrint(scope),i);
  while (i>0)
  {
    QCString ns = scope.left(i);
    if (!ns.isEmpty())
    {
      auto it = Doxygen::namespaceAliasMap.find(ns.str());
      if (it!=Doxygen::namespaceAliasMap.end())
      {
        scope=QCString(it->second)+scope.right(scope.length()-i);
        i=it->second.length();
      }
    }
    if (i>0 && ns==scope.left(i)) break;
  }
  printf("result=%s\n",qPrint(scope));
}
#endif

QCString stripPath(const QCString &s)
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
bool containsWord(const QCString &str,const char *word)
{
  if (str.isEmpty() || word==nullptr) return false;
  static const reg::Ex re(R"(\a+)");
  std::string s = str.str();
  for (reg::Iterator it(s,re) ; it!=reg::Iterator() ; ++it)
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
  static reg::Ex re(R"(\s*(\<\a+\>)\s*)");
  std::string s = sentence.str();
  reg::Iterator it(s,re);
  reg::Iterator end;
  std::string result;
  bool found=false;
  size_t p=0;
  for ( ; it!=end ; ++it)
  {
    const auto match = *it;
    std::string part = match[1].str();
    if (part!=word)
    {
      size_t i = match.position();
      size_t l = match.length();
      result+=s.substr(p,i-p);
      result+=match.str();
      p=i+l;
    }
    else
    {
      found=true;
      size_t i = match[1].position();
      size_t l = match[1].length();
      result+=s.substr(p,i-p);
      p=i+l;
    }
  }
  result+=s.substr(p);
  sentence = QCString(result).simplifyWhiteSpace();
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
  if (s.isEmpty()) return QCString();
  const char *p = s.data();

  // search for leading empty lines
  int i=0,li=-1,l=static_cast<int>(s.length());
  char c = 0;
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
  if (bi<=li) return QCString(); // only empty lines
  //printf("docLine='%s' len=%d li=%d bi=%d\n",qPrint(s),s.length(),li,bi);
  return s.mid(li,bi-li);
}

//--------------------------------------------------------------------------

static std::unordered_map<std::string,SrcLangExt> g_extLookup;

struct Lang2ExtMap
{
  const char *langName;
  const char *parserName;
  SrcLangExt parserId;
  const char *defExt;
};

static std::vector<Lang2ExtMap> g_lang2extMap =
{
//  language       parser           parser option
  { "idl",         "c",             SrcLangExt::IDL,      ".idl" },
  { "java",        "c",             SrcLangExt::Java,     ".java"},
  { "javascript",  "c",             SrcLangExt::JS,       ".js"  },
  { "csharp",      "c",             SrcLangExt::CSharp,   ".cs"  },
  { "d",           "c",             SrcLangExt::D,        ".d"   },
  { "php",         "c",             SrcLangExt::PHP,      ".php" },
  { "objective-c", "c",             SrcLangExt::ObjC,     ".m"   },
  { "c",           "c",             SrcLangExt::Cpp,      ".c"   },
  { "c++",         "c",             SrcLangExt::Cpp,      ".cpp" },
  { "slice",       "c",             SrcLangExt::Slice,    ".ice" },
  { "python",      "python",        SrcLangExt::Python,   ".py"  },
  { "fortran",     "fortran",       SrcLangExt::Fortran,  ".f"   },
  { "fortranfree", "fortranfree",   SrcLangExt::Fortran,  ".f90" },
  { "fortranfixed", "fortranfixed", SrcLangExt::Fortran,  ".f"   },
  { "vhdl",        "vhdl",          SrcLangExt::VHDL,     ".vhdl"},
  { "xml",         "xml",           SrcLangExt::XML,      ".xml" },
  { "sql",         "sql",           SrcLangExt::SQL,      ".sql" },
  { "md",          "md",            SrcLangExt::Markdown, ".md"  },
  { "lex",         "lex",           SrcLangExt::Lex,      ".l"   },
};

bool updateLanguageMapping(const QCString &extension,const QCString &language)
{
  QCString langName = language.lower();
  auto it1 = std::find_if(g_lang2extMap.begin(),g_lang2extMap.end(),
                        [&langName](const auto &info) { return info.langName==langName; });
  if (it1 == g_lang2extMap.end()) return false;

  // found the language
  SrcLangExt parserId = it1->parserId;
  QCString extName = extension.lower();
  if (extName.isEmpty()) return FALSE;
  if (extName.at(0)!='.') extName.prepend(".");
  auto it2 = g_extLookup.find(extName.str());
  if (it2!=g_extLookup.end())
  {
    g_extLookup.erase(it2); // language was already register for this ext
  }
  //printf("registering extension %s\n",qPrint(extName));
  g_extLookup.emplace(extName.str(),parserId);
  if (!Doxygen::parserManager->registerExtension(extName,it1->parserName))
  {
    err("Failed to assign extension %s to parser %s for language %s\n",
        extName.data(),it1->parserName,qPrint(language));
  }
  else
  {
    //msg("Registered extension %s to language parser %s...\n",
    //    extName.data(),qPrint(language));
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
  updateLanguageMapping(".cxxm",     "c"); // C++20 modules
  updateLanguageMapping(".cppm",     "c"); // C++20 modules
  updateLanguageMapping(".ccm",      "c"); // C++20 modules
  updateLanguageMapping(".c++m",     "c"); // C++20 modules
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
  updateLanguageMapping(".l",        "lex");
  updateLanguageMapping(".doxygen_lex_c", "c"); // this is a placeholder so we can map initializations
                                                // in the lex scanning to cpp
}

void addCodeOnlyMappings()
{
  updateLanguageMapping(".xml",   "xml");
  updateLanguageMapping(".sql",   "sql");
}

SrcLangExt getLanguageFromFileName(const QCString& fileName, SrcLangExt defLang)
{
  FileInfo fi(fileName.str());
  // we need only the part after the last ".", newer implementations of FileInfo have 'suffix()' for this.
  QCString extName = QCString(fi.extension(FALSE)).lower();
  if (extName.isEmpty()) extName=".no_extension";
  if (extName.at(0)!='.') extName.prepend(".");
  auto it = g_extLookup.find(extName.str());
  if (it!=g_extLookup.end()) // listed extension
  {
    //printf("getLanguageFromFileName(%s)=%x\n",qPrint(fi.extension()),*pVal);
    return it->second;
  }
  //printf("getLanguageFromFileName(%s) not found!\n",qPrint(fileName));
  return defLang; // not listed => assume C-ish language.
}

/// Routine to handle the language attribute of the `\code` command
SrcLangExt getLanguageFromCodeLang(QCString &fileName)
{
  // try the extension
  auto lang = getLanguageFromFileName(fileName, SrcLangExt::Unknown);
  if (lang == SrcLangExt::Unknown)
  {
    // try the language names
    QCString langName = fileName.lower();
    if (langName.at(0)=='.') langName = langName.mid(1);
    auto it = std::find_if(g_lang2extMap.begin(),g_lang2extMap.end(),
                        [&langName](const auto &info) { return info.langName==langName; });
    if (it != g_lang2extMap.end())
    {
      lang     = it->parserId;
      fileName = it->defExt;
    }
    else // default to C++
    {
      return SrcLangExt::Cpp;
    }
  }
  return lang;
}

QCString getFileNameExtension(const QCString &fn)
{
  if (fn.isEmpty()) return "";
  int lastDot = fn.findRev('.');
  if (lastDot!=-1) return fn.mid(lastDot);
  return "";
}

//--------------------------------------------------------------------------

static MemberDef *getMemberFromSymbol(const Definition *scope,const FileDef *fileScope,
                                const QCString &n)
{
  if (scope==nullptr ||
      (scope->definitionType()!=Definition::TypeClass &&
       scope->definitionType()!=Definition::TypeNamespace
      )
     )
  {
    scope=Doxygen::globalScope;
  }

  QCString name = n;
  if (name.isEmpty())
    return nullptr; // no name was given

  auto &range = Doxygen::symbolMap->find(name);
  if (range.empty())
    return nullptr; // could not find any matching symbols

  // mostly copied from getResolvedClassRec()
  QCString explicitScopePart;
  int qualifierIndex = computeQualifiedIndex(name);
  if (qualifierIndex!=-1)
  {
    explicitScopePart = name.left(qualifierIndex);
    replaceNamespaceAliases(explicitScopePart);
    name = name.mid(qualifierIndex+2);
  }
  //printf("explicitScopePart=%s\n",qPrint(explicitScopePart));

  int minDistance = 10000;
  MemberDef *bestMatch = nullptr;

  for (Definition *d : range)
  {
    if (d->definitionType()==Definition::TypeMember)
    {
      SymbolResolver resolver(fileScope);
      int distance = resolver.isAccessibleFromWithExpScope(scope,d,explicitScopePart);
      if (distance!=-1 && distance<minDistance)
      {
        minDistance = distance;
        bestMatch = toMemberDef(d);
        //printf("new best match %s distance=%d\n",qPrint(bestMatch->qualifiedName()),distance);
      }
    }
  }
  return bestMatch;
}

/*! Returns true iff the given name string appears to be a typedef in scope. */
bool checkIfTypedef(const Definition *scope,const FileDef *fileScope,const QCString &n)
{
  MemberDef *bestMatch = getMemberFromSymbol(scope,fileScope,n);

  if (bestMatch && bestMatch->isTypedef())
    return TRUE; // closest matching symbol is a typedef
  else
    return FALSE;
}

static int nextUTF8CharPosition(const QCString &utf8Str,uint32_t len,uint32_t startPos)
{
  if (startPos>=len) return len;
  uint8_t c = static_cast<uint8_t>(utf8Str[startPos]);
  int bytes=getUTF8CharNumBytes(c);
  if (c=='&') // skip over character entities
  {
    bytes=1;
    int (*matcher)(int) = nullptr;
    c = static_cast<uint8_t>(utf8Str[startPos+bytes]);
    if (c=='#') // numerical entity?
    {
      bytes++;
      c = static_cast<uint8_t>(utf8Str[startPos+bytes]);
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
      while ((c = static_cast<uint8_t>(utf8Str[startPos+bytes]))!=0 && matcher(c))
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
  if (doc.isEmpty()) return "";
  //printf("parseCommentAsText(%s)\n",qPrint(doc));
  TextStream t;
  auto parser { createDocParser() };
  auto ast    { validatingParseDoc(*parser.get(),
                                   fileName,lineNr,
                                   scope,md,doc,FALSE,FALSE,
                                   QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT)) };
  auto astImpl = dynamic_cast<const DocNodeAST*>(ast.get());
  if (astImpl)
  {
    TextDocVisitor visitor(t);
    std::visit(visitor,astImpl->root);
  }
  QCString result = convertCharEntitiesToUTF8(t.str().c_str()).stripWhiteSpace();
  int i=0;
  int charCnt=0;
  int l=static_cast<int>(result.length());
  while ((i=nextUTF8CharPosition(result,l,i))<l)
  {
    charCnt++;
    if (charCnt>=80) break;
  }
  if (charCnt>=80) // try to truncate the string
  {
    while ((i=nextUTF8CharPosition(result,l,i))<l && charCnt<100)
    {
      charCnt++;
      if (result.at(i)==',' ||
          result.at(i)=='.' ||
          result.at(i)=='!' ||
          result.at(i)=='?' ||
          result.at(i)=='}')    // good for UTF-16 characters and } otherwise also a good point to stop the string
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
    linkifyText(TextGeneratorOLImpl(ol),d,nullptr,nullptr,a.type);
    ol.endConstraintType();
    ol.startConstraintDocs();
    ol.generateDoc(d->docFile(),d->docLine(),d,nullptr,a.docs,TRUE,FALSE,
                   QCString(),FALSE,FALSE,Config_getBool(MARKDOWN_SUPPORT));
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
  const size_t cmdLen = 40960;
  static char cmd[cmdLen];
  char *p = cmd;
  p += qsnprintf(p,cmdLen,"/usr/bin/atos -p %d ", (int)getpid());
  for (int x = 0; x < frameCount; x++)
  {
    p += qsnprintf(p,cmdLen,"%p ", backtraceFrames[x]);
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

static void transcodeCharacterBuffer(const QCString &fileName,std::string &contents,
           const QCString &inputEncoding,const QCString &outputEncoding)
{
  if (inputEncoding.isEmpty() || outputEncoding.isEmpty()) return; // no encoding specified
  if (qstricmp(inputEncoding,outputEncoding)==0) return;           // input encoding same as output encoding
  void *cd = portable_iconv_open(outputEncoding.data(),inputEncoding.data());
  if (cd==reinterpret_cast<void *>(-1))
  {
    term("unsupported character conversion: '%s'->'%s': %s\n"
        "Check the INPUT_ENCODING setting in the config file!\n",
        qPrint(inputEncoding),qPrint(outputEncoding),strerror(errno));
  }
  size_t      iLeft      = contents.size();
  const char *srcPtr     = contents.data();
  size_t      tmpBufSize = contents.size()*4+1;
  size_t      oLeft      = tmpBufSize;
  std::string tmpBuf;
  tmpBuf.resize(tmpBufSize);
  char *dstPtr           = tmpBuf.data();
  size_t newSize=0;
  if (!portable_iconv(cd, &srcPtr, &iLeft, &dstPtr, &oLeft))
  {
    newSize = tmpBufSize-oLeft;
    tmpBuf.resize(newSize);
    std::swap(contents,tmpBuf);
    //printf("iconv: input size=%d output size=%d\n[%s]\n",size,newSize,qPrint(srcBuf));
  }
  else
  {
    term("%s: failed to translate characters from %s to %s: check INPUT_ENCODING\n",
        qPrint(fileName),qPrint(inputEncoding),qPrint(outputEncoding));
  }
  portable_iconv_close(cd);
}

//! read a file name \a fileName and optionally filter and transcode it
bool readInputFile(const QCString &fileName,std::string &contents,bool filter,bool isSourceCode)
{
  // try to open file
  FileInfo fi(fileName.str());
  if (!fi.exists()) return FALSE;
  QCString filterName = getFileFilter(fileName,isSourceCode);
  if (filterName.isEmpty() || !filter)
  {
    std::ifstream f = Portable::openInputStream(fileName,true);
    if (!f.is_open())
    {
      err("could not open file %s\n",qPrint(fileName));
      return FALSE;
    }
    // read the file
    contents.resize(fi.size());
    f.read(contents.data(),fi.size());
    if (f.fail())
    {
      err("problems while reading file %s\n",qPrint(fileName));
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
      err("could not execute filter %s\n",qPrint(filterName));
      return FALSE;
    }
    const int bufSize=4096;
    char buf[bufSize];
    int numRead = 0;
    while ((numRead=static_cast<int>(fread(buf,1,bufSize,f)))>0)
    {
      //printf(">>>>>>>>Reading %d bytes\n",numRead);
      contents.append(buf,numRead);
    }
    Portable::pclose(f);
    Debug::print(Debug::FilterOutput, 0, "Filter output\n");
    Debug::print(Debug::FilterOutput,0,"-------------\n%s\n-------------\n",qPrint(contents));
  }

  if (contents.size()>=2 &&
      static_cast<uint8_t>(contents[0])==0xFF &&
      static_cast<uint8_t>(contents[1])==0xFE // Little endian BOM
     ) // UCS-2LE encoded file
  {
    transcodeCharacterBuffer(fileName,contents,"UCS-2LE","UTF-8");
  }
  else if (contents.size()>=2 &&
           static_cast<uint8_t>(contents[0])==0xFE &&
           static_cast<uint8_t>(contents[1])==0xFF // big endian BOM
         ) // UCS-2BE encoded file
  {
    transcodeCharacterBuffer(fileName,contents,"UCS-2BE","UTF-8");
  }
  else if (contents.size()>=3 &&
           static_cast<uint8_t>(contents[0])==0xEF &&
           static_cast<uint8_t>(contents[1])==0xBB &&
           static_cast<uint8_t>(contents[2])==0xBF
     ) // UTF-8 encoded file
  {
    contents.erase(0,3); // remove UTF-8 BOM: no translation needed
  }
  else // transcode according to the INPUT_ENCODING setting
  {
    // do character transcoding if needed.
    transcodeCharacterBuffer(fileName,contents,getEncoding(fi),"UTF-8");
  }

  filterCRLF(contents);
  return true;
}

// Replace %word by word in title
QCString filterTitle(const QCString &title)
{
  std::string tf;
  std::string t = title.str();
  static const reg::Ex re(R"(%[a-z_A-Z]+)");
  reg::Iterator it(t,re);
  reg::Iterator end;
  size_t p = 0;
  for (; it!=end ; ++it)
  {
    const auto &match = *it;
    size_t i = match.position();
    size_t l = match.length();
    if (i>p) tf+=t.substr(p,i-p);
    tf+=match.str().substr(1); // skip %
    p=i+l;
  }
  tf+=t.substr(p);
  return QCString(tf);
}

//---------------------------------------------------------------------------------------------------

template<class PatternList, class PatternElem, typename PatternGet = QCString(*)(const PatternElem &)>
bool genericPatternMatch(const FileInfo &fi,
                         const PatternList &patList,
                         PatternElem &elem,
                         PatternGet getter)
{
  bool caseSenseNames = getCaseSenseNames();
  bool found = FALSE;

  // For platforms where the file system is non case sensitive overrule the setting
  if (!Portable::fileSystemIsCaseSensitive())
  {
    caseSenseNames = FALSE;
  }

  if (!patList.empty())
  {
    std::string fn = fi.fileName();
    std::string fp = fi.filePath();
    std::string afp= fi.absFilePath();

    for (const auto &li : patList)
    {
      std::string pattern = getter(li).str();
      if (!pattern.empty())
      {
        size_t i=pattern.find('=');
        if (i!=std::string::npos) pattern=pattern.substr(0,i); // strip of the extension specific filter name

        if (!caseSenseNames)
        {
          pattern = QCString(pattern).lower().str();
          fn      = QCString(fn).lower().str();
          fp      = QCString(fp).lower().str();
          afp     = QCString(afp).lower().str();
        }
        reg::Ex re(pattern,reg::Ex::Mode::Wildcard);
        found = re.isValid() && (reg::match(fn,re) ||
                                 (fn!=fp && reg::match(fp,re)) ||
                                 (fn!=afp && fp!=afp && reg::match(afp,re)));
        if (found)
        {
          elem = li;
          break;
        }
        //printf("Matching '%s' against pattern '%s' found=%d\n",
        //    qPrint(fi->fileName()),qPrint(pattern),found);
      }
    }
  }
  return found;
}

//----------------------------------------------------------------------------
// returns TRUE if the name of the file represented by 'fi' matches
// one of the file patterns in the 'patList' list.

bool patternMatch(const FileInfo &fi,const StringVector &patList)
{
  std::string elem;
  auto getter = [](std::string s) { return QCString(s); };
  return genericPatternMatch(fi,patList,elem,getter);
}

QCString getEncoding(const FileInfo &fi)
{
  InputFileEncoding elem;
  auto getter = [](const InputFileEncoding &e) { return e.pattern; };
  if (genericPatternMatch(fi,Doxygen::inputFileEncodingList,elem,getter)) // check for file specific encoding
  {
    return elem.encoding;
  }
  else // fall back to default encoding
  {
    return Config_getString(INPUT_ENCODING);
  }
}

QCString externalLinkTarget(const bool parent)
{
  bool extLinksInWindow = Config_getBool(EXT_LINKS_IN_WINDOW);
  if (extLinksInWindow)
    return "target=\"_blank\" ";
  else if (parent)
    return "target=\"_parent\" ";
  else
    return "";
}

QCString createHtmlUrl(const QCString &relPath,
                       const QCString &ref,
                       bool href,
                       bool isLocalFile,
                       const QCString &targetFileName,
                       const QCString &anchor)
{
  QCString url;
  if (!ref.isEmpty())
  {
    url = externalRef(relPath,ref,href);
  }
  if (!targetFileName.isEmpty())
  {
    QCString fn = targetFileName;
    if (ref.isEmpty())
    {
      if (!anchor.isEmpty() && isLocalFile)
      {
        fn=""; // omit file name for local links
      }
      else
      {
        url = relPath;
      }
    }
    url+=fn;
  }
  if (!anchor.isEmpty()) url+="#"+anchor;
  //printf("createHtmlUrl(relPath=%s,local=%d,target=%s,anchor=%s)=%s\n",qPrint(relPath),isLocalFile,qPrint(targetFileName),qPrint(anchor),qPrint(url));
  return url;
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
      size_t l = result.length();
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
void writeColoredImgData(const QCString &dir,ColoredImgDataItem data[])
{
  int hue   = Config_getInt(HTML_COLORSTYLE_HUE);
  int sat   = Config_getInt(HTML_COLORSTYLE_SAT);
  int gamma = Config_getInt(HTML_COLORSTYLE_GAMMA);
  while (data->name)
  {
    QCString fileName = dir+"/"+data->name;
    ColoredImage img(data->width,data->height,data->content,data->alpha,
                     sat,hue,gamma);
    if (!img.save(fileName))
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
QCString replaceColorMarkers(const QCString &str)
{
  if (str.isEmpty()) return QCString();
  std::string result;
  std::string s=str.str();
  static const reg::Ex re(R"(##[0-9A-Fa-f][0-9A-Fa-f])");
  reg::Iterator it(s,re);
  reg::Iterator end;
  int hue   = Config_getInt(HTML_COLORSTYLE_HUE);
  int sat   = Config_getInt(HTML_COLORSTYLE_SAT);
  int gamma = Config_getInt(HTML_COLORSTYLE_GAMMA);
  size_t sl=s.length();
  size_t p=0;
  for (; it!=end ; ++it)
  {
    const auto &match = *it;
    size_t i = match.position();
    size_t l = match.length();
    if (i>p) result+=s.substr(p,i-p);
    std::string lumStr = match.str().substr(2);
#define HEXTONUM(x) (((x)>='0' && (x)<='9') ? ((x)-'0') :       \
                     ((x)>='a' && (x)<='f') ? ((x)-'a'+10) :    \
                     ((x)>='A' && (x)<='F') ? ((x)-'A'+10) : 0)

    double r = 0,g = 0,b = 0;
    int level = HEXTONUM(lumStr[0])*16+HEXTONUM(lumStr[1]);
    ColoredImage::hsl2rgb(hue/360.0,sat/255.0,
                          pow(level/255.0,gamma/100.0),&r,&g,&b);
    int red   = static_cast<int>(r*255.0);
    int green = static_cast<int>(g*255.0);
    int blue  = static_cast<int>(b*255.0);
    char colStr[8];
    colStr[0]='#';
    colStr[1]=hex[red>>4];
    colStr[2]=hex[red&0xf];
    colStr[3]=hex[green>>4];
    colStr[4]=hex[green&0xf];
    colStr[5]=hex[blue>>4];
    colStr[6]=hex[blue&0xf];
    colStr[7]=0;
    //printf("replacing %s->%s (level=%d)\n",qPrint(lumStr),colStr,level);
    result+=colStr;
    p=i+l;
  }
  if (p<sl) result+=s.substr(p);
  return QCString(result);
}

/** Copies the contents of file with name \a src to the newly created
 *  file with name \a dest. Returns TRUE if successful.
 */
bool copyFile(const QCString &src,const QCString &dest)
{
  if (!Dir().copy(src.str(),dest.str()))
  {
    err("could not copy file %s to %s\n",qPrint(src),qPrint(dest));
    return false;
  }
  return true;
}

/** Returns the line number of the line following the line with the marker.
 *  \sa routine extractBlock
 */
int lineBlock(const QCString &text,const QCString &marker)
{
  int result = 1;

  // find the character positions of the first marker
  int m1 = text.find(marker);
  if (m1==-1) return result;

  // find start line positions for the markers
  bool found=false;
  int p=0, i=0;
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
    case SrcLangExt::Unknown:  return "Unknown";
    case SrcLangExt::IDL:      return "IDL";
    case SrcLangExt::Java:     return "Java";
    case SrcLangExt::CSharp:   return "C#";
    case SrcLangExt::D:        return "D";
    case SrcLangExt::PHP:      return "PHP";
    case SrcLangExt::ObjC:     return "Objective-C";
    case SrcLangExt::Cpp:      return "C++";
    case SrcLangExt::JS:       return "JavaScript";
    case SrcLangExt::Python:   return "Python";
    case SrcLangExt::Fortran:  return "Fortran";
    case SrcLangExt::VHDL:     return "VHDL";
    case SrcLangExt::XML:      return "XML";
    case SrcLangExt::SQL:      return "SQL";
    case SrcLangExt::Markdown: return "Markdown";
    case SrcLangExt::Slice:    return "Slice";
    case SrcLangExt::Lex:      return "Lex";
  }
  return "Unknown";
}

/** Returns the scope separator to use given the programming language \a lang */
QCString getLanguageSpecificSeparator(SrcLangExt lang,bool classScope)
{
  if (lang==SrcLangExt::Java || lang==SrcLangExt::CSharp || lang==SrcLangExt::VHDL || lang==SrcLangExt::Python)
  {
    return ".";
  }
  else if (lang==SrcLangExt::PHP && !classScope)
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
  static const std::unordered_set<std::string> schemes = {
    "http", "https", "ftp", "ftps", "sftp", "file", "news", "irc", "ircs"
  };
  QCString loc_url = url.stripWhiteSpace();
  int colonPos = loc_url.find(':');
  return colonPos!=-1 && schemes.find(loc_url.left(colonPos).str())!=schemes.end();
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
  bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
  bool extractPackage = Config_getBool(EXTRACT_PACKAGE);

  return (prot!=Protection::Private && prot!=Protection::Package)  ||
         (prot==Protection::Private && extractPrivate) ||
         (prot==Protection::Package && extractPackage);
}

//---------------------------------------------------------------------------

QCString stripIndentation(const QCString &s)
{
  if (s.isEmpty()) return s; // empty string -> we're done

  //printf("stripIndentation:\n%s\n------\n",qPrint(s));
  // compute minimum indentation over all lines
  const char *p=s.data();
  char c=0;
  int indent=0;
  int minIndent=1000000; // "infinite"
  bool searchIndent=TRUE;
  int tabSize=Config_getInt(TAB_SIZE);
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
  TextStream result;
  p=s.data();
  indent=0;
  while ((c=*p++))
  {
    if (c=='\n') // start of new line
    {
      indent=0;
      result << c;
    }
    else if (indent<minIndent) // skip until we reach minIndent
    {
      if (c=='\t')
      {
        int newIndent = indent+tabSize-(indent%tabSize);
        int i=newIndent;
        while (i>minIndent) // if a tab crosses the minIndent boundary fill the rest with spaces
        {
          result << ' ';
          i--;
        }
        indent=newIndent;
      }
      else // space
      {
        indent++;
      }
    }
    else if (c=='\\' && qstrncmp(p,"ilinebr ",8)==0)
      // we also need to remove the indentation after a \ilinebr command at the end of a line
    {
      result << "\\ilinebr ";
      p+=8;
      int skipAmount=0;
      for (int j=0;j<minIndent;j++) if (*(p+j)==' ') skipAmount++; // test to see if we have the indent
      if (skipAmount==minIndent)
      {
        p+=skipAmount; // remove the indent
      }
    }
    else // copy anything until the end of the line
    {
      result << c;
    }
  }

  //printf("stripIndentation: result=\n%s\n------\n",qPrint(result.str()));

  return result.str();
}

// strip up to \a indentationLevel spaces from each line in \a doc (excluding the first line)
void stripIndentationVerbatim(QCString &doc,const int indentationLevel)
{
  //printf("stripIndentationVerbatim(level=%d):\n%s\n------\n",indentationLevel,qPrint(doc));
  if (indentationLevel <= 0 || doc.isEmpty()) return; // nothing to strip

  // by stripping content the string will only become shorter so we write the results
  // back into the input string and then resize it at the end.
  char c = 0;
  const char *src = doc.data();
  char *dst = doc.rawData();
  bool insideIndent = false; // skip the initial line from stripping
  int cnt = 0;
  while ((c=*src++))
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
  doc.resize(static_cast<uint32_t>(dst-doc.data()));
  //printf("stripIndentationVerbatim: result=\n%s\n------\n",qPrint(doc));
}

bool fileVisibleInIndex(const FileDef *fd,bool &genSourceFile)
{
  bool allExternals = Config_getBool(ALLEXTERNALS);
  bool isDocFile = fd->isDocumentationFile();
  genSourceFile = !isDocFile && fd->generateSourceFile();
  return ( ((allExternals && fd->isLinkable()) ||
            fd->isLinkableInProject()
           ) &&
           !isDocFile
         );
}

//--------------------------------------------------------------------------------------

#if 0
/*! @brief Get one unicode character as an unsigned integer from utf-8 string
 *
 * @param s utf-8 encoded string
 * @param idx byte position of given string \a s.
 * @return the unicode codepoint, 0 - MAX_UNICODE_CODEPOINT
 * @see getNextUtf8OrToLower()
 * @see getNextUtf8OrToUpper()
 */
uint32_t getUtf8Code( const QCString& s, int idx )
{
  const int length = s.length();
  if (idx >= length) { return 0; }
  const uint32_t c0 = (uint8_t)s.at(idx);
  if ( c0 < 0xC2 || c0 >= 0xF8 ) // 1 byte character
  {
    return c0;
  }
  if (idx+1 >= length) { return 0; }
  const uint32_t c1 = ((uint8_t)s.at(idx+1)) & 0x3f;
  if ( c0 < 0xE0 ) // 2 byte character
  {
    return ((c0 & 0x1f) << 6) | c1;
  }
  if (idx+2 >= length) { return 0; }
  const uint32_t c2 = ((uint8_t)s.at(idx+2)) & 0x3f;
  if ( c0 < 0xF0 ) // 3 byte character
  {
    return ((c0 & 0x0f) << 12) | (c1 << 6) | c2;
  }
  if (idx+3 >= length) { return 0; }
  // 4 byte character
  const uint32_t c3 = ((uint8_t)s.at(idx+3)) & 0x3f;
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
uint32_t getUtf8CodeToLower( const QCString& s, int idx )
{
  const uint32_t v = getUtf8Code( s, idx );
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
uint32_t getUtf8CodeToUpper( const QCString& s, int idx )
{
  const uint32_t v = getUtf8Code( s, idx );
  return v < 0x7f ? toupper( v ) : v;
}
#endif



//----------------------------------------------------------------------------

/** Strip the direction part from docs and return it as a string in canonical form
 *  The input \a docs string can start with e.g. "[in]", "[in, out]", "[inout]", "[out,in]"...
 *  @returns either "[in,out]", "[in]", or "[out]" or the empty string.
 */
QCString extractDirection(QCString &docs)
{
  std::string s = docs.str();
  static const reg::Ex re(R"(\[([ inout,]+)\])");
  reg::Iterator it(s,re);
  reg::Iterator end;
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
      unsigned char ioMask=0;
      size_t inIndex  = dir.find( "in");
      size_t outIndex = dir.find("out");
      if ( inIndex!=std::string::npos) dir.erase( inIndex,2),ioMask|=(1<<0);
      if (outIndex!=std::string::npos) dir.erase(outIndex,3),ioMask|=(1<<1);
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
                   MemberListType *outListType1,
                   MemberListType *outListType2
                  )
{
  bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);

  // default representing 1-1 mapping
  *outListType1=inListType;
  *outListType2=MemberListType::Invalid();

  if (inProt==Protection::Public)
  {
    if (inListType.isPrivate())
    {
      *outListType1=MemberListType::Invalid();
    }
  }
  else if (inProt==Protection::Protected)
  {
    if (inListType.isPrivate() || inListType.isPublic())
    {
      *outListType1=MemberListType::Invalid();
    }
    else if (inListType.isProtected())
    {
      *outListType2=inListType.toPublic();
    }
  }
  else if (inProt==Protection::Private)
  {
    if (inListType.isPublic() || inListType.isProtected())
    {
      *outListType1=MemberListType::Invalid();
    }
    else if (inListType.isPrivate())
    {
      if (extractPrivate)
      {
        *outListType1=inListType.toPublic();
        *outListType2=inListType.toProtected();
      }
      else
      {
        *outListType1=MemberListType::Invalid();
      }
    }
  }

  //printf("convertProtectionLevel(type=%s prot=%d): %s,%s\n",
  //    qPrint(inListType.to_string()),inProt,qPrint(outListType1->to_string()),qPrint(outListType2->to_string()));
}

bool mainPageHasTitle()
{
  return Doxygen::mainPage!=nullptr && Doxygen::mainPage->hasTitle();
}

QCString getDotImageExtension()
{
  QCString imgExt = Config_getEnumAsString(DOT_IMAGE_FORMAT);
  int i= imgExt.find(':'); // strip renderer part when using e.g. 'png:cairo:gd' as format
  return i==-1 ? imgExt : imgExt.left(i);
}

bool openOutputFile(const QCString &outFile,std::ofstream &f)
{
  assert(!f.is_open());
  bool fileOpened=FALSE;
  bool writeToStdout=outFile=="-";
  if (writeToStdout) // write to stdout
  {
    f.basic_ios<char>::rdbuf(std::cout.rdbuf());
    fileOpened = true;
  }
  else // write to file
  {
    FileInfo fi(outFile.str());
    if (fi.exists()) // create a backup
    {
      Dir dir;
      FileInfo backup(fi.filePath()+".bak");
      if (backup.exists()) // remove existing backup
        dir.remove(backup.filePath());
      dir.rename(fi.filePath(),fi.filePath()+".bak");
    }
    f = Portable::openOutputStream(outFile);
    fileOpened = f.is_open();
  }
  return fileOpened;
}

static bool keyWordsFortranC(const char *contents)
{
  static const std::unordered_set<std::string> fortran_C_keywords = {
    "character", "call", "close", "common", "continue",
    "case", "contains", "cycle", "class", "codimension",
    "concurrent", "contiguous", "critical"
  };

  if (*contents != 'c' && *contents != 'C') return false;

  const char *c = contents;
  QCString keyword;
  while (*c && *c != ' ') {keyword += *c; c++;}
  keyword = keyword.lower();

  return (fortran_C_keywords.find(keyword.str()) != fortran_C_keywords.end());
}

//------------------------------------------------------
// simplified way to know if this is fixed form
bool recognizeFixedForm(const QCString &contents, FortranFormat format)
{
  int column=0;
  bool skipLine=FALSE;

  if (format == FortranFormat::Fixed) return TRUE;
  if (format == FortranFormat::Free)  return FALSE;

  int tabSize=Config_getInt(TAB_SIZE);
  size_t sizCont = contents.length();
  for (size_t i=0;i<sizCont;i++)
  {
    column++;

    switch(contents.at(i))
    {
      case '\n':
        column=0;
        skipLine=FALSE;
        break;
      case '\t':
        column += tabSize-1;
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
        if (column==1)
        {
          return !keyWordsFortranC(contents.data()+i);
        }
        // fallthrough
      case '*':
        if (column==1) return TRUE;
        if (skipLine) break;
        return FALSE;
      case '!':
        if (column!=6) skipLine=TRUE;
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
  QCString parserName = Doxygen::parserManager->getParserName(ext);

  if (parserName == "fortranfixed") return FortranFormat::Fixed;
  else if (parserName == "fortranfree") return FortranFormat::Free;

  return FortranFormat::Unknown;
}
//------------------------------------------------------------------------

//! remove disabled blocks and all block markers from \a s and return the result as a string
QCString selectBlocks(const QCString &s,const SelectionBlockList &blockList,const SelectionMarkerInfo &markerInfo)
{
  if (s.isEmpty()) return s;

  // helper to find the end of a block
  auto skipBlock = [&markerInfo](const char *p,const SelectionBlock &blk)
  {
    char c = 0;
    while ((c=*p))
    {
      if (c==markerInfo.markerChar && qstrncmp(p,markerInfo.endStr,markerInfo.endLen)==0) // end marker
      {
        size_t len = markerInfo.endLen;
        bool negate = *(p+markerInfo.endLen)=='!';
        if (negate) len++;
        size_t blkNameLen = qstrlen(blk.name);
        if (qstrncmp(p+len,blk.name,blkNameLen)==0 &&                                // matching marker name
            qstrncmp(p+len+blkNameLen,markerInfo.closeStr,markerInfo.closeLen)==0) // matching marker closing
        {
          //printf("Found end marker %s enabled=%d negate=%d\n",blk.name,blk.enabled,negate);
          return p+len+blkNameLen+markerInfo.closeLen;
        }
        else // not the right marker id
        {
          p++;
        }
      }
      else // not and end marker
      {
        p++;
      }
    }
    return p;
  };

  QCString result;
  result.reserve(s.length());
  const char *p = s.data();
  char c = 0;
  while ((c=*p))
  {
    if (c==markerInfo.markerChar) // potential start of marker
    {
      if (qstrncmp(p,markerInfo.beginStr,markerInfo.beginLen)==0) // start of begin marker
      {
        bool found = false;
        size_t len = markerInfo.beginLen;
        bool negate = *(p+len)=='!';
        if (negate) len++;
        for (const auto &blk : blockList)
        {
          size_t blkNameLen = qstrlen(blk.name);
          if (qstrncmp(p+len,blk.name,blkNameLen)==0 &&                                // matching marker name
              qstrncmp(p+len+blkNameLen,markerInfo.closeStr,markerInfo.closeLen)==0) // matching marker closing
          {
            bool blockEnabled = blk.enabled!=negate;
            //printf("Found start marker %s enabled=%d negate=%d\n",blk.name,blk.enabled,negate);
            p+=len+blkNameLen+markerInfo.closeLen;
            if (!blockEnabled) // skip until the end of the block
            {
              //printf("skipping block\n");
              p=skipBlock(p,blk);
            }
            found=true;
            break;
          }
        }
        if (!found) // unknown marker id
        {
          result+=c;
          p++;
        }
      }
      else if (qstrncmp(p,markerInfo.endStr,markerInfo.endLen)==0) // start of end marker
      {
        bool found = false;
        size_t len = markerInfo.endLen;
        bool negate = *(p+len)=='!';
        if (negate) len++;
        for (const auto &blk : blockList)
        {
          size_t blkNameLen = qstrlen(blk.name);
          if (qstrncmp(p+len,blk.name,blkNameLen)==0 &&                                // matching marker name
              qstrncmp(p+len+blkNameLen,markerInfo.closeStr,markerInfo.closeLen)==0) // matching marker closing
          {
            //printf("Found end marker %s enabled=%d negate=%d\n",blk.name,blk.enabled,negate);
            p+=len+blkNameLen+markerInfo.closeLen;
            found=true;
            break;
          }
        }
        if (!found) // unknown marker id
        {
          result+=c;
          p++;
        }
      }
      else // not a start or end marker
      {
        result+=c;
        p++;
      }
    }
    else // not a marker character
    {
      result+=c;
      p++;
    }
  }
  //printf("====\n%s\n-----\n%s\n~~~~\n",qPrint(s),qPrint(result));
  return result;
}

void checkBlocks(const QCString &s, const QCString fileName,const SelectionMarkerInfo &markerInfo)
{
  if (s.isEmpty()) return;

  const char *p = s.data();
  char c = 0;
  while ((c=*p))
  {
    if (c==markerInfo.markerChar) // potential start of marker
    {
      if (qstrncmp(p,markerInfo.beginStr,markerInfo.beginLen)==0) // start of begin marker
      {
        size_t len = markerInfo.beginLen;
        bool negate = *(p+len)=='!';
        if (negate) len++;
        p += len;
        QCString marker;
        while (*p)
        {
          if (markerInfo.closeLen==0 && *p=='\n') // matching end of line
          {
            warn(fileName,-1,"Remaining begin replacement with marker '%s'",qPrint(marker));
            break;
          }
          else if (markerInfo.closeLen!= 0 && qstrncmp(p,markerInfo.closeStr,markerInfo.closeLen)==0) // matching marker closing
          {
            p += markerInfo.closeLen;
            warn(fileName,-1,"Remaining begin replacement with marker '%s'",qPrint(marker));
            break;
          }
          marker += *p;
          p++;
        }
      }
      else if (qstrncmp(p,markerInfo.endStr,markerInfo.endLen)==0) // start of end marker
      {
        size_t len = markerInfo.endLen;
        bool negate = *(p+len)=='!';
        if (negate) len++;
        p += len;
        QCString marker;
        while (*p)
        {
          if (markerInfo.closeLen==0 && *p=='\n') // matching end of line
          {
            warn(fileName,-1,"Remaining end replacement with marker '%s'",qPrint(marker));
            break;
          }
          else if (markerInfo.closeLen!= 0 && qstrncmp(p,markerInfo.closeStr,markerInfo.closeLen)==0) // matching marker closing
          {
            p += markerInfo.closeLen;
            warn(fileName,-1,"Remaining end replacement with marker '%s'",qPrint(marker));
            break;
          }
          marker += *p;
          p++;
        }
      }
    }
    p++;
  }
}


QCString removeEmptyLines(const QCString &s)
{
  std::string out;
  out.reserve(s.length());
  const char *p=s.data();
  if (p)
  {
    char c = 0;
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
        else out+=c;
      }
      else
      {
        out+=c;
      }
    }
  }
  //printf("removeEmptyLines(%s)=%s\n",qPrint(s),qPrint(out));
  return out;
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
StringVector split(const std::string &s,const reg::Ex &delimiter)
{
  StringVector result;
  reg::Iterator iter(s, delimiter);
  reg::Iterator end;
  size_t p=0;
  for ( ; iter != end; ++iter)
  {
    const auto &match = *iter;
    size_t i=match.position();
    size_t l=match.length();
    if (i>p) result.push_back(s.substr(p,i-p));
    p=i+l;
  }
  if (p<s.length()) result.push_back(s.substr(p));
  return result;
}

/// find the index of a string in a vector of strings, returns -1 if the string could not be found
int findIndex(const StringVector &sv,const std::string &s)
{
  auto it = std::find(sv.begin(),sv.end(),s);
  return it!=sv.end() ? static_cast<int>(it-sv.begin()) : -1;
}

/// find the index of the first occurrence of pattern \a re in a string \a s
/// returns -1 if the pattern could not be found
int findIndex(const std::string &s,const reg::Ex &re)
{
  reg::Match match;
  return reg::search(s,match,re) ? static_cast<int>(match.position()) : -1;
}

/// create a string where the string in the vector are joined by the given delimiter
std::string join(const StringVector &sv,const std::string &delimiter)
{
  std::string result;
  bool first=true;
  for (const auto &s : sv)
  {
    if (!first) result+=delimiter;
    first=false;
    result+=s;
  }
  return result;
}

QCString integerToAlpha(int n, bool upper)
{
  QCString result;
  int residual = n;

  char modVal[2];
  modVal[1] = 0;
  while (residual > 0)
  {
    modVal[0] = (upper ? 'A': 'a') + (residual-1)%26;
    result = modVal + result;
    residual = (residual-1) / 26;
  }
  return result;
}

QCString integerToRoman(int n, bool upper)
{
  static const char *str_romans_upper[] = {  "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" };
  static const char *str_romans_lower[] = {  "m", "cm", "d", "cd", "c", "xc", "l", "xl", "x", "ix", "v", "iv", "i" };
  static const int values[]             = { 1000,  900, 500,  400, 100,   90,  50,   40,  10,    9,   5,    4,   1 };
  static const char **str_romans = upper ? str_romans_upper : str_romans_lower;

  QCString result;
  int residual = n;

  for (int i = 0; i < 13; ++i)
  {
    while (residual - values[i] >= 0)
    {
      result += str_romans[i];
      residual -= values[i];
    }
  }

  return result;
}

QCString detab(const QCString &s,size_t &refIndent)
{
  int tabSize = Config_getInt(TAB_SIZE);
  size_t size = s.length();
  GrowBuf out(size);
  const char *data = s.data();
  size_t i=0;
  int col=0;
  constexpr auto doxy_nbsp = "&_doxy_nbsp;";  // doxygen escape command for UTF-8 nbsp
  const int maxIndent=1000000; // value representing infinity
  int minIndent=maxIndent;
  bool skip = false;
  while (i<size)
  {
    char c = data[i++];
    switch(c)
    {
      case '\t': // expand tab
        {
          int stop = tabSize - (col%tabSize);
          //printf("expand at %d stop=%d\n",col,stop);
          col+=stop;
          while (stop--) out.addChar(' ');
        }
        break;
      case '\\':
        if (data[i] == '\\') // escaped command -> ignore
        {
          out.addChar(c);
          out.addChar(data[i++]);
          col+=2;
        }
        else if (i+5<size && qstrncmp(&data[i],"iskip",5)==0) // \iskip command
        {
          i+=5;
          skip = true;
        }
        else if (i+8<size && qstrncmp(&data[i],"endiskip",8)==0) // \endiskip command
        {
          i+=8;
          skip = false;
        }
        else // some other command
        {
          out.addChar(c);
          col++;
        }
        break;
      case '\n': // reset column counter
        out.addChar(c);
        col=0;
        break;
      case ' ': // increment column counter
        out.addChar(c);
        col++;
        break;
      default: // non-whitespace => update minIndent
        if (c<0 && i<size) // multibyte sequence
        {
          // special handling of the UTF-8 nbsp character 0xC2 0xA0
          int nb = isUTF8NonBreakableSpace(data);
          if (nb>0)
          {
            out.addStr(doxy_nbsp);
            i+=nb-1;
          }
          else
          {
            int bytes = getUTF8CharNumBytes(c);
            for (int j=0;j<bytes-1 && c;j++)
            {
              out.addChar(c);
              c = data[i++];
            }
            out.addChar(c);
          }
        }
        else
        {
          out.addChar(c);
        }
        if (!skip && col<minIndent) minIndent=col;
        col++;
    }
  }
  if (minIndent!=maxIndent) refIndent=minIndent; else refIndent=0;
  out.addChar(0);
  //printf("detab(\n%s\n)=[\n%s\n]\n",qPrint(s),qPrint(out.get()));
  return out.get();
}

QCString getProjectId()
{
  QCString projectCookie = Config_getString(HTML_PROJECT_COOKIE);
  if (projectCookie.isEmpty()) return QCString();
  uint8_t md5_sig[16];
  char sigStr[34];
  MD5Buffer(projectCookie.data(),static_cast<unsigned int>(projectCookie.length()),md5_sig);
  MD5SigToString(md5_sig,sigStr);
  sigStr[32]='_'; sigStr[33]=0;
  return sigStr;
}

//! Return the index of the last :: in the string \a name that is still before the first <
int computeQualifiedIndex(const QCString &name)
{
  int l = static_cast<int>(name.length());
  int lastSepPos = -1;
  const char *p = name.data();
  int i=l-2;
  int sharpCount=0;
  // --- begin optimized version of ts=name.findRev(">::");
  int ts = -1;
  while (i>=0)
  {
    if (p[i]=='>')
    {
      if (sharpCount==0 && p[i+1]==':' && p[i+2]==':')
      {
        ts=i;
        break;
      }
      sharpCount++;
    }
    else if (p[i]=='<')
    {
      sharpCount--;
    }
    i--;
  }
  // --- end optimized version
  if (ts==-1) ts=0; else p+=++ts;
  for (i=ts;i<l-1;i++)
  {
    char c=*p++;
    if (c==':' && *p==':') lastSepPos=i;
    if (c=='<') break;
  }
  return lastSepPos;
}

void mergeMemberOverrideOptions(MemberDefMutable *md1,MemberDefMutable *md2)
{
  if (Config_getBool(CALL_GRAPH)  !=md1->hasCallGraph())   md2->overrideCallGraph(md1->hasCallGraph());
  if (Config_getBool(CALLER_GRAPH)!=md1->hasCallerGraph()) md2->overrideCallerGraph(md1->hasCallerGraph());
  if (Config_getBool(CALL_GRAPH)  !=md2->hasCallGraph())   md1->overrideCallGraph( md2->hasCallGraph());
  if (Config_getBool(CALLER_GRAPH)!=md2->hasCallerGraph()) md1->overrideCallerGraph(md2->hasCallerGraph());

  if (Config_getBool(SHOW_ENUM_VALUES)  !=md1->hasEnumValues())   md2->overrideEnumValues(md1->hasEnumValues());
  if (Config_getBool(SHOW_ENUM_VALUES)  !=md2->hasEnumValues())   md1->overrideEnumValues( md2->hasEnumValues());

  if (Config_getBool(REFERENCED_BY_RELATION)!=md1->hasReferencedByRelation()) md2->overrideReferencedByRelation(md1->hasReferencedByRelation());
  if (Config_getBool(REFERENCES_RELATION)   !=md1->hasReferencesRelation())   md2->overrideReferencesRelation(md1->hasReferencesRelation());
  if (Config_getBool(REFERENCED_BY_RELATION)!=md2->hasReferencedByRelation()) md1->overrideReferencedByRelation(md2->hasReferencedByRelation());
  if (Config_getBool(REFERENCES_RELATION)   !=md2->hasReferencesRelation())   md1->overrideReferencesRelation(md2->hasReferencesRelation());

  if (Config_getBool(INLINE_SOURCES)!=md1->hasInlineSource()) md2->overrideInlineSource(md1->hasInlineSource());
  if (Config_getBool(INLINE_SOURCES)!=md2->hasInlineSource()) md1->overrideInlineSource(md2->hasInlineSource());
}

