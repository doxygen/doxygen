/*****************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

// own include
#include "util.h"

// std includes (sorted)
#include <algorithm>
#include <cctype>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <mutex>
#include <unordered_set>

// other includes (sorted)
#include "arguments.h"
#include "classdef.h"
#include "classlist.h"
#include "conceptdef.h"
#include "config.h"
#include "datetime.h"
#include "debug.h"
#include "defargs.h"
#include "dir.h"
#include "dirdef.h"
#include "doxygen.h"
#include "example.h"
#include "filedef.h"
#include "fileinfo.h"
#include "filename.h"
#include "groupdef.h"
#include "htmlentity.h"
#include "image.h"
#include "language.h"
#include "md5hash.h"
#include "membername.h"
#include "message.h"
#include "moduledef.h"
#include "namespacedef.h"
#include "outputlist.h"
#include "pagedef.h"
#include "parserintf.h"
#include "portable.h"
#include "regex.h"
#include "stringutil.h"
#include "symbolresolver.h"
#include "textstream.h"
#include "trace.h"
#include "utf8.h"

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
DString removeAnonymousScopes(const DString &str)
{
  std::string result;
  if (str.empty()) return result;

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
DString replaceAnonymousScopes(const DString &s,const DString &replacement)
{
  if (s.empty()) return s;
  static const reg::Ex marker(R"(@\d+)");
  std::string result = reg::replace(s.str(),marker,
                                    !replacement.empty() ? replacement.data() : "__anonymous__");
  //printf("replaceAnonymousScopes('%s')='%s'\n",qPrint(s),qPrint(result));
  return result;
}


// strip anonymous left hand side part of the scope
DString stripAnonymousNamespaceScope(const DString &s)
{
  int i=0,p=0,l=0;
  DString newScope;
  int sl = static_cast<int>(s.length());
  while ((i=getScopeFragment(s,p,&l))!=-1)
  {
    //printf("Scope fragment %s\n",qPrint(s.mid(i,l)));
    if (Doxygen::namespaceLinkedMap->find(s.left(i+l))!=nullptr)
    {
      if (s.at(i)!='@')
      {
        if (!newScope.empty()) newScope+="::";
        newScope+=s.mid(i,l);
      }
    }
    else if (i<sl)
    {
      if (!newScope.empty()) newScope+="::";
      newScope+=s.right(sl-i);
      goto done;
    }
    p=i+l;
  }
done:
  //printf("stripAnonymousNamespaceScope('%s')='%s'\n",qPrint(s),qPrint(newScope));
  return newScope;
}

static inline DString stripFromPath(const DString &p,StringVector l)
{
  // look at all the strings in the list and strip the longest match
  DString potential;
  DString path=Portable::removeLongPathMarker(p);
  size_t length = 0;
  for (const auto &s : l)
  {
    DString prefix = s;
    if (prefix.length() > length &&
        dstricmp(path.left(prefix.length()),prefix)==0) // case insensitive compare
    {
      length = prefix.length();
      potential = path.mid(prefix.length());
    }
  }
  if (length>0) return potential;
  return path;
}

/*! strip part of \a path if it matches
 *  one of the paths in the Config_getList(STRIP_FROM_PATH) list
 */
DString stripFromPath(const DString &path)
{
  return stripFromPath(path,Config_getList(STRIP_FROM_PATH));
}

/*! strip part of \a path if it matches
 *  one of the paths in the Config_getList(INCLUDE_PATH) list
 */
DString stripFromIncludePath(const DString &path)
{
  return stripFromPath(path,Config_getList(STRIP_FROM_INC_PATH));
}

DString resolveTypeDef(const Definition *context,const DString &qualifiedName,
                        const Definition **typedefContext)
{
  AUTO_TRACE("context='{}' qualifiedName='{}'",context?context->name():"",qualifiedName);
  DString result;
  if (qualifiedName.empty())
  {
    AUTO_TRACE_EXIT("empty name");
    return result;
  }

  const Definition *mContext=context;
  if (typedefContext) *typedefContext=context;

  // see if the qualified name has a scope part
  if (qualifiedName.find('<')!=DString::npos)
  {
    AUTO_TRACE_EXIT("template");
    return result;
  }
  size_t scopeIndex = qualifiedName.rfind("::");
  DString resName=qualifiedName;
  if (scopeIndex!=DString::npos) // strip scope part for the name
  {
    resName=qualifiedName.mid(scopeIndex+2);
    if (resName.empty())
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
    if (scopeIndex!=DString::npos)
    {
      // split-off scope part
      DString resScopeName = qualifiedName.left(scopeIndex);
      //printf("resScopeName='%s'\n",qPrint(resScopeName));

      // look-up scope in context
      int is=0,ps=0,l=0;
      while ((is=getScopeFragment(resScopeName,ps,&l))!=-1)
      {
        DString qualScopePart = resScopeName.mid(is,l);
        DString tmp = resolveTypeDef(mContext,qualScopePart);
        if (!tmp.empty()) qualScopePart=tmp;
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
    DString args = md->argsString();
    if (args.find(")(")!=DString::npos) // typedef of a function/member pointer
    {
      result+=args;
    }
    else if (args.find('[')!=DString::npos) // typedef of an array
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
    charMap[static_cast<int>('(')].before=false;
    charMap[static_cast<int>('=')].before=false;
    charMap[static_cast<int>('&')].before=false;
    charMap[static_cast<int>('*')].before=false;
    charMap[static_cast<int>('[')].before=false;
    charMap[static_cast<int>('|')].before=false;
    charMap[static_cast<int>('+')].before=false;
    charMap[static_cast<int>(';')].before=false;
    charMap[static_cast<int>(':')].before=false;
    charMap[static_cast<int>('/')].before=false;

    charMap[static_cast<int>('=')].after=false;
    charMap[static_cast<int>(' ')].after=false;
    charMap[static_cast<int>('[')].after=false;
    charMap[static_cast<int>(']')].after=false;
    charMap[static_cast<int>('\t')].after=false;
    charMap[static_cast<int>('\n')].after=false;
    charMap[static_cast<int>(')')].after=false;
    charMap[static_cast<int>(',')].after=false;
    charMap[static_cast<int>('<')].after=false;
    charMap[static_cast<int>('|')].after=false;
    charMap[static_cast<int>('+')].after=false;
    charMap[static_cast<int>('(')].after=false;
    charMap[static_cast<int>('/')].after=false;
  }
  struct CharElem
  {
    CharElem() : before(true), after(true) {}
    bool before;
    bool after;
  };

  CharElem charMap[256];
};

static CharAroundSpace g_charAroundSpace;

// Note: this function is not reentrant due to the use of static buffer!
DString removeRedundantWhiteSpace(const DString &s)
{
  bool cliSupport = Config_getBool(CPP_CLI_SUPPORT);
  bool vhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);

  if (s.empty() || vhdl) return s;

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
                   pc!='*' && pc!='&'  && pc!='(' && pc!='/' && pc!='[' &&
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
              !(isId(nc) || nc==')' || nc==',' || disspace(nc))
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
 * begins, or DString::npos if one is not found.
 */
static size_t findParameterList(const DString &name)
{
  size_t pos=DString::npos;
  int templateDepth=0;
  do
  {
    if (templateDepth > 0)
    {
      size_t nextOpenPos  = name.rfind('>', pos);
      size_t nextClosePos = name.rfind('<', pos);
      if (nextOpenPos!=DString::npos && nextClosePos!=DString::npos && nextOpenPos>nextClosePos)
      {
        ++templateDepth;
        pos=nextOpenPos-1;
      }
      else if (nextClosePos!=DString::npos)
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
      size_t lastAnglePos = name.rfind('>', pos);
      size_t bracePos     = name.rfind('(', pos);
      if (lastAnglePos!=DString::npos && bracePos!=DString::npos && lastAnglePos>bracePos)
      {
        ++templateDepth;
        pos=lastAnglePos-1;
      }
      else
      {
        size_t bp = bracePos>0 ? name.rfind('(',bracePos-1) : DString::npos;
        // bp test is to allow foo(int(&)[10]), but we need to make an exception for operator()
        return bp==DString::npos || (bp>=8 && name.mid(bp-8,10)=="operator()") ? bracePos : bp;
      }
    }
  } while (pos!=DString::npos);
  return DString::npos;
}

bool rightScopeMatch(const DString &scope, const DString &name)
{
  size_t sl=scope.length();
  size_t nl=name.length();
  return (name==scope || // equal
          (scope.right(nl)==name && // substring
           sl>1+nl && scope.at(sl-nl-1)==':' && scope.at(sl-nl-2)==':' // scope
          )
         );
}

bool leftScopeMatch(const DString &scope, const DString &name)
{
  size_t sl=scope.length();
  size_t nl=name.length();
  return (name==scope || // equal
          (name.left(sl)==scope && // substring
           nl>sl+1 && name.at(sl)==':' && name.at(sl+1)==':' // scope
          )
         );
}



void writeMarkerList(OutputList &ol,const std::string &markerText,size_t numMarkers,
                     std::function<void(size_t)> replaceFunc)
{
  static const reg::Ex marker(R"(@(\d+))");
  reg::Iterator it(markerText,marker);
  reg::Iterator end;
  size_t index=0;
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

DString writeMarkerList(const std::string &markerText,size_t numMarkers,
                         std::function<DString(size_t)> replaceFunc)
{
  DString result;
  static const reg::Ex marker(R"(@(\d+))");
  reg::Iterator it(markerText,marker);
  reg::Iterator end;
  size_t index=0;
  for ( ; it!=end ; ++it)
  {
    const auto &match = *it;
    size_t newIndex = match.position();
    size_t matchLen = match.length();
    result += markerText.substr(index,newIndex-index);
    unsigned long entryIndex = std::stoul(match[1].str());
    if (entryIndex<static_cast<unsigned long>(numMarkers))
    {
      result+=replaceFunc(entryIndex);
    }
    index=newIndex+matchLen;
  }
  if (index<markerText.size())
  {
    result += markerText.substr(index);
  }
  return result;
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
    ol.writeObjectLink(DString(),e.file,e.anchor,e.name);
    ol.popGeneratorState();

    ol.pushGeneratorState();
    ol.disable(OutputType::Man);
    ol.disable(OutputType::Html);
    // link for Latex / pdf with anchor because the sources
    // are not hyperlinked (not possible with a verbatim environment).
    ol.writeObjectLink(DString(),e.file,DString(),e.name);
    ol.popGeneratorState();
  };

  writeMarkerList(ol, theTranslator->trWriteList(static_cast<int>(list.size())).str(), list.size(), replaceFunc);

  ol.writeString(".");
}

DString inlineTemplateArgListToDoc(const ArgumentList &al)
{
  DString paramDocs;
  if (al.hasTemplateDocumentation())
  {
    for (const Argument &a : al)
    {
      if (!a.docs.empty())
      {
        if (!a.name.empty())
        {
          paramDocs+=" \\ilinebr @tparam "+a.name+" "+a.docs;
        }
        else if (!a.type.empty())
        {
          DString type = a.type;
          type.stripPrefix("class ");
          type.stripPrefix("typename ");
          type = type.stripWhiteSpace();
          paramDocs+=" \\ilinebr @tparam "+type+" "+a.docs;
        }
      }
    }
  }
  return paramDocs;
}

DString argListToString(const ArgumentList &al,bool useCanonicalType,bool showDefVals)
{
  DString result;
  if (!al.hasParameters()) return result;
  result+="(";
  for (auto it = al.begin() ; it!=al.end() ;)
  {
    Argument a = *it;
    DString type1 = useCanonicalType && !a.canType.empty() ? a.canType : a.type;
    DString type2;
    if (size_t i=type1.find(")("); i!=DString::npos) // hack to deal with function pointers
    {
      type2=type1.mid(i);
      type1=type1.left(i);
    }
    if (!a.attrib.empty())
    {
      result+=a.attrib+" ";
    }
    if (!a.name.empty() || !a.array.empty())
    {
      result+= type1+" "+a.name+type2+(useCanonicalType && !a.canType.isEmpty()?"":a.array);
    }
    else
    {
      result+= type1+type2;
    }
    if (!a.defval.empty() && showDefVals)
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
  if (!al.trailingReturnType().empty()) result+=al.trailingReturnType();
  if (al.pureSpecifier()) result+=" =0";
  return removeRedundantWhiteSpace(result);
}

DString tempArgListToString(const ArgumentList &al,SrcLangExt lang,bool includeDefault)
{
  DString result;
  if (al.empty()) return result;
  result="<";
  bool first=true;
  for (const auto &a : al)
  {
    if (a.defval.empty() || includeDefault)
    {
      if (!first) result+=", ";
      if (!a.name.empty()) // add template argument name
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
          if (a.type.find("...")!=DString::npos)
          {
            result+="...";
          }
        }
        else // nothing found -> take whole name
        {
          result+=a.type;
        }
      }
      if (!a.typeConstraint.empty() && lang==SrcLangExt::Java)
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
 * smaller (DOS)).
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

static DString getFilterFromList(const DString &name,const StringVector &filterList,bool &found)
{
  found=false;
  // compare the file name to the filter pattern list
  for (const auto &filterStr : filterList)
  {
    DString fs = filterStr;
    if (size_t i_equals=fs.find('='); i_equals!=DString::npos)
    {
      DString filterPattern = fs.left(i_equals);
      DString input = name;
      if (!Portable::fileSystemIsCaseSensitive())
      {
        filterPattern = filterPattern.lower();
        input = input.lower();
      }
      reg::Ex re(filterPattern.str(),reg::Ex::Mode::Wildcard);
      if (re.isValid() && reg::match(input.str(),re))
      {
        // found a match!
        DString filterName = fs.mid(i_equals+1);
        if (filterName.find(' ')!=DString::npos)
        { // add quotes if the name has spaces
          filterName="\""+filterName+"\"";
        }
        found=true;
        return filterName;
      }
    }
  }

  // no match
  return "";
}

DString getFileFilter(const DString &name,bool isSourceCode)
{
  // sanity check
  if (name.empty()) return "";

  StringVector filterSrcList = Config_getList(FILTER_SOURCE_PATTERNS);
  StringVector filterList    = Config_getList(FILTER_PATTERNS);

  DString filterName;
  bool found=false;
  if (isSourceCode && !filterSrcList.empty())
  { // first look for source filter pattern list
    filterName = getFilterFromList(name,filterSrcList,found);
  }
  if (!found && filterName.empty())
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

DString fileToString(const DString &name,bool filter,bool isSourceCode)
{
  if (name.empty()) return DString();
  bool fileOpened=false;
  if (name[0]=='-' && name[1]==0) // read from stdin
  {
    std::string contents;
    std::string line;
    while (getline(std::cin,line))
    {
      contents+=line+'\n';
    }
    return contents;
  }
  else // read from file
  {
    FileInfo fi(name.str());
    if (!fi.exists() || !fi.isFile())
    {
      err("file '{}' not found\n",name);
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
    err("cannot open file '{}' for reading\n",name);
  }
  return "";
}

void trimBaseClassScope(const BaseClassList &bcl,DString &s,int level=0)
{
  //printf("trimBaseClassScope level=%d '%s'\n",level,qPrint(s));
  for (const auto &bcd : bcl)
  {
    ClassDef *cd=bcd.classDef;
    //printf("Trying class %s\n",qPrint(cd->name()));
    if (size_t spos=s.find(cd->name()+"::"); spos!=DString::npos)
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

static void stripIrrelevantString(DString &target,const DString &str,bool insideTemplate)
{
  AUTO_TRACE("target='{}' str='{}'",target,str);
  if (target==str) { target.clear(); return; }
  size_t i=0,p=0;
  size_t l=str.length();
  bool changed=false;
  int sharpCount=0;
  while ((i=target.find(str,p))!=DString::npos)
  {
    for (size_t q=p;q<i;q++)
    {
      if (target[q]=='<') sharpCount++;
      else if (target[q]=='>' && sharpCount>0) sharpCount--;
    }
    bool isMatch = (i==0 || !isId(target.at(i-1))) && // not a character before str
         (i+l==target.length() || !isId(target.at(i+l))) && // not a character after str
         !insideTemplate && sharpCount==0; // not inside template, because e.g. <const A> is different than <A>, see issue #11663
    if (isMatch)
    {
      size_t i1=target.find('*',i+l);
      size_t i2=target.find('&',i+l);
      if (i1==DString::npos && i2==DString::npos)
      {
        // strip str from target at index i
        target=target.left(i)+target.mid(i+l);
        changed=true;
        i-=l;
      }
      else if ((i1!=DString::npos && i<i1) || (i2!=DString::npos && i<i2)) // str before * or &
      {
        // move str to front
        target=str+" "+target.left(i)+target.mid(i+l);
        changed=true;
        i++;
      }
    }
    p = i+l;
  }
  if (changed) target=target.stripWhiteSpace();
  AUTO_TRACE_EXIT("target='{}'",target,str);
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
void stripIrrelevantConstVolatile(DString &s,bool insideTemplate)
{
  //printf("stripIrrelevantConstVolatile(%s)=",qPrint(s));
  stripIrrelevantString(s,"const",insideTemplate);
  stripIrrelevantString(s,"volatile",insideTemplate);
  stripIrrelevantString(s,"final",insideTemplate);
  //printf("%s\n",qPrint(s));
}


static DString stripDeclKeywords(const DString &s)
{
  size_t i=s.find(" class ");
  if (i!=DString::npos) return s.left(i)+s.mid(i+6);
  i=s.find(" typename ");
  if (i!=DString::npos) return s.left(i)+s.mid(i+9);
  i=s.find(" union ");
  if (i!=DString::npos) return s.left(i)+s.mid(i+6);
  i=s.find(" struct ");
  if (i!=DString::npos) return s.left(i)+s.mid(i+7);
  return s;
}

// forward decl for circular dependencies
static DString extractCanonicalType(const Definition *d,const FileDef *fs,DString type,SrcLangExt lang,bool insideTemplate);

static DString getCanonicalTemplateSpec(const Definition *d,const FileDef *fs,const DString& spec,SrcLangExt lang)
{
  AUTO_TRACE("spec={}",spec);
  DString templSpec = spec.stripWhiteSpace();
  // this part had been commented out before... but it is needed to match for instance
  // std::list<std::string> against list<string> so it is now back again!
  if (!templSpec.empty() && templSpec.at(0) == '<')
  {
    templSpec = "< " + extractCanonicalType(d,fs,templSpec.mid(1).stripWhiteSpace(),lang,true);
  }
  DString resolvedType = lang==SrcLangExt::Java ? templSpec : resolveTypeDef(d,templSpec);
  if (!resolvedType.empty()) // not known as a typedef either
  {
    templSpec = resolvedType;
  }
  //printf("getCanonicalTemplateSpec(%s)=%s\n",qPrint(spec),qPrint(templSpec));
  AUTO_TRACE_EXIT("result={}",templSpec);
  return templSpec;
}


static DString getCanonicalTypeForIdentifier(
    const Definition *d,const FileDef *fs,const DString &word,SrcLangExt lang,
    DString *tSpec,int count=0)
{
  if (count>10) return word; // oops recursion

  DString symName,result,templSpec,tmpName;
  if (tSpec && !tSpec->empty())
    templSpec = stripDeclKeywords(getCanonicalTemplateSpec(d,fs,*tSpec,lang));

  AUTO_TRACE("d='{}' fs='{}' word='{}' templSpec='{}'",d?d->name():"",fs?fs->name():"",word,templSpec);

  if (word.rfind("::")!=DString::npos && !(tmpName=stripScope(word)).empty())
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
  DString ts            = resolver.getTemplateSpec();
  DString resolvedType  = resolver.getResolvedType();

  bool isTemplInst = cd && !templSpec.empty();
  if (!cd && !templSpec.empty())
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
  //    cd ? cd->isTemplate():false,
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
      else if (!ts.empty() && templSpec.empty())
      {
        // use formal template args for spec
        templSpec = stripDeclKeywords(getCanonicalTemplateSpec(d,fs,ts,lang));
      }

      result = removeRedundantWhiteSpace(cd->qualifiedName() + templSpec);

      if (cd->isTemplate() && tSpec) //
      {
        if (!templSpec.empty()) // specific instance
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
      else if (ts.empty() && !templSpec.empty() && cd && !cd->isTemplate() && tSpec)
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
      DString type = mType->typeString();
      if (type.startsWith("typename "))
      {
        type.stripPrefix("typename ");
        type = stripTemplateSpecifiersFromScope(type,false);
      }
      if (!type.empty()) // see issue #11065
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
    if (resolvedType.empty()) // not known as a typedef either
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

static DString extractCanonicalType(const Definition *d,const FileDef *fs,DString type,SrcLangExt lang,bool insideTemplate)
{
  AUTO_TRACE("d={} fs={} type='{}'",d?d->name():"",fs?fs->name():"",type);
  type = type.stripWhiteSpace();

  // strip const and volatile keywords that are not relevant for the type
  stripIrrelevantConstVolatile(type,insideTemplate);

  // strip leading keywords
  type.stripPrefix("class ");
  type.stripPrefix("struct ");
  type.stripPrefix("union ");
  type.stripPrefix("enum ");
  type.stripPrefix("typename ");

  type = removeRedundantWhiteSpace(type);
  //printf("extractCanonicalType(type=%s) start: def=%s file=%s\n",qPrint(type),
  //    d ? qPrint(d->name()) : "<null>", fs ? qPrint(fs->name()) : "<null>");

  DString canType;
  DString templSpec,word;
  int i=0,p=0,pp=0;
  while ((i=extractClassNameFromType(type,p,word,templSpec))!=-1)
    // foreach identifier in the type
  {
    //printf("     i=%d p=%d\n",i,p);
    if (i>pp)
    {
      if (i-pp>=2 && type[i-2]==':' && type[i-1]==':') // skip over leading ::, see issue #12021
      {
        canType += type.mid(pp,i-pp-2);
      }
      else
      {
        canType += type.mid(pp,i-pp);
      }
    }

    DString ct = getCanonicalTypeForIdentifier(d,fs,word,lang,&templSpec);

    // in case the ct is empty it means that "word" represents scope "d"
    // and this does not need to be added to the canonical
    // type (it is redundant), so/ we skip it. This solves problem 589616.
    if (ct.empty() && type.mid(p,2)=="::")
    {
      p+=2;
    }
    else
    {
      canType += ct;
    }
    //printf(" word=%s templSpec=%s canType=%s ct=%s\n",
    //    qPrint(word), qPrint(templSpec), qPrint(canType), qPrint(ct));
    if (!templSpec.empty()) // if we didn't use up the templSpec already
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
        canType += getCanonicalTypeForIdentifier(d,fs,matchStr,lang,nullptr);
        tp=ti+tl;
      }
      canType+=ts.substr(tp);
    }

    pp=p;
  }
  canType += type.mid(pp);
  AUTO_TRACE_EXIT("canType='{}'",canType);

  return removeRedundantWhiteSpace(canType);
}

static DString extractCanonicalArgType(const Definition *d,const FileDef *fs,const Argument &arg,SrcLangExt lang)
{
  DString type = arg.type.stripWhiteSpace();
  DString name = arg.name;
  //printf("----- extractCanonicalArgType(type=%s,name=%s)\n",qPrint(type),qPrint(name));
  if ((type=="const" || type=="volatile") && !name.empty())
  { // name is part of type => correct
    type+=" ";
    type+=name;
  }
  if (name=="const" || name=="volatile")
  { // name is part of type => correct
    if (!type.empty()) type+=" ";
    type+=name;
  }
  if (!arg.array.empty())
  {
    type+=arg.array;
  }

  return extractCanonicalType(d,fs,type,lang,false);
}

static std::mutex g_matchArgsMutex;

// a bit of debug support for matchArguments
//#define MATCH
//#define NOMATCH
//#define MATCH   printf("Match at line %d\n",__LINE__);
//#define NOMATCH printf("Nomatch at line %d\n",__LINE__);
#define MATCH   AUTO_TRACE_EXIT("match at line {}",__LINE__);
#define NOMATCH AUTO_TRACE_EXIT("no match at line {}",__LINE__);

static bool matchCanonicalTypes(
    const Definition *srcScope,const FileDef *srcFileScope,const DString &srcType,
    const Definition *dstScope,const FileDef *dstFileScope,const DString &dstType,
    SrcLangExt lang)
{
  AUTO_TRACE("srcType='{}' dstType='{}'",srcType,dstType);
  if (srcType==dstType) return true;

  // check if the types are function pointers
  size_t i1=srcType.find(")(");
  if (i1==DString::npos) return false;
  size_t i2=dstType.find(")(");
  if (i1!=i2) return false;

  // check if the result part of the function pointer types matches
  size_t j1=srcType.find("(");
  if (j1==DString::npos || j1>i1) return false;
  size_t j2=dstType.find("(");
  if (j2!=j1) return false;
  if (srcType.left(j1)!=dstType.left(j2)) return false; // different return types

  // if srcType and dstType are both function pointers with the same return type,
  // then match against the parameter lists.
  // This way srcType='void (*fptr)(int x)' will match against `void (*fptr)(int y)' because
  // 'int x' matches 'int y'. A simple literal string match would treat these as different.
  auto srcAl = stringToArgumentList(lang,srcType.mid(i1+1));
  auto dstAl = stringToArgumentList(lang,dstType.mid(i2+1));
  return matchArguments2(srcScope,srcFileScope,srcType.left(j1),srcAl.get(),
                         dstScope,dstFileScope,dstType.left(j2),dstAl.get(),
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

  DString sSrcName = " "+srcA.name;
  DString sDstName = " "+dstA.name;
  DString srcType  = srcA.type;
  DString dstType  = dstA.type;
  stripIrrelevantConstVolatile(srcType,false);
  stripIrrelevantConstVolatile(dstType,false);
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
    if (srcA.canType.empty() || dstA.canType.empty())
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
    return true;
  }
  else
  {
    //printf("   Canonical types do not match [%s]<->[%s]\n",
    //    qPrint(srcA->canType),qPrint(dstA->canType));
    NOMATCH
    AUTO_TRACE_EXIT("false");
    return false;
  }
}


// new algorithm for argument matching
bool matchArguments2(const Definition *srcScope,const FileDef *srcFileScope,const DString &srcReturnType,const ArgumentList *srcAl,
                     const Definition *dstScope,const FileDef *dstFileScope,const DString &dstReturnType,const ArgumentList *dstAl,
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
      return true;
    }
    else
    {
      NOMATCH
      return false;
    }
  }

  // handle special case with void argument
  if ( srcAl->empty() && dstAl->size()==1 && dstAl->front().type=="void" )
  { // special case for finding match between func() and func(void)
    Argument a;
    a.type = "void";
    const_cast<ArgumentList*>(srcAl)->push_back(a);
    MATCH
    return true;
  }
  if ( dstAl->empty() && srcAl->size()==1 && srcAl->front().type=="void" )
  { // special case for finding match between func(void) and func()
    Argument a;
    a.type = "void";
    const_cast<ArgumentList*>(dstAl)->push_back(a);
    MATCH
    return true;
  }

  if (srcAl->size() != dstAl->size())
  {
    NOMATCH
    return false; // different number of arguments -> no match
  }

  if (checkCV)
  {
    if (srcAl->constSpecifier() != dstAl->constSpecifier())
    {
      NOMATCH
      return false; // one member is const, the other not -> no match
    }
    if (srcAl->volatileSpecifier() != dstAl->volatileSpecifier())
    {
      NOMATCH
      return false; // one member is volatile, the other not -> no match
    }
  }

  if (srcAl->refQualifier() != dstAl->refQualifier())
  {
    NOMATCH
    return false; // one member is has a different ref-qualifier than the other
  }

  if (srcReturnType=="auto" && dstReturnType=="auto" && srcAl->trailingReturnType()!=dstAl->trailingReturnType())
  {
    NOMATCH
    return false; // one member is has a different return type than the other
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
      return false;
    }
  }
  MATCH
  return true; // all arguments match
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
    if (srcA.defval.empty() && !dstA.defval.empty())
    {
      //printf("Defval changing '%s'->'%s'\n",qPrint(srcA.defval),qPrint(dstA.defval));
      srcA.defval=dstA.defval;
    }
    else if (!srcA.defval.empty() && dstA.defval.empty())
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
      if (srcA.name.empty() && !dstA.name.empty())
      {
        //printf("type: '%s':='%s'\n",qPrint(srcA.type),qPrint(dstA.type));
        //printf("name: '%s':='%s'\n",qPrint(srcA.name),qPrint(dstA.name));
        srcA.type = dstA.type;
        srcA.name = dstA.name;
      }
      else if (!srcA.name.empty() && dstA.name.empty())
      {
        //printf("type: '%s':='%s'\n",qPrint(dstA.type),qPrint(srcA.type));
        //printf("name: '%s':='%s'\n",qPrint(dstA.name),qPrint(srcA.name));
        dstA.type = srcA.type;
        dstA.name = srcA.name;
      }
      else if (!srcA.name.empty() && !dstA.name.empty())
      {
        //printf("srcA.name=%s dstA.name=%s\n",qPrint(srcA.name),qPrint(dstA.name));
        if (forceNameOverwrite)
        {
          srcA.name = dstA.name;
        }
        else
        {
          if (srcA.docs.empty() && !dstA.docs.empty())
          {
            srcA.name = dstA.name;
          }
          else if (!srcA.docs.empty() && dstA.docs.empty())
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
      else if (srcA.name.empty() && !dstA.name.empty())
      {
        srcA.name = dstA.name;
      }
      else if (dstA.name.empty() && !srcA.name.empty())
      {
        dstA.name = srcA.name;
      }
    }
    size_t i1=srcA.type.find("::"),
           i2=dstA.type.find("::"),
           j1=srcA.type.length()-i1-2,
           j2=dstA.type.length()-i2-2;
    if (i1!=DString::npos && i2==DString::npos && srcA.type.right(j1)==dstA.type)
    {
      //printf("type: '%s':='%s'\n",qPrint(dstA.type),qPrint(srcA.type));
      //printf("name: '%s':='%s'\n",qPrint(dstA.name),qPrint(srcA.name));
      dstA.type = srcA.type.left(i1+2)+dstA.type;
      dstA.name = srcA.name;
    }
    else if (i1==DString::npos && i2!=DString::npos && dstA.type.right(j2)==srcA.type)
    {
      //printf("type: '%s':='%s'\n",qPrint(srcA.type),qPrint(dstA.type));
      //printf("name: '%s':='%s'\n",qPrint(dstA.name),qPrint(srcA.name));
      srcA.type = dstA.type.left(i2+2)+srcA.type;
      srcA.name = dstA.name;
    }
    if (srcA.docs.empty() && !dstA.docs.empty())
    {
      srcA.docs = dstA.docs;
    }
    else if (dstA.docs.empty() && !srcA.docs.empty())
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

bool matchTemplateArguments(const ArgumentList &srcAl,const ArgumentList &dstAl)
{
  AUTO_TRACE("srcAl={} dstAl={}",argListToString(srcAl),argListToString(dstAl));
  if (srcAl.size()!=dstAl.size()) // different number of template parameters -> overload
  {
    AUTO_TRACE_EXIT("different number of parameters");
    return false;
  }
  auto isUnconstraintTemplate = [](const DString &type)
  {
    return type=="typename" || type=="class" || type.startsWith("typename ") || type.startsWith("class ");
  };
  auto srcIt = srcAl.begin();
  auto dstIt = dstAl.begin();
  while (srcIt!=srcAl.end() && dstIt!=dstAl.end())
  {
    const Argument &srcA = *srcIt;
    const Argument &dstA = *dstIt;
    if ((!isUnconstraintTemplate(srcA.type) || !isUnconstraintTemplate(dstA.type)) && srcA.type!=dstA.type) // different constraints -> overload
    {
      AUTO_TRACE_EXIT("different constraints");
      return false;
    }
    ++srcIt;
    ++dstIt;
  }
  AUTO_TRACE_EXIT("same");
  // no overload with respect to the template parameters
  return true;
}

//---------------------------------------------------------------------------------------

GetDefResult getDefs(const GetDefInput &input)
{
  GetDefResult result;
  if (input.memberName.empty()) return result;
  AUTO_TRACE("scopeName={},memberName={},forceEmptyScope={}",
      input.scopeName,input.memberName,input.forceEmptyScope);

  //printf("@@ --- getDefsNew(%s,%s)-----------\n",qPrint(scName),qPrint(mbName));
  const Definition *scope = Doxygen::globalScope;
  SymbolResolver resolver;
  if (input.currentFile) resolver.setFileScope(input.currentFile);
  if (!input.scopeName.empty() && !input.forceEmptyScope)
  {
    scope = resolver.resolveSymbol(scope,input.scopeName);
  }
  if (scope==Doxygen::globalScope)
  {
    scope = input.currentFile;
  }
  //printf("@@  -> found scope scope=%s member=%s out=%s\n",qPrint(input.scopeName),qPrint(input.memberName),qPrint(scope?scope->name():""));
  //
  const Definition *symbol = resolver.resolveSymbol(scope,input.memberName,input.args,input.checkCV,input.insideCode,true);
  //printf("@@  -> found symbol in=%s out=%s\n",qPrint(input.memberName),qPrint(symbol?symbol->qualifiedName():DString()));
  if (symbol && symbol->definitionType()==Definition::TypeMember)
  {
    result.md = toMemberDef(symbol);
    result.cd = result.md->getClassDef();
    if (result.cd==nullptr) result.nd = result.md->getNamespaceDef();
    if (result.cd==nullptr && result.nd==nullptr) result.fd = result.md->getFileDef();
    result.gd = result.md->getGroupDef();
    result.found = true;
  }
  else if (symbol && symbol->definitionType()==Definition::TypeClass)
  {
    result.cd = toClassDef(symbol);
    result.found = true;
  }
  else if (symbol && symbol->definitionType()==Definition::TypeNamespace)
  {
    result.nd = toNamespaceDef(symbol);
    result.found = true;
  }
  else if (symbol && symbol->definitionType()==Definition::TypeConcept)
  {
    result.cnd = toConceptDef(symbol);
    result.found = true;
  }
  else if (symbol && symbol->definitionType()==Definition::TypeModule)
  {
    result.modd = toModuleDef(symbol);
    result.found = true;
  }
  return result;
}


/*!
 * Searches for a scope definition given its name as a string via parameter
 * `scope`.
 *
 * The parameter `docScope` is a string representing the name of the scope in
 * which the `scope` string was found.
 *
 * The function returns true if the scope is known and documented or
 * false if it is not.
 * If true is returned exactly one of the parameter `cd`, `nd`
 * will be non-zero:
 *   - if `cd` is non zero, the scope was a class pointed to by cd.
 *   - if `nd` is non zero, the scope was a namespace pointed to by nd.
 */
static bool getScopeDefs(const DString &docScope,const DString &scope,
    ClassDef *&cd, ConceptDef *&cnd, NamespaceDef *&nd,ModuleDef *&modd)
{
  cd=nullptr;
  cnd=nullptr;
  nd=nullptr;
  modd=nullptr;

  DString scopeName=scope;
  //printf("getScopeDefs: docScope='%s' scope='%s'\n",qPrint(docScope),qPrint(scope));
  if (scopeName.empty()) return false;

  bool explicitGlobalScope=false;
  if (scopeName.at(0)==':' && scopeName.at(1)==':')
  {
    scopeName=scopeName.mid(2);
    explicitGlobalScope=true;
  }
  if (scopeName.empty())
  {
    return false;
  }

  DString docScopeName=docScope;
  int scopeOffset=explicitGlobalScope ? 0 : static_cast<int>(docScopeName.length());

  do // for each possible docScope (from largest to and including empty)
  {
    DString fullName=scopeName;
    if (scopeOffset>0) fullName.prepend(docScopeName.left(scopeOffset)+"::");

    if (((cd=getClass(fullName)) ||         // normal class
         (cd=getClass(fullName+"-p"))       // ObjC protocol
        ) && cd->isLinkable())
    {
      return true; // class link written => quit
    }
    else if ((nd=Doxygen::namespaceLinkedMap->find(fullName)) && nd->isLinkable())
    {
      return true; // namespace link written => quit
    }
    else if ((cnd=Doxygen::conceptLinkedMap->find(fullName)) && cnd->isLinkable())
    {
      return true; // concept link written => quit
    }
    else if ((modd=ModuleManager::instance().modules().find(fullName)) && modd->isLinkable())
    {
      return true; // module link written => quit
    }
    if (scopeOffset==0)
    {
      scopeOffset=-1;
    }
    else
    {
      size_t o = docScopeName.rfind("::",scopeOffset-1);
      scopeOffset = o!=DString::npos ? static_cast<int>(o) : 0;
    }
  } while (scopeOffset>=0);

  return false;
}

static bool isLowerCase(DString &s)
{
  if (s.empty()) return true;
  const char *p=s.data();
  int c=0;
  while ((c=static_cast<uint8_t>(*p++))) if (!islower(c)) return false;
  return true;
}

bool resolveRef(/* in */  const DString &scName,
    /* in */  const DString &name,
    /* in */  bool inSeeBlock,
    /* out */ const Definition **resContext,
    /* out */ const MemberDef  **resMember,
    /* in */ SrcLangExt lang,
    bool lookForSpecialization,
    const FileDef *currentFile,
    bool checkScope
    )
{
  AUTO_TRACE("scope={} name={} inSeeBlock={} lang={} lookForSpecialization={} currentFile={} checkScope={}",
      scName,name,inSeeBlock,lang,lookForSpecialization,currentFile ? currentFile->name() : "", checkScope);
  //printf("resolveRef(scope=%s,name=%s,inSeeBlock=%d)\n",qPrint(scName),qPrint(name),inSeeBlock);
  DString tsName = name;
  //bool memberScopeFirst = tsName.find('#')!=-1;
  DString fullName = substitute(tsName,"#","::");
  if (fullName.find("anonymous_namespace{")==DString::npos)
  {
    fullName = removeRedundantWhiteSpace(substitute(fullName,".","::",3));
  }
  else
  {
    fullName = removeRedundantWhiteSpace(fullName);
  }

  size_t templStartPos;
  if (lang==SrcLangExt::CSharp && (templStartPos=fullName.find('<'))!=DString::npos)
  {
    size_t templEndPos = fullName.rfind('>');
    if (templEndPos!=DString::npos)
    {
      fullName = mangleCSharpGenericName(fullName.left(templEndPos+1))+fullName.mid(templEndPos+1);
      AUTO_TRACE_ADD("C# mangled name='{}'",fullName);
    }
  }

  size_t bracePos   = findParameterList(fullName);
  size_t endNamePos = bracePos!=DString::npos ? bracePos : fullName.length();
  size_t scopePos   = fullName.rfind("::",endNamePos);
  bool explicitScope = fullName.startsWith("::") &&   // ::scope or #scope
                       ((scopePos!=DString::npos && scopePos>2) || // ::N::A
                        tsName.startsWith("::") ||    // ::foo in local scope
                        scName==nullptr               // #foo  in global scope
                       );
  bool allowTypeOnly=false;

  // default result values
  *resContext=nullptr;
  *resMember=nullptr;

  if (bracePos==DString::npos) // simple name
  {
    // the following if() was commented out for releases in the range
    // 1.5.2 to 1.6.1, but has been restored as a result of bug report 594787.
    if (!inSeeBlock && scopePos==DString::npos && isLowerCase(tsName))
    { // link to lower case only name => do not try to autolink
      AUTO_TRACE_ADD("false");
      return false;
    }

    ClassDef *cd=nullptr;
    NamespaceDef *nd=nullptr;
    ConceptDef *cnd=nullptr;
    ModuleDef *modd=nullptr;

    //printf("scName=%s fullName=%s\n",qPrint(scName),qPrint(fullName));

    // check if this is a class or namespace reference
    if (scName!=fullName && getScopeDefs(scName,fullName,cd,cnd,nd,modd))
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
      else if (modd)
      {
        *resContext = modd;
      }
      else // scope matches that of a namespace
      {
        ASSERT(nd!=nullptr);
        *resContext = nd;
      }
      AUTO_TRACE_ADD("true");
      return true;
    }
    else if (scName==fullName || (!inSeeBlock && scopePos==DString::npos))
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
  DString nameStr=fullName.left(endNamePos);
  if (explicitScope) nameStr=nameStr.mid(2);


  // extract arguments
  DString argsStr;
  if (bracePos!=DString::npos) argsStr=fullName.mid(bracePos);

  // strip template specifier
  // TODO: match against the correct partial template instantiation
  size_t templPos = nameStr.find('<');
  bool tryUnspecializedVersion = false;
  if (templPos!=DString::npos && nameStr.find("operator")==DString::npos)
  {
    size_t endTemplPos=nameStr.rfind('>');
    if (endTemplPos!=DString::npos)
    {
      if (!lookForSpecialization)
      {
        nameStr=nameStr.left(templPos)+nameStr.mid(endTemplPos+1);
      }
      else
      {
        tryUnspecializedVersion = true;
      }
    }
  }

  DString scopeStr=scName;
  if (!explicitScope && nameStr.length()>scopeStr.length() && leftScopeMatch(scopeStr,nameStr))
  {
    nameStr=nameStr.mid(scopeStr.length()+2);
  }

  const GroupDef     *gd = nullptr;
  const ConceptDef   *cnd = nullptr;
  const ModuleDef    *modd = nullptr;

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
    size_t np = nameStr.find("::");
    if (checkScope && result.md && result.md->getOuterScope()==Doxygen::globalScope &&
        !result.md->isStrongEnumValue() &&
        (!scopeStr.empty() || (np!=DString::npos && np>0)))
    {
      // we did find a member, but it is a global one while we were explicitly
      // looking for a scoped variable. See bug 616387 for an example why this check is needed.
      // note we do need to support autolinking to "::symbol" hence the >0
      //printf("not global member!\n");
      *resContext=nullptr;
      *resMember=nullptr;
      AUTO_TRACE_ADD("false");
      return false;
    }
    //printf("after getDefs md=%p cd=%p fd=%p nd=%p gd=%p\n",md,cd,fd,nd,gd);
    if (result.md)
    {
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
        return false;
      }
    }
    else if (result.cd)   *resContext=result.cd;
    else if (result.nd)   *resContext=result.nd;
    else if (result.fd)   *resContext=result.fd;
    else if (result.gd)   *resContext=result.gd;
    else if (result.cnd)  *resContext=result.cnd;
    else if (result.modd) *resContext=result.modd;
    else
    {
      *resContext=nullptr; *resMember=nullptr;
        AUTO_TRACE_ADD("false");
      return false;
    }
    //printf("member=%s (md=%p) anchor=%s linkable()=%d context=%s\n",
    //    qPrint(md->name()), md, qPrint(md->anchor()), md->isLinkable(), qPrint((*resContext)->name()));
    AUTO_TRACE_ADD("true");
    return true;
  }
  else if (inSeeBlock && !nameStr.empty() && (gd=Doxygen::groupLinkedMap->find(nameStr)))
  { // group link
    *resContext=gd;
    AUTO_TRACE_ADD("true");
    return true;
  }
  else if ((cnd=Doxygen::conceptLinkedMap->find(nameStr)))
  {
    *resContext=cnd;
    AUTO_TRACE_ADD("true");
    return true;
  }
  else if ((modd=ModuleManager::instance().modules().find(nameStr)))
  {
    *resContext=modd;
    AUTO_TRACE_ADD("true");
    return true;
  }
  else if (tsName.find('.')!=DString::npos) // maybe a link to a file
  {
    bool ambig = false;
    const FileDef *fd=Doxygen::inputNameLinkedMap->findFileDef(tsName,ambig);
    if (fd && !ambig)
    {
      *resContext=fd;
      AUTO_TRACE_ADD("true");
      return true;
    }
  }

  if (tryUnspecializedVersion)
  {
    bool b = resolveRef(scName,name,inSeeBlock,resContext,resMember,lang,false,nullptr,checkScope);
    AUTO_TRACE_ADD("{}",b);
    return b;
  }
  if (bracePos!=DString::npos) // Try without parameters as well, could be a constructor invocation
  {
    *resContext=getClass(fullName.left(bracePos));
    if (*resContext)
    {
      AUTO_TRACE_ADD("true");
      return true;
    }
  }
  //printf("resolveRef: %s not found!\n",qPrint(name));

  AUTO_TRACE_ADD("false");
  return false;
}

DString linkToText(SrcLangExt lang,const DString &link,bool ignoreDots)
{
  //bool optimizeOutputJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  DString result=link;
  if (!result.empty())
  {
    // replace # by ::
    result=substitute(result,"#","::");
    // replace . by ::
    if (!ignoreDots && result.find('<')==DString::npos) result=substitute(result,".","::",3);
    // strip leading :: prefix if present
    if (result.at(0)==':' && result.at(1)==':')
    {
      result=result.mid(2);
    }
    DString sep = getLanguageSpecificSeparator(lang);
    if (sep!="::")
    {
      result=substitute(result,"::",sep);
    }
  }
  //printf("linkToText(%s,lang=%d)=%s\n",qPrint(link),lang,qPrint(result));
  return result;
}

static const DirDef *resolveDirLink(const DString &linkRef)
{
  const DirDef *dd = Doxygen::dirLinkedMap->find(FileInfo(linkRef.str()).absFilePath()+"/");
  //printf("resolveDirLink(%s) -> %s\n",qPrint(linkRef),dd?qPrint(dd->name()):"<none>");
  if (dd==nullptr)
  {
    StringVector stripPaths = Config_getList(STRIP_FROM_PATH);
    for (const auto &path : stripPaths)
    {
      FileInfo fi(path+linkRef.str());
      //printf("  trying to strip path '%s' from linkRef '%s' fi='%s'\n",qPrint(path),qPrint(linkRef),qPrint(fi.absFilePath()));
      dd = Doxygen::dirLinkedMap->find(fi.absFilePath()+"/");
      if (dd) break;
    }
  }
  return dd;
}

bool resolveLink(/* in */ const DString &scName,
    /* in */ const DString &lr,
    /* in */ bool /*inSeeBlock*/,
    /* out */ const Definition **resContext,
    /* out */ DString &resAnchor,
    /* in */ SrcLangExt lang,
    /* in */ const DString &prefix
    )
{
  *resContext=nullptr;

  DString linkRef=lr;
  if (lang==SrcLangExt::CSharp)
  {
    linkRef = mangleCSharpGenericName(linkRef);
  }
  DString linkRefWithoutTemplates = stripTemplateSpecifiersFromScope(linkRef,false);
  AUTO_TRACE("scName='{}',ref='{}'",scName,lr);
  const FileDef  *fd = nullptr;
  const GroupDef *gd = nullptr;
  const PageDef  *pd = nullptr;
  const ClassDef *cd = nullptr;
  const DirDef   *dir = nullptr;
  const ConceptDef *cnd = nullptr;
  const ModuleDef *modd = nullptr;
  const NamespaceDef *nd = nullptr;
  const SectionInfo *si = nullptr;
  bool ambig = false;
  if (linkRef.empty()) // no reference name!
  {
    AUTO_TRACE_EXIT("no_ref");
    return false;
  }
  else if ((pd=Doxygen::pageLinkedMap->find(linkRef))) // link to a page
  {
    gd = pd->getGroupDef();
    if (gd)
    {
      if (!pd->name().empty()) si=SectionManager::instance().find(pd->name());
      *resContext=gd;
      if (si) resAnchor = si->label();
    }
    else
    {
      *resContext=pd;
    }
    AUTO_TRACE_EXIT("page");
    return true;
  }
  else if ((si=SectionManager::instance().find(prefix+linkRef)))
  {
    *resContext=si->definition();
    resAnchor = si->label();
    AUTO_TRACE_EXIT("section anchor={} def={}",resAnchor,si->definition()?si->definition()->name():"<none>");
    return true;
  }
  else if (!prefix.empty() && (si=SectionManager::instance().find(linkRef)))
  {
    *resContext=si->definition();
    resAnchor = si->label();
    AUTO_TRACE_EXIT("section anchor={} def={}",resAnchor,si->definition()?si->definition()->name():"<none>");
    return true;
  }
  else if ((pd=Doxygen::exampleLinkedMap->find(linkRef))) // link to an example
  {
    *resContext=pd;
    AUTO_TRACE_EXIT("example");
    return true;
  }
  else if ((gd=Doxygen::groupLinkedMap->find(linkRef))) // link to a group
  {
    *resContext=gd;
    AUTO_TRACE_EXIT("group");
    return true;
  }
  else if ((fd=Doxygen::inputNameLinkedMap->findFileDef(linkRef,ambig)) // file link
      && fd->isLinkable())
  {
    *resContext=fd;
    AUTO_TRACE_EXIT("file");
    return true;
  }
  else if ((cd=getClass(linkRef))) // class link
  {
    *resContext=cd;
    resAnchor=cd->anchor();
    AUTO_TRACE_EXIT("class");
    return true;
  }
  else if (lang==SrcLangExt::Java &&
           (cd=getClass(linkRefWithoutTemplates))) // Java generic class link
  {
    *resContext=cd;
    resAnchor=cd->anchor();
    AUTO_TRACE_EXIT("generic");
    return true;
  }
  else if ((cd=getClass(linkRef+"-p"))) // Obj-C protocol link
  {
    *resContext=cd;
    resAnchor=cd->anchor();
    AUTO_TRACE_EXIT("protocol");
    return true;
  }
  else if ((cnd=getConcept(linkRef))) // C++20 concept definition
  {
    *resContext=cnd;
    resAnchor=cnd->anchor();
    AUTO_TRACE_EXIT("concept");
    return true;
  }
  else if ((modd=ModuleManager::instance().modules().find(linkRef)))
  {
    *resContext=modd;
    resAnchor=modd->anchor();
    AUTO_TRACE_EXIT("module");
    return true;
  }
  else if ((nd=Doxygen::namespaceLinkedMap->find(linkRef)))
  {
    *resContext=nd;
    AUTO_TRACE_EXIT("namespace");
    return true;
  }
  else if ((dir=resolveDirLink(linkRef)) && dir->isLinkable())
  {
    *resContext=dir;
    AUTO_TRACE_EXIT("directory");
    return true;
  }
  else // probably a member reference
  {
    const MemberDef *md = nullptr;
    bool res = resolveRef(scName,lr,true,resContext,&md,lang);
    if (md) resAnchor=md->anchor();
    AUTO_TRACE_EXIT("member? res={}",res);
    return res;
  }
}

//----------------------------------------------------------------------


DString findExampleFilePath(const DString &file,bool &ambig)
{
  ambig=false;
  DString result;
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
    StringVector examplePathList = Config_getList(EXAMPLE_PATH);
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
    FileDef *fd = Doxygen::exampleNameLinkedMap->findFileDef(file,ambig);
    if (fd && !ambig)
    {
      result=fd->absFilePath();
    }
  }
  return result;
}

//----------------------------------------------------------------------

DString substituteKeywords(const DString &file, const DString &s,const KeywordSubstitutionList &keywords)
{
  std::string substRes;
  int line = 1;
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
          size_t keyLen = dstrlen(kw.keyword);
          if (dstrncmp(p,kw.keyword,keyLen)==0)
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
                DString value = DString(startArg+1).left(endArg-startArg-1);
                auto &&getValue = std::get<KeywordSubstitution::GetValueWithParam>(kw.getValueVariant);
                substRes+=getValue(value).str();
                p=endArg+1;
                //printf("found '%s'->'%s'\n",kw.keyword,qPrint(getValue(value)));
              }
              else
              {
                //printf("missing argument\n");
                warn(file,line,"Missing argument for '{}'",kw.keyword);
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
            else
            {
              //printf("%s %d Expected arguments, none specified '%s'\n",qPrint(file), line, qPrint(kw.keyword));
              warn(file,line,"Expected arguments for '{}' but none were specified",kw.keyword);
              p+=keyLen;
            }
            found = true;
            break;
          }
        }
      }
      if (!found) // copy
      {
        if (c=='\n') line++;
        substRes+=c;
        p++;
      }
    }
  }
  return substRes;
}

DString showDate(const DString &fmt)
{
   // get the current date and time
  std::tm dat{};
  int specFormat=0;
  DString specDate = "";
  DString err = dateTimeFromString(specDate,dat,specFormat);

  // do the conversion
  int usedFormat=0;
  return formatDateTime(fmt,dat,usedFormat);
}

DString projectLogoFile()
{
  DString projectLogo = Config_getString(PROJECT_LOGO);
  if (!projectLogo.empty())
  {
    // check for optional width= and height= specifier
    if (size_t wi = projectLogo.find(" width="); wi!=DString::npos) // and strip them
    {
      projectLogo = projectLogo.left(wi);
    }
    if (size_t hi = projectLogo.find(" height="); hi!=DString::npos)
    {
      projectLogo = projectLogo.left(hi);
    }
  }
  //printf("projectlogo='%s'\n",qPrint(projectLogo));
  return projectLogo;
}

DString projectLogoSize()
{
  DString sizeVal;
  DString projectLogo = Config_getString(PROJECT_LOGO);
  if (!projectLogo.empty())
  {
    auto extractDimension = [&projectLogo](const char *startMarker,size_t startPos,size_t endPos) -> DString
    {
      DString result = projectLogo.mid(startPos,endPos-startPos).stripWhiteSpace().quoted();
      if (result.length()>=2 && result.at(0)!='"' && result.at(result.length()-1)!='"')
      {
        result="\""+result+"\"";
      }
      result.prepend(startMarker);
      return result;
    };
    // check for optional width= and height= specifier
    size_t wi = projectLogo.find(" width=");
    size_t hi = projectLogo.find(" height=");
    if (wi!=DString::npos && hi!=DString::npos)
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
    else if (wi!=DString::npos) // ... width=x..."
    {
      sizeVal = extractDimension(" width=", wi+7, projectLogo.length());
    }
    else if (hi!=DString::npos) // ... height=x..."
    {
      sizeVal = extractDimension(" height=", hi+8, projectLogo.length());
    }
  }
  //printf("projectsize='%s'\n",qPrint(sizeVal));
  return sizeVal;
}


//----------------------------------------------------------------------

/*! Returns the character index within \a name of the first prefix
 *  in Config_getList(IGNORE_PREFIX) that matches \a name at the left hand side,
 *  or zero if no match was found
 */
int getPrefixIndex(const DString &name)
{
  if (name.empty()) return 0;
  int result=0;
  StringVector sl = Config_getList(IGNORE_PREFIX);
  for (const auto &s : sl)
  {
    const char *ps=s.c_str();
    const char *pd=name.data();
    int i=0;
    while (*ps!=0 && *pd!=0 && *ps==*pd)
    {
      ps++;
      pd++;
      i++;
    }
    if (*ps==0 && *pd!=0)
    {
      result=i;
      break;
    }
  }
  if (result<static_cast<int>(name.length())-1 && name.at(result)=='[') result++; // for e.g. [union] return u
  return result;
}

//----------------------------------------------------------------------------

bool useCaseSenseNames()
{
  auto caseSenseNames = Config_getEnum(CASE_SENSE_NAMES);

  if (caseSenseNames == CASE_SENSE_NAMES_t::YES) return true;
  else if (caseSenseNames == CASE_SENSE_NAMES_t::NO) return false;
  else return Portable::fileSystemIsCaseSensitive();
}

DString escapeCharsInString(const DString &name,bool allowDots,bool allowUnderscore)
{
  if (name.empty()) return name;
  bool caseSenseNames = useCaseSenseNames();
  bool allowUnicodeNames = Config_getBool(ALLOW_UNICODE_NAMES);
  DString result;
  result.reserve(name.length()+8);
  signed char c = 0;
  const char *p=name.data();
  while ((c=*p++)!=0)
  {
    switch(c)
    {
      case '_': if (allowUnderscore) result+='_'; else result+="__"; break;
      case '-': result+='-';  break;
      case ':': result+="_1"; break;
      case '/': result+="_2"; break;
      case '<': result+="_3"; break;
      case '>': result+="_4"; break;
      case '*': result+="_5"; break;
      case '&': result+="_6"; break;
      case '|': result+="_7"; break;
      case '.': if (allowDots) result+='.'; else result+="_8"; break;
      case '!': result+="_9"; break;
      case ',': result+="_00"; break;
      case ' ': result+="_01"; break;
      case '{': result+="_02"; break;
      case '}': result+="_03"; break;
      case '?': result+="_04"; break;
      case '^': result+="_05"; break;
      case '%': result+="_06"; break;
      case '(': result+="_07"; break;
      case ')': result+="_08"; break;
      case '+': result+="_09"; break;
      case '=': result+="_0a"; break;
      case '$': result+="_0b"; break;
      case '\\': result+="_0c"; break;
      case '@': result+="_0d"; break;
      case ']': result+="_0e"; break;
      case '[': result+="_0f"; break;
      case '#': result+="_0g"; break;
      case '"': result+="_0h"; break;
      case '~': result+="_0i"; break;
      case '\'': result+="_0j"; break;
      case ';': result+="_0k"; break;
      case '`': result+="_0l"; break;
      default:
                if (c<0)
                {
                  bool doEscape = true;
                  if (allowUnicodeNames)
                  {
                    int charLen = getUTF8CharNumBytes(c);
                    if (charLen>0)
                    {
                      result+=DString(p-1,charLen);
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
                    result+=ids;
                  }
                }
                else if (caseSenseNames || !isupper(c))
                {
                  result+=c;
                }
                else
                {
                  result+='_';
                  result+=static_cast<char>(tolower(c));
                }
                break;
    }
  }
  return result;
}

DString unescapeCharsInString(const DString &s)
{
  if (s.empty()) return s;
  bool caseSenseNames = useCaseSenseNames();
  DString result;
  result.reserve(s.length());
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
DString convertNameToFile(const DString &name,bool allowDots,bool allowUnderscore)
{
  if (name.empty()) return name;
  bool shortNames = Config_getBool(SHORT_NAMES);
  bool createSubdirs = Config_getBool(CREATE_SUBDIRS);
  DString result;
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
      result=result.left(128-32)+md5str(result.view());
    }
  }
  if (createSubdirs)
  {
    int l1Dir=0,l2Dir=0;
    int createSubdirsLevel = Config_getInt(CREATE_SUBDIRS_LEVEL);
    int createSubdirsBitmaskL2 = (1<<createSubdirsLevel)-1;

    // compute md5 hash to determine sub directory to use
    auto md5_sig = md5hash(result.view());
    l1Dir = md5_sig[14] & 0xf;
    l2Dir = md5_sig[15] & createSubdirsBitmaskL2;

    result.prepend(DString().sprintf("d%x/d%02x/",l1Dir,l2Dir));
  }
  //printf("*** convertNameToFile(%s)->%s\n",qPrint(name),qPrint(result));
  return result;
}

DString relativePathToRoot(const DString &name)
{
  DString result;
  if (Config_getBool(CREATE_SUBDIRS))
  {
    if (name.empty())
    {
      return REL_PATH_TO_ROOT;
    }
    else if (size_t i = name.rfind('/'); i!=DString::npos)
    {
      result=REL_PATH_TO_ROOT;
    }
  }
  return result;
}

DString determineAbsoluteIncludeName(const DString &curFile,const DString &incFileName)
{
  bool searchIncludes = Config_getBool(SEARCH_INCLUDES);
  DString absIncFileName = incFileName;
  FileInfo fi(curFile.str());
  if (fi.exists())
  {
    DString absName = fi.dirPath(true)+"/"+incFileName;
    FileInfo fi2(absName.str());
    if (fi2.exists())
    {
      absIncFileName=fi2.absFilePath();
    }
    else if (searchIncludes) // search in INCLUDE_PATH as well
    {
      StringVector includePath = Config_getList(INCLUDE_PATH);
      for (const auto &incPath : includePath)
      {
        FileInfo fi3(incPath);
        if (fi3.exists() && fi3.isDir())
        {
          absName = fi3.absFilePath()+"/"+incFileName;
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
      DString subdir;
      subdir.sprintf("d%x",l1);
      if (!d.exists(subdir.str()) && !d.mkdir(subdir.str()))
      {
        term("Failed to create output directory '{}'\n",subdir);
      }
      for (int l2=0; l2<createSubdirsLevelPow2; l2++)
      {
        DString subsubdir;
        subsubdir.sprintf("d%x/d%02x",l1,l2);
        if (!d.exists(subsubdir.str()) && !d.mkdir(subsubdir.str()))
        {
          term("Failed to create output directory '{}'\n",subsubdir);
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
      DString subdir;
      subdir.sprintf("d%x",l1);
      for (int l2=0; l2 < createSubdirsLevelPow2; l2++)
      {
        DString subsubdir;
        subsubdir.sprintf("d%x/d%02x",l1,l2);
        if (d.exists(subsubdir.str()) && d.empty(subsubdir.str()))
        {
          d.rmdir(subsubdir.str());
        }
      }
      if (d.exists(subdir.str()) && d.empty(subdir.str()))
      {
        d.rmdir(subdir.str());
      }
    }
  }
}

/*! Input is a scopeName, output is the scopename split into a
 *  namespace part (as large as possible) and a classname part.
 */
void extractNamespaceName(const DString &scopeName,
    DString &className,DString &namespaceName,
    bool allowEmptyClass)
{
  DString clName=scopeName;
  NamespaceDef *nd = nullptr;
  size_t i=0;
  int p=0;
  if (!clName.empty() && (nd=getResolvedNamespace(clName)) && getClass(clName)==nullptr)
  { // the whole name is a namespace (and not a class)
    namespaceName=nd->name();
    className.clear();
    goto done;
  }
  p=static_cast<int>(clName.length())-2;
  while (p>=0 && (i=clName.rfind("::",p))!=DString::npos)
    // see if the first part is a namespace (and not a class)
  {
    //printf("Trying %s\n",qPrint(clName.left(i)));
    if (i>0 && (nd=getResolvedNamespace(clName.left(i))) && getClass(clName.left(i))==nullptr)
    {
      //printf("found!\n");
      namespaceName=nd->name();
      className=clName.mid(i+2);
      goto done;
    }
    p=static_cast<int>(i)-2; // try a smaller piece of the scope
  }
  //printf("not found!\n");

  // not found, so we just have to guess.
  className=scopeName;
  namespaceName.clear();

done:
  if (className.empty() && !namespaceName.empty() && !allowEmptyClass)
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

DString insertTemplateSpecifierInScope(const DString &scope,const DString &templ)
{
  DString result=scope;
  if (!templ.empty() && scope.find('<')==DString::npos)
  {
    size_t si=0, pi=0;
    ClassDef *cd=nullptr;
    while (
        (si=scope.find("::",pi))!=DString::npos && !getClass(scope.left(si)+templ) &&
        ((cd=getClass(scope.left(si)))==nullptr || cd->templateArguments().empty())
        )
    {
      //printf("Tried '%s'\n",qPrint((scope.left(si)+templ)));
      pi=si+2;
    }
    if (si==DString::npos) // not nested => append template specifier
    {
      result+=templ;
    }
    else // nested => insert template specifier before after first class name
    {
      result=scope.left(si) + templ + scope.mid(si);
    }
  }
  //printf("insertTemplateSpecifierInScope('%s','%s')=%s\n",
  //    qPrint(scope),qPrint(templ),qPrint(result));
  return result;
}


/*! Strips the scope from a name. Examples: A::B will return A
 *  and A<T>::B<N::C<D> > will return A<T>.
 */
DString stripScope(const DString &name)
{
  DString result = name;
  int l = static_cast<int>(result.length());
  int p = 0;
  bool done = false;
  bool skipBracket=false; // if brackets do not match properly, ignore them altogether
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
    skipBracket=true;
  }
  while (!done); // if < > unbalanced repeat ignoring them
  //printf("stripScope(%s)=%s\n",name,name);
  return name;
}

/*! Converts a string to a HTML id string */
DString convertToId(const DString &s)
{
  if (s.empty()) return s;
  DString result;
  result.reserve(s.length()+8);
  const char *p = s.data();
  char c        = 0;
  bool first    = true;
  while ((c=*p++))
  {
    char encChar[4];
    if ((c>='0' && c<='9') || (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='-')
    { // any permissive character except _
      if (first && c>='0' && c<='9') result+='a'; // don't start with a digit
      result+=c;
    }
    else
    {
      encChar[0]='_';
      encChar[1]=hex[static_cast<unsigned char>(c)>>4];
      encChar[2]=hex[static_cast<unsigned char>(c)&0xF];
      encChar[3]=0;
      result+=encChar;
    }
    first=false;
  }
  return result;
}

/*! Converts a string to an XML-encoded string */
DString convertToXML(const DString &s, bool keepEntities, const bool citeEntry)
{
  if (s.empty()) return s;
  DString result;
  result.reserve(s.length()+32);
  const char *p = s.data();
  char c = 0;
  while ((c=*p++))
  {
    switch (c)
    {
      case '<':  result+="&lt;";   break;
      case '>':  result+="&gt;";   break;
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
                     result+=c;
                     while (p<e) result+=*p++;
                   }
                   else
                   {
                     result+="&amp;";
                   }
                 }
                 else if (citeEntry)
                 {
                   p = HtmlEntityMapper::instance().writeHtmlEntity(
                       result,
                       p-1,
                       [](HtmlEntityMapper::SymType symType) { return HtmlEntityMapper::instance().xml(symType); },
                       "&amp;");
                 }
                 else
                 {
                   result+="&amp;";
                 }
                 break;
      case '\'': result+="&apos;"; break;
      case '"':  result+="&quot;"; break;
      case  1: case  2: case  3: case  4: case  5: case  6: case  7: case  8:
      case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
      case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
      case 27: case 28: case 29: case 30: case 31:
        break; // skip invalid XML characters (see http://www.w3.org/TR/2000/REC-xml-20001006#NT-Char)
      default:   result+=c;       break;
    }
  }
  return result;
}

/*! Converts a string to a HTML-encoded string */
DString convertToHtml(const DString &s,bool keepEntities)
{
  if (s.empty()) return s;
  DString result;
  result.reserve(s.length()+32);
  const char *p=s.data();
  char c = 0;
  while ((c=*p++))
  {
    switch (c)
    {
      case '<':  result+="&lt;";   break;
      case '>':  result+="&gt;";   break;
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
                     result+=c;
                     while (p<e) result+=*p++;
                   }
                   else
                   {
                     result+="&amp;";
                   }
                 }
                 else
                 {
                   result+="&amp;";
                 }
                 break;
      case '\'': result+="&#39;";  break;
      case '"':  result+="&quot;"; break;
      default:
        {
          uint8_t uc = static_cast<uint8_t>(c);
          if (uc<32 && !isspace(c))
          {
            result+="&#x24";
            result+=hex[uc>>4];
            result+=hex[uc&0xF];
            result+=';';
          }
          else
          {
            result+=c;
          }
        }
        break;
    }
  }
  return result;
}

DString convertToJSString(const DString &s,bool keepEntities,bool singleQuotes)
{
  if (s.empty()) return s;
  DString result;
  result.reserve(s.length()+32);
  const char *p=s.data();
  char c = 0;
  while ((c=*p++))
  {
    switch (c)
    {
      case '"':  if (!singleQuotes) result+="\\\""; else result+=c;
                 break;
      case '\'': if (singleQuotes) result+="\\\'"; else result+=c;
                 break;
      case '\\': if (*p=='u' && *(p+1)=='{') result+="\\"; // keep \u{..} unicode escapes
                 else result+="\\\\";
                 break;
      default:   result+=c;   break;
    }
  }
  return keepEntities ? result : HtmlEntityMapper::instance().convertCharEntitiesToUTF8(result);
}

/*! Extracts a (sub-)string from \a type starting at \a pos that
 *  could form a class. The index of the match is returned and the found
 *  class \a name and a template argument list \a templSpec. If -1 is returned
 *  there are no more matches.
 */
int extractClassNameFromType(const DString &type,int &pos,DString &name,DString &templSpec,SrcLangExt lang)
{
  AUTO_TRACE("type='{}' pos={} name='{}' lang={}",type,pos,name,lang);
  static const reg::Ex re_norm(R"(\a[\w:]*)");
  static const reg::Ex re_fortran(R"(\a[\w:()=]*)");
  const reg::Ex *re = &re_norm;

  name.clear();
  templSpec.clear();
  if (type.empty())
  {
    AUTO_TRACE_EXIT("empty type");
    return -1;
  }
  size_t typeLen=type.length();
  if (typeLen>0)
  {
    if (lang == SrcLangExt::Fortran)
    {
      if (type[pos]==',')
      {
        AUTO_TRACE_EXIT("comma");
        return -1;
      }
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

      while (ts<typeLen && type[static_cast<uint32_t>(ts)]==' ') { ts++; tl++; } // skip any whitespace
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
        templSpec = DString(type).mid(ts,te-ts);
        tl+=te-ts;
        pos=static_cast<int>(i+l+tl);
      }
      else // no template part
      {
        pos=static_cast<int>(i+l);
      }
      //printf("extractClassNameFromType([in] type=%s,[out] pos=%d,[out] name=%s,[out] templ=%s)=true i=%d\n",
      //    qPrint(type),pos,qPrint(name),qPrint(templSpec),i);
      AUTO_TRACE_EXIT("pos={} templSpec='{}' return={}",pos,templSpec,i);
      return static_cast<int>(i);
    }
  }
  pos = static_cast<int>(typeLen);
  //printf("extractClassNameFromType([in] type=%s,[out] pos=%d,[out] name=%s,[out] templ=%s)=false\n",
  //       qPrint(type),pos,qPrint(name),qPrint(templSpec));
  AUTO_TRACE_EXIT("not found");
  return -1;
}

DString normalizeNonTemplateArgumentsInString(
       const DString &name,
       const Definition *context,
       const ArgumentList &formalArgs)
{
  // skip until <
  size_t p=name.find('<');
  if (p==DString::npos) return name;
  p++;
  DString result = name.left(p);

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
    DString n(match.str());
    bool found=false;
    for (const Argument &formArg : formalArgs)
    {
      if (formArg.name == n)
      {
        found=true;
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


DString substituteTemplateArgumentsInString(
    const DString &nm,
    const ArgumentList &formalArgs,
    const ArgumentList *actualArgs)
{
  AUTO_TRACE("name={} formalArgs={} actualArgs={}",nm,argListToString(formalArgs),actualArgs ? argListToString(*actualArgs) : DString());
  if (formalArgs.empty()) return nm;
  DString result;

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
    DString n(match.str());
    ArgumentList::const_iterator actIt;
    if (actualArgs)
    {
      actIt = actualArgs->begin();
    }
    //printf(": name=%s\n",qPrint(name));

    // if n is a template argument, then we substitute it
    // for its template instance argument.
    bool found=false;
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
      if (formArg.type.startsWith("class ") && formArg.name.empty())
      {
        formArg.name = formArg.type.mid(6);
        formArg.type = "class";
      }
      else if (formArg.type.startsWith("typename ") && formArg.name.empty())
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
        if (formArg.name==n && actualArgs && actIt!=actualArgs->end() && !actArg.type.empty()) // base class is a template argument
        {
          static constexpr auto hasRecursion = [](const DString &prefix,const DString &nameArg,const DString &subst) -> bool
          {
            size_t ii=0;
            size_t pp=0;

            ii = subst.find('<');
            //printf("prefix='%s' subst='%s'\n",qPrint(prefix.mid(prefix.length()-ii-2,ii+1)),qPrint(subst.left(ii+1)));
            if (ii!=DString::npos && prefix.length()>=ii+2 && prefix.mid(prefix.length()-ii-2,ii+1)==subst.left(ii+1))
            {
              return true; // don't replace 'A< ' with 'A< A<...', see issue #10951
            }

            while ((ii=subst.find(nameArg,pp))!=DString::npos)
            {
              bool beforeNonWord = ii==0 || !isId(subst.at(ii-1));
              bool afterNonWord  = subst.length()==ii+nameArg.length() || !isId(subst.at(ii+nameArg.length()));
              if (beforeNonWord && afterNonWord)
              {
                return true; // if nameArg=='A' then subst=='A::Z' or 'S<A>' or 'Z::A' should return true, but 'AA::ZZ' or 'BAH' should not match
              }
              pp=ii+nameArg.length();
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
            if (actArg.name.empty())
            {
              result += actArg.type;
            }
            else
              // for case where the actual arg is something like "unsigned int"
              // the "int" part is in actArg->name.
            {
              result += actArg.type+" "+actArg.name;
            }
            found=true;
          }
        }
        else if (formArg.name==n &&
                 (actualArgs==nullptr || actIt==actualArgs->end()) &&
                 !formArg.defval.empty() &&
                 formArg.defval!=nm /* to prevent recursion */
            )
        {
          result += substituteTemplateArgumentsInString(formArg.defval,formalArgs,actualArgs);
          found=true;
        }
      }
      else if (formArg.name==n &&
               (actualArgs==nullptr || actIt==actualArgs->end()) &&
               !formArg.defval.empty() &&
               formArg.defval!=nm /* to prevent recursion */
              )
      {
        result += substituteTemplateArgumentsInString(formArg.defval,formalArgs,actualArgs);
        found=true;
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
 *  should be considered, e.g. with \a parentOnly is \c true, `A<T>::B<S>` will
 *  try to strip `<T>` and not `<S>`, while \a parentOnly is \c false will
 *  strip both unless `A<T>` or `B<S>` are specialized template classes.
 */
DString stripTemplateSpecifiersFromScope(const DString &fullName,
    bool parentOnly,
    DString *pLastScopeStripped,
    DString scopeName,
    bool allowArtificial)
{
  //printf("stripTemplateSpecifiersFromScope(name=%s,scopeName=%s)\n",qPrint(fullName),qPrint(scopeName));
  size_t i=fullName.find('<');
  if (i==DString::npos) return fullName;
  DString result;
  size_t p=0;
  size_t l=fullName.length();
  while (i!=DString::npos)
  {
    //printf("1:result+=%s\n",qPrint(fullName.mid(p,i-p)));
    size_t e=i+1;
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
    size_t si = fullName.find("::",e);

    if (parentOnly && si==DString::npos) break;
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
DString mergeScopes(const DString &leftScope,const DString &rightScope)
{
  AUTO_TRACE("leftScope='{}' rightScope='{}'",leftScope,rightScope);
  // case leftScope=="A" rightScope=="A::B" => result = "A::B"
  if (leftScopeMatch(leftScope,rightScope))
  {
    AUTO_TRACE_EXIT("case1={}",rightScope);
    return rightScope;
  }
  DString result;
  size_t i=0,p=leftScope.length();

  // case leftScope=="A::B" rightScope=="B::C" => result = "A::B::C"
  // case leftScope=="A::B" rightScope=="B" => result = "A::B"
  bool found=false;
  while ((i=leftScope.rfind("::",p))!=DString::npos && i>0)
  {
    if (leftScopeMatch(rightScope,leftScope.mid(i+2)))
    {
      result = leftScope.left(i+2)+rightScope;
      found=true;
    }
    p=i-1;
  }
  if (found)
  {
    AUTO_TRACE_EXIT("case2={}",result);
    return result;
  }

  // case leftScope=="A" rightScope=="B" => result = "A::B"
  result=leftScope;
  if (!result.empty() && !rightScope.empty()) result+="::";
  result+=rightScope;
  AUTO_TRACE_EXIT("case3={}",result);
  return result;
}

/*! Returns a fragment from scope \a s, starting at position \a p.
 *
 *  @param s the scope name as a string.
 *  @param p the start position (0 is the first).
 *  @param l the resulting length of the fragment.
 *  @returns the location of the fragment, or -1 if non is found.
 */
int getScopeFragment(const DString &s,int p,int *l)
{
  int sl=static_cast<int>(s.length());
  int sp=p;
  int count=0;
  bool done=false;
  if (sp>=sl) return -1;
  while (sp<sl)
  {
    char c=s.at(sp);
    if (c==':')
    {
      sp++;
      p++;
    }
    else
    {
      break;
    }
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
      if (recursivelyAddGroupListToTitle(ol, gd, false))
      {
        ol.writeString(" &raquo; ");
      }
      ol.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),DString(),gd->groupTitle());
    }
    if (root)
    {
      // add module as a group to the file as well
      if (mod)
      {
        if (!first) { ol.writeString(" &#124; "); } else first=false;
        ol.writeString(theTranslator->trModule(false,true)+" ");
        ol.writeObjectLink(mod->getReference(),mod->getOutputFileBase(),DString(),
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
  recursivelyAddGroupListToTitle(ol,d,true);
}

bool checkExtension(const DString &fName, const DString &ext)
{
  return fName.right(ext.length())==ext;
}

void addHtmlExtensionIfMissing(DString &fName)
{
  if (fName.empty()) return;
  size_t i_fs = fName.rfind('/');
  size_t i_bs = fName.rfind('\\');
  size_t p    = i_fs!=DString::npos && i_bs!=DString::npos ? std::max(i_fs, i_bs) :
                i_fs!=DString::npos ? i_fs : i_bs!=DString::npos ? i_bs : 0;
  size_t i    = fName.find('.',p); // search for . after path part
  if (i==DString::npos)
  {
    fName+=Doxygen::htmlFileExtension;
  }
}

DString stripExtensionGeneral(const DString &fName, const DString &ext)
{
  DString result=fName;
  if (result.right(ext.length())==ext)
  {
    result=result.left(result.length()-ext.length());
  }
  return result;
}

DString stripExtension(const DString &fName)
{
  return stripExtensionGeneral(fName, Doxygen::htmlFileExtension);
}

DString stripPath(const DString &s)
{
  DString result=s;
  if (size_t i=result.rfind('/'); i!=DString::npos)
  {
    result=result.mid(i+1);
  }
  if (size_t i=result.rfind('\\'); i!=DString::npos)
  {
    result=result.mid(i+1);
  }
  return result;
}

DString makeBaseName(const DString &name, const DString &ext)
{
  return stripExtensionGeneral(stripPath(name), ext);
}

/** Special version of DString::stripWhiteSpace() that only strips
 *  completely blank lines.
 *  @param s the string to be stripped
 *  @param docLine the line number corresponding to the start of the
 *         string. This will be adjusted based on the number of lines stripped
 *         from the start.
 *  @returns The stripped string.
 */
DString stripLeadingAndTrailingEmptyLines(const DString &s,int &docLine)
{
  if (s.empty()) return DString();
  const char *p = s.data();

  // search for leading empty lines
  int i=0,li=-1,l=static_cast<int>(s.length());
  char c = 0;
  while ((c=*p))
  {
    if (c==' ' || c=='\t' || c=='\r') { i++; p++; }
    else if (c=='\\' && literal_at(p,"\\ilinebr")) { i+=8; li=i; p+=8; }
    else if (c=='\n') { i++; li=i; docLine++; p++; }
    else break;
  }

  // search for trailing empty lines
  int b=l-1,bi=-1;
  p=s.data()+b;
  while (b>=0)
  {
    c=*p;
    if (c==' ' || c=='\t' || c=='\r') { b--; p--; }
    else if (c=='r' && b>=7 && literal_at(p-7,"\\ilinebr")) { bi=b-7; b-=8; p-=8; }
    else if (c=='>' && b>=11 && literal_at(p-11,"\\ilinebr<br>")) { bi=b-11; b-=12; p-=12; }
    else if (c=='\n') { bi=b; b--; p--; }
    else break;
  }

  // return whole string if no leading or trailing lines where found
  if (li==-1 && bi==-1) return s;

  // return substring
  if (bi==-1) bi=l;
  if (li==-1) li=0;
  if (bi<=li) return DString(); // only empty lines
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

bool updateLanguageMapping(const DString &extension,const DString &language)
{
  DString langName = language.lower();
  auto it1 = std::find_if(g_lang2extMap.begin(),g_lang2extMap.end(),
                        [&langName](const auto &info) { return info.langName==langName; });
  if (it1 == g_lang2extMap.end()) return false;

  // found the language
  SrcLangExt parserId = it1->parserId;
  DString extName = extension.lower();
  if (extName.empty()) return false;
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
    err("Failed to assign extension {} to parser {} for language {}\n",
        extName.data(),it1->parserName,language);
  }
  else
  {
    //msg("Registered extension {} to language parser {}...\n",
    //    extName,language);
  }
  return true;
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

SrcLangExt getLanguageFromFileName(const DString& fileName, SrcLangExt defLang)
{
  FileInfo fi(fileName.str());
  // we need only the part after the last ".", newer implementations of FileInfo have 'suffix()' for this.
  DString extName = DString(fi.extension(false)).lower();
  if (extName.empty()) extName=".no_extension";
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
SrcLangExt getLanguageFromCodeLang(DString &fileName)
{
  // try the extension
  auto lang = getLanguageFromFileName(fileName, SrcLangExt::Unknown);
  if (lang == SrcLangExt::Unknown)
  {
    // try the language names
    DString langName = fileName.lower();
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

DString getFileNameExtension(const DString &fn)
{
  if (fn.empty()) return "";
  if (size_t lastDot = fn.rfind('.'); lastDot!=DString::npos) return fn.mid(lastDot);
  return "";
}

//--------------------------------------------------------------------------

static MemberDef *getMemberFromSymbol(const Definition *scope,const FileDef *fileScope,
                                const DString &n)
{
  if (scope==nullptr ||
      (scope->definitionType()!=Definition::TypeClass &&
       scope->definitionType()!=Definition::TypeNamespace
      )
     )
  {
    scope=Doxygen::globalScope;
  }

  DString name = n;
  if (name.empty())
    return nullptr; // no name was given

  auto &range = Doxygen::symbolMap->find(name);
  if (range.empty())
    return nullptr; // could not find any matching symbols

  // mostly copied from getResolvedClassRec()
  DString explicitScopePart;
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
bool checkIfTypedef(const Definition *scope,const FileDef *fileScope,const DString &n)
{
  MemberDef *bestMatch = getMemberFromSymbol(scope,fileScope,n);

  if (bestMatch && bestMatch->isTypedef())
    return true; // closest matching symbol is a typedef
  else
    return false;
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
    linkifyText(TextGeneratorOLImpl(ol),a.type,LinkifyTextOptions().setScope(d));
    ol.endConstraintType();
    ol.startConstraintDocs();
    ol.generateDoc(d->docFile(),
                   d->docLine(),
                   d,
                   nullptr,
                   a.docs,
                   DocOptions()
                   .setIndexWords(true));
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
  p += snprintf(p,cmdLen,"/usr/bin/atos -p %d ", (int)getpid());
  for (int x = 0; x < frameCount; x++)
  {
    p += snprintf(p,cmdLen,"%p ", backtraceFrames[x]);
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

static void transcodeCharacterBuffer(const DString &fileName,std::string &contents,
           const DString &inputEncoding,const DString &outputEncoding)
{
  if (inputEncoding.empty() || outputEncoding.empty()) return; // no encoding specified
  if (dstricmp(inputEncoding,outputEncoding)==0) return;           // input encoding same as output encoding
  void *cd = portable_iconv_open(outputEncoding.data(),inputEncoding.data());
  if (cd==reinterpret_cast<void *>(-1))
  {
    term("unsupported character conversion: '{}'->'{}': {}\n"
        "Check the INPUT_ENCODING setting in the config file!\n",
        inputEncoding,outputEncoding,strerror(errno));
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
    term("{}: failed to translate characters from {} to {}: check INPUT_ENCODING\n",
        fileName,inputEncoding,outputEncoding);
  }
  portable_iconv_close(cd);
}

//! read a file name \a fileName and optionally filter and transcode it
bool readInputFile(const DString &fileName,std::string &contents,bool filter,bool isSourceCode)
{
  // try to open file
  FileInfo fi(fileName.str());
  if (!fi.exists()) return false;
  DString filterName = getFileFilter(fileName,isSourceCode);
  if (filterName.empty() || !filter)
  {
    std::ifstream f = Portable::openInputStream(fileName,true);
    if (!f.is_open())
    {
      err("could not open file {}\n",fileName);
      return false;
    }
    // read the file
    auto fileSize = fi.size();
    contents.resize(fileSize);
    f.read(contents.data(),fileSize);
    if (f.fail())
    {
      err("problems while reading file {}\n",fileName);
      return false;
    }
  }
  else
  {
    DString cmd=filterName+" \""+fileName+"\"";
    Debug::print(Debug::ExtCmd,0,"Executing popen(`{}`)\n",cmd);
    FILE *f=Portable::popen(cmd,"r");
    if (!f)
    {
      err("could not execute filter {}\n",filterName);
      return false;
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
    Debug::print(Debug::FilterOutput,0,"-------------\n{}\n-------------\n",contents);
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
DString filterTitle(const DString &title)
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
  return tf;
}

//----------------------------------------------------------------------------

DString getEncoding(const FileInfo &fi)
{
  InputFileEncoding elem;
  auto getter = [](const InputFileEncoding &e) { return e.pattern.str(); };
  if (fi.match(Doxygen::inputFileEncodingList,useCaseSenseNames(),&elem,getter)) // check for file specific encoding
  {
    return elem.encoding;
  }
  else // fall back to default encoding
  {
    return Config_getString(INPUT_ENCODING);
  }
}

DString externalLinkTarget(const bool parent)
{
  bool extLinksInWindow = Config_getBool(EXT_LINKS_IN_WINDOW);
  if (extLinksInWindow)
    return "target=\"_blank\" ";
  else if (parent)
    return "target=\"_parent\" ";
  else
    return "";
}

DString createHtmlUrl(const DString &relPath,
                       const DString &ref,
                       bool isLocalFile,
                       const DString &targetFileName,
                       const DString &anchor)
{
  DString url;
  if (!ref.empty())
  {
    url = externalRef(relPath,ref);
  }
  if (!targetFileName.empty())
  {
    DString fn = targetFileName;
    if (ref.empty())
    {
      if (!anchor.empty() && isLocalFile)
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
  if (!anchor.empty())
  {
    if (!url.endsWith("=")) url+="#";
    url+=anchor;
  }
  //printf("createHtmlUrl(relPath=%s,local=%d,target=%s,anchor=%s)=%s\n",qPrint(relPath),isLocalFile,qPrint(targetFileName),qPrint(anchor),qPrint(url));
  return url;
}

DString externalRef(const DString &relPath,const DString &ref)
{
  DString result;
  if (!ref.empty())
  {
    auto it = Doxygen::tagDestinationMap.find(ref.str());
    if (it!=Doxygen::tagDestinationMap.end())
    {
      result = it->second;
      size_t l = result.length();
      if (!relPath.empty() && l>0 && result.at(0)=='.')
      { // relative path -> prepend relPath.
        result.prepend(relPath);
        l+=relPath.length();
      }
      if (l>0 && result.at(l-1)!='/') result+='/';
    }
  }
  else
  {
    result = relPath;
  }
  return result;
}

/** Replaces any markers of the form \#\#AA in input string \a str
 *  by new markers of the form \#AABBCC, where \#AABBCC represents a
 *  valid color, based on the intensity represented by hex number AA
 *  and the current HTML_COLORSTYLE_* settings.
 */
DString replaceColorMarkers(const DString &str)
{
  if (str.empty()) return DString();
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
  return result;
}

/** Copies the contents of file with name \a src to the newly created
 *  file with name \a dest. Returns true if successful.
 */
bool copyFile(const DString &src,const DString &dest)
{
  if (!Dir().copy(src.str(),dest.str()))
  {
    err("could not copy file {} to {}\n",src,dest);
    return false;
  }
  return true;
}

DString getLanguageSpecificSeparator(SrcLangExt lang,bool classScope)
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
bool isURL(const DString &url)
{
  static const std::unordered_set<std::string> schemes = {
    "http", "https", "ftp", "ftps", "sftp", "file", "news", "irc", "ircs"
  };
  DString loc_url = url.stripWhiteSpace();
  size_t colonPos = loc_url.find(':');
  return colonPos!=DString::npos && schemes.find(loc_url.left(colonPos).str())!=schemes.end();
}

/** Corrects URL \a url according to the relative path \a relPath.
 *  Returns the corrected URL. For absolute URLs no correction will be done.
 */
DString correctURL(const DString &url,const DString &relPath)
{
  DString result = url;
  if (!relPath.empty() && !isURL(url))
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

DString stripIndentation(const DString &s,bool skipFirstLine)
{
  if (s.empty()) return s; // empty string -> we're done

  //printf("stripIndentation:\n%s\n------\n",qPrint(s));
  // compute minimum indentation over all lines
  const char *p=s.data();
  char c=0;
  int indent=0;
  int minIndent=1000000; // "infinite"
  bool searchIndent=true;
  int tabSize=Config_getInt(TAB_SIZE);
  bool skipFirst = skipFirstLine;
  while ((c=*p++))
  {
    if      (c=='\t') { indent+=tabSize - (indent%tabSize); }
    else if (c=='\n') { indent=0; searchIndent=true; skipFirst=false; }
    else if (c==' ')  { indent++; }
    else if (searchIndent && !skipFirst)
    {
      searchIndent=false;
      if (indent<minIndent) minIndent=indent;
    }
  }

  // no indent to remove -> we're done
  if (minIndent==0) return substitute(s,"@ilinebr","\\ilinebr");

  // remove minimum indentation for each line
  TextStream result;
  p=s.data();
  indent=0;
  skipFirst=skipFirstLine;
  while ((c=*p++))
  {
    if (c=='\n') // start of new line
    {
      indent=0;
      result << c;
      skipFirst=false;
    }
    else if (indent<minIndent && !skipFirst) // skip until we reach minIndent
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
    else if (c=='\\' && literal_at(p,"ilinebr "))
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
    else if (c=='@' && literal_at(p,"ilinebr"))
    {
      result << "\\ilinebr";
      p+=7;
    }
    else // copy anything until the end of the line
    {
      result << c;
    }
  }

  //printf("stripIndentation: result=\n%s\n------\n",qPrint(result.str()));

  return result.str();
}

// strip up to \a indentationLevel spaces from each line in \a doc (excluding the first line
//  when skipFirstLine is set to true)
void stripIndentationVerbatim(DString &doc,size_t indentationLevel, bool skipFirstLine)
{
  //printf("stripIndentationVerbatim(level=%d):\n%s\n------\n",indentationLevel,qPrint(doc));
  if (indentationLevel <= 0 || doc.empty()) return; // nothing to strip

  // by stripping content the string will only become shorter so we write the results
  // back into the input string and then resize it at the end.
  char c = 0;
  const char *src = doc.data();
  char *dst = doc.rawData();
  bool insideIndent = !skipFirstLine; // skip the initial line from stripping
  size_t cnt = 0;
  if (!skipFirstLine) cnt = indentationLevel;
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

//----------------------------------------------------------------------------

/** Strip the direction part from docs and return it as a string in canonical form.
 *  The input \a docs string can start with e.g. "[in]", "[in, out]", "[inout]", "[out,in]"...
 *  @returns either "[in,out]", "[in]", or "[out]" or the empty string.
 */
static DString extractDirection(DString &docs)
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
      if ( inIndex!=std::string::npos) { dir.erase( inIndex,2); ioMask|=(1<<0); }
      size_t outIndex = dir.find("out");
      if (outIndex!=std::string::npos) { dir.erase(outIndex,3); ioMask|=(1<<1); }
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

DString inlineArgListToDoc(const ArgumentList &al)
{
  DString paramDocs;
  if (al.hasDocumentation(true))
  {
    for (const Argument &a : al)
    {
      if (a.hasDocumentation(true))
      {
        DString docsWithoutDir = a.docs;
        DString direction = extractDirection(docsWithoutDir);
        DString name = a.name;
        if (name.empty())
        {
          name = "-";
        }
        paramDocs+=" \\ilinebr @param"+direction+" "+name+" "+docsWithoutDir;
      }
    }
  }
  return paramDocs;
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

DString getDotImageExtension()
{
  DString imgExt = Config_getEnumAsString(DOT_IMAGE_FORMAT);
  size_t i= imgExt.find(':'); // strip renderer part when using e.g. 'png:cairo:gd' as format
  return i==DString::npos ? imgExt : imgExt.left(i);
}

bool openOutputFile(const DString &outFile,std::ofstream &f)
{
  ASSERT(!f.is_open());
  bool fileOpened=false;
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

//------------------------------------------------------

//! remove disabled blocks and all block markers from \a s and return the result as a string
DString selectBlocks(const DString &s,const SelectionBlockList &blockList,const SelectionMarkerInfo &markerInfo)
{
  if (s.empty()) return s;

  // helper to find the end of a block
  auto skipBlock = [&markerInfo](const char *p,const SelectionBlock &blk)
  {
    char c = 0;
    while ((c=*p))
    {
      if (c==markerInfo.markerChar && dstrncmp(p,markerInfo.endStr,markerInfo.endLen)==0) // end marker
      {
        size_t len = markerInfo.endLen;
        bool negate = *(p+markerInfo.endLen)=='!';
        if (negate) len++;
        size_t blkNameLen = dstrlen(blk.name);
        if (dstrncmp(p+len,blk.name,blkNameLen)==0 &&                                // matching marker name
            dstrncmp(p+len+blkNameLen,markerInfo.closeStr,markerInfo.closeLen)==0) // matching marker closing
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

  DString result;
  result.reserve(s.length());
  const char *p = s.data();
  char c = 0;
  while ((c=*p))
  {
    if (c==markerInfo.markerChar) // potential start of marker
    {
      if (dstrncmp(p,markerInfo.beginStr,markerInfo.beginLen)==0) // start of begin marker
      {
        bool found = false;
        size_t len = markerInfo.beginLen;
        bool negate = *(p+len)=='!';
        if (negate) len++;
        for (const auto &blk : blockList)
        {
          size_t blkNameLen = dstrlen(blk.name);
          if (dstrncmp(p+len,blk.name,blkNameLen)==0 &&                                // matching marker name
              dstrncmp(p+len+blkNameLen,markerInfo.closeStr,markerInfo.closeLen)==0) // matching marker closing
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
      else if (dstrncmp(p,markerInfo.endStr,markerInfo.endLen)==0) // start of end marker
      {
        bool found = false;
        size_t len = markerInfo.endLen;
        bool negate = *(p+len)=='!';
        if (negate) len++;
        for (const auto &blk : blockList)
        {
          size_t blkNameLen = dstrlen(blk.name);
          if (dstrncmp(p+len,blk.name,blkNameLen)==0 &&                                // matching marker name
              dstrncmp(p+len+blkNameLen,markerInfo.closeStr,markerInfo.closeLen)==0) // matching marker closing
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

void checkBlocks(const DString &s, const DString fileName,const SelectionMarkerInfo &markerInfo)
{
  if (s.empty()) return;

  const char *p = s.data();
  char c = 0;
  while ((c=*p))
  {
    if (c==markerInfo.markerChar) // potential start of marker
    {
      if (dstrncmp(p,markerInfo.beginStr,markerInfo.beginLen)==0) // start of begin marker
      {
        size_t len = markerInfo.beginLen;
        bool negate = *(p+len)=='!';
        if (negate) len++;
        p += len;
        DString marker;
        while (*p)
        {
          if (markerInfo.closeLen==0 && *p=='\n') // matching end of line
          {
            warn(fileName,-1,"Remaining begin replacement with marker '{}'",marker);
            break;
          }
          else if (markerInfo.closeLen!= 0 && dstrncmp(p,markerInfo.closeStr,markerInfo.closeLen)==0) // matching marker closing
          {
            p += markerInfo.closeLen;
            warn(fileName,-1,"Remaining begin replacement with marker '{}'",marker);
            break;
          }
          marker += *p;
          p++;
        }
      }
      else if (dstrncmp(p,markerInfo.endStr,markerInfo.endLen)==0) // start of end marker
      {
        size_t len = markerInfo.endLen;
        bool negate = *(p+len)=='!';
        if (negate) len++;
        p += len;
        DString marker;
        while (*p)
        {
          if (markerInfo.closeLen==0 && *p=='\n') // matching end of line
          {
            warn(fileName,-1,"Remaining end replacement with marker '{}'",marker);
            break;
          }
          else if (markerInfo.closeLen!= 0 && dstrncmp(p,markerInfo.closeStr,markerInfo.closeLen)==0) // matching marker closing
          {
            p += markerInfo.closeLen;
            warn(fileName,-1,"Remaining end replacement with marker '{}'",marker);
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



DString detab(const DString &s,size_t &refIndent)
{
  int tabSize = Config_getInt(TAB_SIZE);
  size_t size = s.length();
  DString result;
  result.reserve(size+256);
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
          while (stop--) result+=' ';
        }
        break;
      case '\\':
        if (data[i] == '\\') // escaped command -> ignore
        {
          result+=c;
          result+=data[i++];
          col+=2;
        }
        else if (i+5<size && literal_at(data+i,"iskip")) // \iskip command
        {
          i+=5;
          skip = true;
        }
        else if (i+8<size && literal_at(data+i,"endiskip")) // \endiskip command
        {
          i+=8;
          skip = false;
        }
        else // some other command
        {
          result+=c;
          col++;
        }
        break;
      case '\n': // reset column counter
        result+=c;
        col=0;
        break;
      case ' ': // increment column counter
        result+=c;
        col++;
        break;
      default: // non-whitespace => update minIndent
        if (c<0 && i<size) // multibyte sequence
        {
          // special handling of the UTF-8 nbsp character 0xC2 0xA0
          int nb = isUTF8NonBreakableSpace(data);
          if (nb>0)
          {
            result+=doxy_nbsp;
            i+=nb-1;
          }
          else
          {
            int bytes = getUTF8CharNumBytes(c);
            for (int j=0;j<bytes-1 && c;j++)
            {
              result+=c;
              c = data[i++];
            }
            result+=c;
          }
        }
        else
        {
          result+=c;
        }
        if (!skip && col<minIndent) minIndent=col;
        col++;
    }
  }
  if (minIndent!=maxIndent) refIndent=minIndent; else refIndent=0;
  //printf("detab(\n%s\n)=[\n%s\n]\n",qPrint(s),qPrint(out.get()));
  return result;
}

DString getProjectId()
{
  DString projectCookie = Config_getString(HTML_PROJECT_COOKIE);
  if (projectCookie.empty()) return DString();
  return md5str(projectCookie.view())+"_";
}

//! Return the index of the last :: in the string \a name that is still before the first <
int computeQualifiedIndex(const DString &name)
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

// in C# A, A<T>, and A<T,S> are different classes, so we need some way to disguish them using this name mangling
// A      -> A
// A<T>   -> A-1-g
// A<T,S> -> A-2-g
DString mangleCSharpGenericName(const DString &name)
{
  if (size_t idx = name.find('<'); idx!=DString::npos)
  {
    return name.left(idx)+"-"+DString().setNum(name.contains(",")+1)+"-g";
  }
  return name;
}

DString demangleCSharpGenericName(const DString &name,const DString &templArgs)
{
  DString result=name;
  if (result.endsWith("-g"))
  {
    size_t idx = result.find('-');
    result = result.left(idx)+templArgs;
  }
  return result;
}

DString extractBeginRawStringDelimiter(const char *rawStart)
{
  DString text=rawStart;
  size_t i = text.find('"');
  ASSERT(i!=DString::npos);
  return text.mid(i+1,text.length()-i-2); // text=...R"xyz( -> delimiter=xyz
}

DString extractEndRawStringDelimiter(const char *rawEnd)
{
  DString text=rawEnd;
  return text.mid(1,text.length()-2); // text=)xyz" -> delimiter=xyz
}

//----------------------------------------------------------------------------------------------------------

static std::mutex         writeFileContents_lock;
static StringUnorderedSet writeFileContents_set;

DString writeInlineGraph(const DString &baseName,const DString &extension,const DString &content,bool &exists)
{
  DString fileName = baseName + md5str(content.view()) + extension;
  { // ==== start atomic section
    std::lock_guard lock(writeFileContents_lock);
    auto it=writeFileContents_set.find(fileName.str());
    exists = it!=writeFileContents_set.end();
    if (!exists)
    {
      writeFileContents_set.insert(fileName.str());
      if (auto file = Portable::openOutputStream(fileName); file.is_open())
      {
        file.write( content.data(), content.length() );
        file.close();
      }
      else
      {
        err("Could not open file {} for writing\n",fileName);
        return DString();
      }
    }
  } // ==== end atomic section
  return fileName;
}

void cleanupInlineGraphs()
{
  if (Config_getBool(DOT_CLEANUP))
  {
    for (const auto& fileName: writeFileContents_set)
    {
      Dir().remove(qPrint(fileName));
    }
  }
}

