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
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <limits.h>

#include "md5.h"

#include <qregexp.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qcache.h>

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

#define ENABLE_TRACINGSUPPORT 0

#if defined(_OS_MAC_) && ENABLE_TRACINGSUPPORT
#define TRACINGSUPPORT
#endif

#ifdef TRACINGSUPPORT
#include <execinfo.h>
#include <unistd.h>
#endif


//------------------------------------------------------------------------

// selects one of the name to sub-dir mapping algorithms that is used
// to select a sub directory when CREATE_SUBDIRS is set to YES.

#define ALGO_COUNT 1
#define ALGO_CRC16 2
#define ALGO_MD5   3
    
//#define MAP_ALGO ALGO_COUNT
//#define MAP_ALGO ALGO_CRC16
#define MAP_ALGO ALGO_MD5

#define REL_PATH_TO_ROOT "../../"

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
QCString removeAnonymousScopes(const QCString &s)
{
  QCString result;
  if (s.isEmpty()) return result;
  static QRegExp re("[ :]*@[0-9]+[: ]*");
  int i,l,sl=s.length();
  int p=0;
  while ((i=re.match(s,p,&l))!=-1)
  {
    result+=s.mid(p,i-p);
    int c=i;
    bool b1=FALSE,b2=FALSE;
    while (c<i+l && s.at(c)!='@') if (s.at(c++)==':') b1=TRUE;
    c=i+l-1;
    while (c>=i && s.at(c)!='@') if (s.at(c--)==':') b2=TRUE;
    if (b1 && b2) 
    { 
      result+="::"; 
    }
    p=i+l;
  }
  result+=s.right(sl-p);
  //printf("removeAnonymousScopes('%s')='%s'\n",s.data(),result.data());
  return result;
}

// replace anonymous scopes with __anonymous__ or replacement if provided
QCString replaceAnonymousScopes(const QCString &s,const char *replacement)
{
  QCString result;
  if (s.isEmpty()) return result;
  static QRegExp re("@[0-9]+");
  int i,l,sl=s.length();
  int p=0;
  while ((i=re.match(s,p,&l))!=-1)
  {
    result+=s.mid(p,i-p);
    if (replacement)
    {
      result+=replacement;
    }
    else
    {
      result+="__anonymous__";
    }
    p=i+l;
  }
  result+=s.right(sl-p);
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
    if (Doxygen::namespaceSDict->find(s.left(i+l))!=0)
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

static QCString stripFromPath(const QCString &path,QStrList &l)
{
  // look at all the strings in the list and strip the longest match  
  const char *s=l.first();
  QCString potential;
  unsigned int length = 0;
  while (s)
  {
    QCString prefix = s;
    if (prefix.length() > length &&
        qstricmp(path.left(prefix.length()),prefix)==0) // case insensitive compare
    {
      length = prefix.length();
      potential = path.right(path.length()-prefix.length());
    }
    s = l.next();
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
  MemberDef *md=0;
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
      MemberNameSDict *mnd=0;
      if (resScope->definitionType()==Definition::TypeClass)
      {
        mnd=Doxygen::memberNameSDict;
      }
      else
      {
        mnd=Doxygen::functionNameSDict;
      }
      MemberName *mn=mnd->find(resName);
      if (mn)
      {
        MemberNameIterator mni(*mn);
        MemberDef *tmd=0;
        int minDist=-1;
        for (;(tmd=mni.current());++mni)
        {
          //printf("Found member %s resScope=%s outerScope=%s mContext=%p\n",
          //    tmd->name().data(), resScope->name().data(), 
          //    tmd->getOuterScope()->name().data(), mContext);
          if (tmd->isTypedef() /*&& tmd->getOuterScope()==resScope*/)
          {
            int dist=isAccessibleFrom(resScope,0,tmd);
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
  QCString name=n;
  ClassDef *result = Doxygen::classSDict->find(name);
  //if (result==0 && !exact) // also try generic and protocol versions
  //{
  //  result = Doxygen::classSDict->find(name+"-g");
  //  if (result==0)
  //  {
  //    result = Doxygen::classSDict->find(name+"-p");
  //  }
  //}
  //printf("getClass(%s)=%s\n",n,result?result->name().data():"<none>");
  return result;
}

NamespaceDef *getResolvedNamespace(const char *name)
{
  if (name==0 || name[0]=='\0') return 0;
  QCString *subst = Doxygen::namespaceAliasDict[name];
  if (subst)
  {
    int count=0; // recursion detection guard
    QCString *newSubst;
    while ((newSubst=Doxygen::namespaceAliasDict[*subst]) && count<10)
    {
      subst=newSubst;
      count++;
    }
    if (count==10)
    {
      warn_uncond("possible recursive namespace alias detected for %s!\n",name);
    }
    return Doxygen::namespaceSDict->find(subst->data());
  }
  else
  {
    return Doxygen::namespaceSDict->find(name);
  }
}

static QDict<MemberDef> g_resolvedTypedefs;
static QDict<Definition> g_visitedNamespaces;

// forward declaration
static const ClassDef *getResolvedClassRec(const Definition *scope,
                              const FileDef *fileScope,
                              const char *n,
                              const MemberDef **pTypeDef,
                              QCString *pTemplSpec,
                              QCString *pResolvedType
                             );
int isAccessibleFromWithExpScope(const Definition *scope,const FileDef *fileScope,const Definition *item,
                     const QCString &explicitScopePart);

/*! Returns the class representing the value of the typedef represented by \a md
 *  within file \a fileScope.
 *
 *  Example: typedef A T; will return the class representing A if it is a class.
 * 
 *  Example: typedef int T; will return 0, since "int" is not a class.
 */
const ClassDef *newResolveTypedef(const FileDef *fileScope,
                                  const MemberDef *md,
                                  const MemberDef **pMemType,
                                  QCString *pTemplSpec,
                                  QCString *pResolvedType,
                                  const ArgumentList *actTemplParams)
{
  //printf("newResolveTypedef(md=%p,cachedVal=%p)\n",md,md->getCachedTypedefVal());
  bool isCached = md->isTypedefValCached(); // value already cached
  if (isCached)
  {
    //printf("Already cached %s->%s [%s]\n",
    //    md->name().data(),
    //    md->getCachedTypedefVal()?md->getCachedTypedefVal()->name().data():"<none>",
    //    md->getCachedResolvedTypedef()?md->getCachedResolvedTypedef().data():"<none>");

    if (pTemplSpec)    *pTemplSpec    = md->getCachedTypedefTemplSpec();
    if (pResolvedType) *pResolvedType = md->getCachedResolvedTypedef();
    return md->getCachedTypedefVal();
  }
  //printf("new typedef\n");
  QCString qname = md->qualifiedName();
  if (g_resolvedTypedefs.find(qname)) return 0; // typedef already done

  g_resolvedTypedefs.insert(qname,md); // put on the trace list
  
  const ClassDef *typeClass = md->getClassDef();
  QCString type = md->typeString(); // get the "value" of the typedef
  if (typeClass && typeClass->isTemplate() &&
      actTemplParams && !actTemplParams->empty())
  {
    type = substituteTemplateArgumentsInString(type,
            typeClass->templateArguments(),*actTemplParams);
  }
  QCString typedefValue = type;
  int tl=type.length();
  int ip=tl-1; // remove * and & at the end
  while (ip>=0 && (type.at(ip)=='*' || type.at(ip)=='&' || type.at(ip)==' ')) 
  {
    ip--;
  }
  type=type.left(ip+1);
  type.stripPrefix("const ");  // strip leading "const"
  type.stripPrefix("struct "); // strip leading "struct"
  type.stripPrefix("union ");  // strip leading "union"
  int sp=0;
  tl=type.length(); // length may have been changed
  while (sp<tl && type.at(sp)==' ') sp++;
  const MemberDef *memTypeDef = 0;
  const ClassDef  *result = getResolvedClassRec(md->getOuterScope(),
                                  fileScope,type,&memTypeDef,0,pResolvedType);
  // if type is a typedef then return what it resolves to.
  if (memTypeDef && memTypeDef->isTypedef()) 
  {
    result=newResolveTypedef(fileScope,memTypeDef,pMemType,pTemplSpec);
    goto done;
  }
  else if (memTypeDef && memTypeDef->isEnumerate() && pMemType)
  {
    *pMemType = memTypeDef;
  }

  //printf("type=%s result=%p\n",type.data(),result);
  if (result==0)
  {
    // try unspecialized version if type is template
    int si=type.findRev("::");
    int i=type.find('<');
    if (si==-1 && i!=-1) // typedef of a template => try the unspecialized version
    {
      if (pTemplSpec) *pTemplSpec = type.mid(i);
      result = getResolvedClassRec(md->getOuterScope(),fileScope,
                                   type.left(i),0,0,pResolvedType);
      //printf("result=%p pRresolvedType=%s sp=%d ip=%d tl=%d\n",
      //    result,pResolvedType?pResolvedType->data():"<none>",sp,ip,tl);
    }
    else if (si!=-1) // A::B
    {
      i=type.find('<',si);
      if (i==-1) // Something like A<T>::B => lookup A::B
      {
        i=type.length();
      }
      else // Something like A<T>::B<S> => lookup A::B, spec=<S>
      {
        if (pTemplSpec) *pTemplSpec = type.mid(i);
      }
      result = getResolvedClassRec(md->getOuterScope(),fileScope,
           stripTemplateSpecifiersFromScope(type.left(i),FALSE),0,0,
           pResolvedType);
    }

    //if (result) ip=si+sp+1;
  }

done:
  if (pResolvedType)
  {
    if (result)
    {
      *pResolvedType=result->qualifiedName();
      //printf("*pResolvedType=%s\n",pResolvedType->data());
      if (sp>0)    pResolvedType->prepend(typedefValue.left(sp));
      if (ip<tl-1) pResolvedType->append(typedefValue.right(tl-ip-1));
    }
    else
    {
      *pResolvedType=typedefValue;
    }
  }

  // remember computed value for next time
  if (result && result->getDefFileName()!="<code>") 
    // this check is needed to prevent that temporary classes that are 
    // introduced while parsing code fragments are being cached here.
  {
    //printf("setting cached typedef %p in result %p\n",md,result);
    //printf("==> %s (%s,%d)\n",result->name().data(),result->getDefFileName().data(),result->getDefLine());
    //printf("*pResolvedType=%s\n",pResolvedType?pResolvedType->data():"<none>");
    const_cast<MemberDef*>(md)->cacheTypedefVal(result,
        pTemplSpec ? *pTemplSpec : QCString(),
        pResolvedType ? *pResolvedType : QCString()
       );
  }
  
  g_resolvedTypedefs.remove(qname); // remove from the trace list
  
  return result;
}

/*! Substitutes a simple unqualified \a name within \a scope. Returns the
 *  value of the typedef or \a name if no typedef was found.
 */
static QCString substTypedef(const Definition *scope,const FileDef *fileScope,const QCString &name,
            const MemberDef **pTypeDef=0)
{
  QCString result=name;
  if (name.isEmpty()) return result;

  // lookup scope fragment in the symbol map
  DefinitionIntf *di = Doxygen::symbolMap->find(name);
  if (di==0) return result; // no matches

  MemberDef *bestMatch=0;
  if (di->definitionType()==DefinitionIntf::TypeSymbolList) // multi symbols
  {
    // search for the best match
    DefinitionListIterator dli(*(DefinitionList*)di);
    Definition *d;
    int minDistance=10000; // init at "infinite"
    for (dli.toFirst();(d=dli.current());++dli) // foreach definition
    {
      // only look at members
      if (d->definitionType()==Definition::TypeMember)
      {
        // that are also typedefs
        MemberDef *md = dynamic_cast<MemberDef *>(d);
        if (md->isTypedef()) // d is a typedef
        {
          // test accessibility of typedef within scope.
          int distance = isAccessibleFromWithExpScope(scope,fileScope,d,"");
          if (distance!=-1 && distance<minDistance) 
            // definition is accessible and a better match
          {
            minDistance=distance;
            bestMatch = md; 
          }
        }
      }
    }
  }
  else if (di->definitionType()==DefinitionIntf::TypeMember) // single symbol
  {
    Definition *d = (Definition*)di;
    // that are also typedefs
    MemberDef *md = dynamic_cast<MemberDef *>(di);
    if (md->isTypedef()) // d is a typedef
    {
      // test accessibility of typedef within scope.
      int distance = isAccessibleFromWithExpScope(scope,fileScope,d,"");
      if (distance!=-1) // definition is accessible 
      {
        bestMatch = md; 
      }
    }
  }
  if (bestMatch) 
  {
    result = bestMatch->typeString();
    if (pTypeDef) *pTypeDef=bestMatch;
  }
  
  //printf("substTypedef(%s,%s)=%s\n",scope?scope->name().data():"<global>",
  //                                  name.data(),result.data());
  return result;
}

static const Definition *endOfPathIsUsedClass(const SDict<Definition> *cl,const QCString &localName)
{
  if (cl)
  {
    SDict<Definition>::Iterator cli(*cl);
    Definition *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      if (cd->localName()==localName)
      {
        return cd;
      }
    }
  }
  return 0;
}

/*! Starting with scope \a start, the string \a path is interpreted as
 *  a part of a qualified scope name (e.g. A::B::C), and the scope is 
 *  searched. If found the scope definition is returned, otherwise 0 
 *  is returned.
 */
static const Definition *followPath(const Definition *start,const FileDef *fileScope,const QCString &path)
{
  int is,ps;
  int l;
  const Definition *current=start;
  ps=0;
  //printf("followPath: start='%s' path='%s'\n",start?start->name().data():"<none>",path.data());
  // for each part of the explicit scope
  while ((is=getScopeFragment(path,ps,&l))!=-1)
  {
    // try to resolve the part if it is a typedef
    const MemberDef *typeDef=0;
    QCString qualScopePart = substTypedef(current,fileScope,path.mid(is,l),&typeDef);
    //printf("      qualScopePart=%s\n",qualScopePart.data());
    if (typeDef)
    {
      const ClassDef *type = newResolveTypedef(fileScope,typeDef);
      if (type)
      {
        //printf("Found type %s\n",type->name().data());
        return type;
      }
    }
    const Definition *next = current->findInnerCompound(qualScopePart);
    //printf("++ Looking for %s inside %s result %s\n",
    //     qualScopePart.data(),
    //     current->name().data(),
    //     next?next->name().data():"<null>");
    if (next==0) // failed to follow the path 
    {
      //printf("==> next==0!\n");
      if (current->definitionType()==Definition::TypeNamespace)
      {
        next = endOfPathIsUsedClass(
            (dynamic_cast<const NamespaceDef *>(current))->getUsedClasses(),qualScopePart);
      }
      else if (current->definitionType()==Definition::TypeFile)
      {
        next = endOfPathIsUsedClass(
            (dynamic_cast<const FileDef *>(current))->getUsedClasses(),qualScopePart);
      }
      current = next;
      if (current==0) break;
    }
    else // continue to follow scope
    {
      current = next;
      //printf("==> current = %p\n",current);
    }
    ps=is+l;
  }
  //printf("followPath(start=%s,path=%s) result=%s\n",
  //    start->name().data(),path.data(),current?current->name().data():"<null>");
  return current; // path could be followed
}

bool accessibleViaUsingClass(const SDict<Definition> *cl,
                             const FileDef *fileScope,
                             const Definition *item,
                             const QCString &explicitScopePart=""
                            )
{
  //printf("accessibleViaUsingClass(%p)\n",cl);
  if (cl) // see if the class was imported via a using statement 
  {
    SDict<Definition>::Iterator cli(*cl);
    Definition *ucd;
    bool explicitScopePartEmpty = explicitScopePart.isEmpty();
    for (cli.toFirst();(ucd=cli.current());++cli)
    {
      //printf("Trying via used class %s\n",ucd->name().data());
      const Definition *sc = explicitScopePartEmpty ? ucd : followPath(ucd,fileScope,explicitScopePart);
      if (sc && sc==item) return TRUE; 
      //printf("Try via used class done\n");
    }
  }
  return FALSE;
}

bool accessibleViaUsingNamespace(const NamespaceSDict *nl,
                                 const FileDef *fileScope,
                                 const Definition *item,
                                 const QCString &explicitScopePart="")
{
  static QDict<void> visitedDict;
  if (nl) // check used namespaces for the class
  {
    NamespaceSDict::Iterator nli(*nl);
    NamespaceDef *und;
    int count=0;
    for (nli.toFirst();(und=nli.current());++nli,count++)
    {
      //printf("[Trying via used namespace %s: count=%d/%d\n",und->name().data(),
      //    count,nl->count());
      const Definition *sc = explicitScopePart.isEmpty() ? und : followPath(und,fileScope,explicitScopePart);
      if (sc && item->getOuterScope()==sc) 
      {
        //printf("] found it\n");
        return TRUE; 
      }
      if (item->getLanguage()==SrcLangExt_Cpp)
      {
        QCString key=und->name();
        if (und->getUsedNamespaces() && visitedDict.find(key)==0)
        {
          visitedDict.insert(key,(void *)0x08);

          if (accessibleViaUsingNamespace(und->getUsedNamespaces(),fileScope,item,explicitScopePart))
          {
            //printf("] found it via recursion\n");
            return TRUE;
          }

          visitedDict.remove(key);
        }
      }
      //printf("] Try via used namespace done\n");
    }
  }
  return FALSE;
}

const int MAX_STACK_SIZE = 1000;

/** Helper class representing the stack of items considered while resolving
 *  the scope.
 */
class AccessStack
{
  public:
    AccessStack() : m_index(0) {}
    void push(const Definition *scope,const FileDef *fileScope,const Definition *item)
    {
      if (m_index<MAX_STACK_SIZE)
      {
        m_elements[m_index].scope     = scope;
        m_elements[m_index].fileScope = fileScope;
        m_elements[m_index].item      = item;
        m_index++;
      }
    }
    void push(const Definition *scope,const FileDef *fileScope,const Definition *item,const QCString &expScope)
    {
      if (m_index<MAX_STACK_SIZE)
      {
        m_elements[m_index].scope     = scope;
        m_elements[m_index].fileScope = fileScope;
        m_elements[m_index].item      = item;
        m_elements[m_index].expScope  = expScope;
        m_index++;
      }
    }
    void pop()
    {
      if (m_index>0) m_index--;
    }
    bool find(const Definition *scope,const FileDef *fileScope, const Definition *item)
    {
      int i=0;
      for (i=0;i<m_index;i++)
      {
        AccessElem *e = &m_elements[i];
        if (e->scope==scope && e->fileScope==fileScope && e->item==item) 
        {
          return TRUE;
        }
      }
      return FALSE;
    }
    bool find(const Definition *scope,const FileDef *fileScope, const Definition *item,const QCString &expScope)
    {
      int i=0;
      for (i=0;i<m_index;i++)
      {
        AccessElem *e = &m_elements[i];
        if (e->scope==scope && e->fileScope==fileScope && e->item==item && e->expScope==expScope) 
        {
          return TRUE;
        }
      }
      return FALSE;
    }

  private:
    /** Element in the stack. */
    struct AccessElem
    {
      const Definition *scope;
      const FileDef *fileScope;
      const Definition *item;
      QCString expScope;
    };
    int m_index;
    AccessElem m_elements[MAX_STACK_SIZE];
};

/* Returns the "distance" (=number of levels up) from item to scope, or -1
 * if item in not inside scope. 
 */
int isAccessibleFrom(const Definition *scope,const FileDef *fileScope,const Definition *item)
{
  //printf("<isAccessibleFrom(scope=%s,item=%s itemScope=%s)\n",
  //    scope->name().data(),item->name().data(),item->getOuterScope()->name().data());

  static AccessStack accessStack;
  if (accessStack.find(scope,fileScope,item))
  {
    return -1;
  }
  accessStack.push(scope,fileScope,item);

  int result=0; // assume we found it
  int i;

  Definition *itemScope=item->getOuterScope();
  bool memberAccessibleFromScope = 
      (item->definitionType()==Definition::TypeMember &&                   // a member
       itemScope && itemScope->definitionType()==Definition::TypeClass  && // of a class
       scope->definitionType()==Definition::TypeClass &&                   // accessible
       (dynamic_cast<const ClassDef*>(scope))->isAccessibleMember(dynamic_cast<const MemberDef *>(item)) // from scope
      );
  bool nestedClassInsideBaseClass = 
      (item->definitionType()==Definition::TypeClass &&                    // a nested class
       itemScope && itemScope->definitionType()==Definition::TypeClass &&  // inside a base 
       scope->definitionType()==Definition::TypeClass &&                   // class of scope
       (dynamic_cast<const ClassDef*>(scope))->isBaseClass(dynamic_cast<ClassDef*>(itemScope),TRUE)          
      );

  if (itemScope==scope || memberAccessibleFromScope || nestedClassInsideBaseClass) 
  {
    //printf("> found it\n");
    if (nestedClassInsideBaseClass) result++; // penalty for base class to prevent
                                              // this is preferred over nested class in this class
                                              // see bug 686956
  }
  else if (scope==Doxygen::globalScope)
  {
    if (fileScope)
    {
      SDict<Definition> *cl = fileScope->getUsedClasses();
      if (accessibleViaUsingClass(cl,fileScope,item)) 
      {
        //printf("> found via used class\n");
        goto done;
      }
      NamespaceSDict *nl = fileScope->getUsedNamespaces();
      if (accessibleViaUsingNamespace(nl,fileScope,item)) 
      {
        //printf("> found via used namespace\n");
        goto done;
      }
    }
    //printf("> reached global scope\n");
    result=-1; // not found in path to globalScope
  }
  else // keep searching
  {
    // check if scope is a namespace, which is using other classes and namespaces
    if (scope->definitionType()==Definition::TypeNamespace)
    {
      const NamespaceDef *nscope = dynamic_cast<const NamespaceDef*>(scope);
      //printf("  %s is namespace with %d used classes\n",nscope->name().data(),nscope->getUsedClasses());
      const SDict<Definition> *cl = nscope->getUsedClasses();
      if (accessibleViaUsingClass(cl,fileScope,item)) 
      {
        //printf("> found via used class\n");
        goto done;
      }
      const NamespaceSDict *nl = nscope->getUsedNamespaces();
      if (accessibleViaUsingNamespace(nl,fileScope,item)) 
      {
        //printf("> found via used namespace\n");
        goto done;
      }
    }
    // repeat for the parent scope
    i=isAccessibleFrom(scope->getOuterScope(),fileScope,item);
    //printf("> result=%d\n",i);
    result= (i==-1) ? -1 : i+2;
  }
done:
  accessStack.pop();
  //Doxygen::lookupCache.insert(key,new int(result));
  return result;
}


/* Returns the "distance" (=number of levels up) from item to scope, or -1
 * if item in not in this scope. The explicitScopePart limits the search
 * to scopes that match \a scope (or its parent scope(s)) plus the explicit part.
 * Example:
 *
 * class A { public: class I {}; };
 * class B { public: class J {}; };
 *
 * - Looking for item=='J' inside scope=='B' will return 0.
 * - Looking for item=='I' inside scope=='B' will return -1 
 *   (as it is not found in B nor in the global scope).
 * - Looking for item=='A::I' inside scope=='B', first the match B::A::I is tried but 
 *   not found and then A::I is searched in the global scope, which matches and 
 *   thus the result is 1.
 */
int isAccessibleFromWithExpScope(const Definition *scope,const FileDef *fileScope,
                     const Definition *item,const QCString &explicitScopePart)
{
  if (explicitScopePart.isEmpty())
  {
    // handle degenerate case where there is no explicit scope.
    return isAccessibleFrom(scope,fileScope,item);
  }

  static AccessStack accessStack;
  if (accessStack.find(scope,fileScope,item,explicitScopePart))
  {
    return -1;
  }
  accessStack.push(scope,fileScope,item,explicitScopePart);


  //printf("  <isAccessibleFromWithExpScope(%s,%s,%s)\n",scope?scope->name().data():"<global>",
  //                                      item?item->name().data():"<none>",
  //                                      explicitScopePart.data());
  int result=0; // assume we found it
  const Definition *newScope = followPath(scope,fileScope,explicitScopePart);
  if (newScope)  // explicitScope is inside scope => newScope is the result
  {
    Definition *itemScope = item->getOuterScope();
    //printf("    scope traversal successful %s<->%s!\n",itemScope->name().data(),newScope->name().data());
    //if (newScope && newScope->definitionType()==Definition::TypeClass)
    //{
    //  ClassDef *cd = (ClassDef *)newScope;
    //  printf("---> Class %s: bases=%p\n",cd->name().data(),cd->baseClasses());
    //}
    if (itemScope==newScope)  // exact match of scopes => distance==0
    {
      //printf("> found it\n");
    }
    else if (itemScope && newScope &&
             itemScope->definitionType()==Definition::TypeClass &&
             newScope->definitionType()==Definition::TypeClass &&
             (dynamic_cast<const ClassDef*>(newScope))->isBaseClass(dynamic_cast<const ClassDef*>(itemScope),TRUE,0)
            )
    {
      // inheritance is also ok. Example: looking for B::I, where 
      // class A { public: class I {} };
      // class B : public A {}
      // but looking for B::I, where
      // class A { public: class I {} };
      // class B { public: class I {} };
      // will find A::I, so we still prefer a direct match and give this one a distance of 1
      result=1;

      //printf("scope(%s) is base class of newScope(%s)\n",
      //    scope->name().data(),newScope->name().data());
    }
    else
    {
      int i=-1;
      if (newScope->definitionType()==Definition::TypeNamespace)
      {
        g_visitedNamespaces.insert(newScope->name(),newScope);
        // this part deals with the case where item is a class
        // A::B::C but is explicit referenced as A::C, where B is imported
        // in A via a using directive.
        //printf("newScope is a namespace: %s!\n",newScope->name().data());
        const NamespaceDef *nscope = dynamic_cast<const NamespaceDef*>(newScope);
        const SDict<Definition> *cl = nscope->getUsedClasses();
        if (cl)
        {
          SDict<Definition>::Iterator cli(*cl);
          const Definition *cd;
          for (cli.toFirst();(cd=cli.current());++cli)
          {
            //printf("Trying for class %s\n",cd->name().data());
            if (cd==item)
            {
              //printf("> class is used in this scope\n");
              goto done;
            }
          }
        }
        const NamespaceSDict *nl = nscope->getUsedNamespaces();
        if (nl)
        {
          NamespaceSDict::Iterator nli(*nl);
          const NamespaceDef *nd;
          for (nli.toFirst();(nd=nli.current());++nli)
          {
            if (g_visitedNamespaces.find(nd->name())==0)
            {
              //printf("Trying for namespace %s\n",nd->name().data());
              i = isAccessibleFromWithExpScope(scope,fileScope,item,nd->name());
              if (i!=-1)
              {
                //printf("> found via explicit scope of used namespace\n");
                goto done;
              }
            }
          }
        }
      }
      // repeat for the parent scope
      if (scope!=Doxygen::globalScope)
      {
        i = isAccessibleFromWithExpScope(scope->getOuterScope(),fileScope,
            item,explicitScopePart);
      }
      //printf("  | result=%d\n",i);
      result = (i==-1) ? -1 : i+2;
    }
  }
  else // failed to resolve explicitScope
  {
    //printf("    failed to resolve: scope=%s\n",scope->name().data());
    if (scope->definitionType()==Definition::TypeNamespace)
    {
      const NamespaceDef *nscope = dynamic_cast<const NamespaceDef*>(scope);
      const NamespaceSDict *nl = nscope->getUsedNamespaces();
      if (accessibleViaUsingNamespace(nl,fileScope,item,explicitScopePart)) 
      {
        //printf("> found in used namespace\n");
        goto done;
      }
    }
    if (scope==Doxygen::globalScope)
    {
      if (fileScope)
      {
        const NamespaceSDict *nl = fileScope->getUsedNamespaces();
        if (accessibleViaUsingNamespace(nl,fileScope,item,explicitScopePart)) 
        {
          //printf("> found in used namespace\n");
          goto done;
        }
      }
      //printf("> not found\n");
      result=-1;
    }
    else // continue by looking into the parent scope
    {
      int i=isAccessibleFromWithExpScope(scope->getOuterScope(),fileScope,
          item,explicitScopePart);
      //printf("> result=%d\n",i);
      result= (i==-1) ? -1 : i+2;
    }
  }

done:
  //printf("  > result=%d\n",result);
  accessStack.pop();
  //Doxygen::lookupCache.insert(key,new int(result));
  return result;
}

int computeQualifiedIndex(const QCString &name)
{
  int i = name.find('<');
  return name.findRev("::",i==-1 ? name.length() : i);
}

static void getResolvedSymbol(const Definition *scope,
                       const FileDef *fileScope,
                       Definition *d, 
                       const QCString &explicitScopePart,
                       ArgumentList *actTemplParams,
                       int &minDistance,
                       const ClassDef *&bestMatch,
                       const MemberDef *&bestTypedef,
                       QCString &bestTemplSpec,
                       QCString &bestResolvedType
                      )
{
  //printf("  => found type %x name=%s d=%p\n",
  //       d->definitionType(),d->name().data(),d);

  // only look at classes and members that are enums or typedefs
  if (d->definitionType()==Definition::TypeClass ||
      (d->definitionType()==Definition::TypeMember && 
       ((dynamic_cast<MemberDef*>(d))->isTypedef() || (dynamic_cast<MemberDef*>(d))->isEnumerate()) 
      )
     )
  {
    g_visitedNamespaces.clear();
    // test accessibility of definition within scope.
    int distance = isAccessibleFromWithExpScope(scope,fileScope,d,explicitScopePart);
    //printf("  %s; distance %s (%p) is %d\n",scope->name().data(),d->name().data(),d,distance);
    if (distance!=-1) // definition is accessible
    {
      // see if we are dealing with a class or a typedef
      if (d->definitionType()==Definition::TypeClass) // d is a class
      {
        ClassDef *cd = dynamic_cast<ClassDef *>(d);
        //printf("cd=%s\n",cd->name().data());
        if (!cd->isTemplateArgument()) // skip classes that
          // are only there to 
          // represent a template 
          // argument
        {
          //printf("is not a templ arg\n");
          if (distance<minDistance) // found a definition that is "closer"
          {
            minDistance=distance;
            bestMatch = cd; 
            bestTypedef = 0;
            bestTemplSpec.resize(0);
            bestResolvedType = cd->qualifiedName();
          }
          else if (distance==minDistance &&
              fileScope && bestMatch &&
              fileScope->getUsedNamespaces() && 
              d->getOuterScope()->definitionType()==Definition::TypeNamespace && 
              bestMatch->getOuterScope()==Doxygen::globalScope
              )
          {
            // in case the distance is equal it could be that a class X
            // is defined in a namespace and in the global scope. When searched
            // in the global scope the distance is 0 in both cases. We have
            // to choose one of the definitions: we choose the one in the
            // namespace if the fileScope imports namespaces and the definition
            // found was in a namespace while the best match so far isn't.
            // Just a non-perfect heuristic but it could help in some situations
            // (kdecore code is an example).
            minDistance=distance;
            bestMatch = cd; 
            bestTypedef = 0;
            bestTemplSpec.resize(0);
            bestResolvedType = cd->qualifiedName();
          }
        }
        else
        {
          //printf("  is a template argument!\n");
        }
      }
      else if (d->definitionType()==Definition::TypeMember)
      {
        MemberDef *md = dynamic_cast<MemberDef *>(d);
        //printf("  member isTypedef()=%d\n",md->isTypedef());
        if (md->isTypedef()) // d is a typedef
        {
          QCString args=md->argsString();
          if (args.isEmpty()) // do not expand "typedef t a[4];"
          {
            //printf("    found typedef!\n");

            // we found a symbol at this distance, but if it didn't
            // resolve to a class, we still have to make sure that
            // something at a greater distance does not match, since
            // that symbol is hidden by this one.
            if (distance<minDistance)
            {
              QCString spec;
              QCString type;
              minDistance=distance;
              const MemberDef *enumType = 0;
              const ClassDef *cd = newResolveTypedef(fileScope,md,&enumType,&spec,&type,actTemplParams);
              if (cd)  // type resolves to a class
              {
                //printf("      bestTypeDef=%p spec=%s type=%s\n",md,spec.data(),type.data());
                bestMatch = cd;
                bestTypedef = md;
                bestTemplSpec = spec;
                bestResolvedType = type;
              }
              else if (enumType) // type resolves to a enum
              {
                //printf("      is enum\n");
                bestMatch = 0;
                bestTypedef = enumType;
                bestTemplSpec = "";
                bestResolvedType = enumType->qualifiedName();
              }
              else if (md->isReference()) // external reference
              {
                bestMatch = 0;
                bestTypedef = md;
                bestTemplSpec = spec;
                bestResolvedType = type;
              }
              else
              {
                bestMatch = 0;
                bestTypedef = md;
                bestTemplSpec.resize(0);
                bestResolvedType.resize(0);
                //printf("      no match\n");
              }
            }
            else
            {
              //printf("      not the best match %d min=%d\n",distance,minDistance);
            }
          }
          else
          {
            //printf("     not a simple typedef\n")
          }
        }
        else if (md->isEnumerate())
        {
          if (distance<minDistance)
          {
            minDistance=distance;
            bestMatch = 0;
            bestTypedef = md;
            bestTemplSpec = "";
            bestResolvedType = md->qualifiedName();
          }
        }
      }
    } // if definition accessible
    else
    {
      //printf("  Not accessible!\n");
    }
  } // if definition is a class or member
  //printf("  bestMatch=%p bestResolvedType=%s\n",bestMatch,bestResolvedType.data());
}

/* Find the fully qualified class name referred to by the input class
 * or typedef name against the input scope.
 * Loops through scope and each of its parent scopes looking for a
 * match against the input name. Can recursively call itself when 
 * resolving typedefs.
 */
static const ClassDef *getResolvedClassRec(const Definition *scope,
    const FileDef *fileScope,
    const char *n,
    const MemberDef **pTypeDef,
    QCString *pTemplSpec,
    QCString *pResolvedType
    )
{
  //printf("[getResolvedClassRec(%s,%s)\n",scope?scope->name().data():"<global>",n);
  QCString name;
  QCString explicitScopePart;
  QCString strippedTemplateParams;
  name=stripTemplateSpecifiersFromScope
                     (removeRedundantWhiteSpace(n),TRUE,
                      &strippedTemplateParams);
  ArgumentList actTemplParams;
  if (!strippedTemplateParams.isEmpty()) // template part that was stripped
  {
    stringToArgumentList(strippedTemplateParams,actTemplParams);
  }

  int qualifierIndex = computeQualifiedIndex(name);
  //printf("name=%s qualifierIndex=%d\n",name.data(),qualifierIndex);
  if (qualifierIndex!=-1) // qualified name
  {
    // split off the explicit scope part
    explicitScopePart=name.left(qualifierIndex);
    // todo: improve namespace alias substitution
    replaceNamespaceAliases(explicitScopePart,explicitScopePart.length());
    name=name.mid(qualifierIndex+2);
  }

  if (name.isEmpty()) 
  {
    //printf("] empty name\n");
    return 0; // empty name
  }

  //printf("Looking for symbol %s\n",name.data());
  DefinitionIntf *di = Doxygen::symbolMap->find(name);
  // the -g (for C# generics) and -p (for ObjC protocols) are now already 
  // stripped from the key used in the symbolMap, so that is not needed here.
  if (di==0) 
  {
    //di = Doxygen::symbolMap->find(name+"-g");
    //if (di==0)
    //{
      di = Doxygen::symbolMap->find(name+"-p");
      if (di==0)
      {
        //printf("no such symbol!\n");
        return 0;
      }
    //}
  }
  //printf("found symbol!\n");

  bool hasUsingStatements = 
    (fileScope && ((fileScope->getUsedNamespaces() && 
                    fileScope->getUsedNamespaces()->count()>0) ||
                   (fileScope->getUsedClasses() && 
                    fileScope->getUsedClasses()->count()>0)) 
    );
  //printf("hasUsingStatements=%d\n",hasUsingStatements);
  // Since it is often the case that the same name is searched in the same
  // scope over an over again (especially for the linked source code generation)
  // we use a cache to collect previous results. This is possible since the
  // result of a lookup is deterministic. As the key we use the concatenated
  // scope, the name to search for and the explicit scope prefix. The speedup
  // achieved by this simple cache can be enormous.
  int scopeNameLen = scope->name().length()+1;
  int nameLen = name.length()+1;
  int explicitPartLen = explicitScopePart.length();
  int fileScopeLen = hasUsingStatements ? 1+fileScope->absFilePath().length() : 0;

  // below is a more efficient coding of
  // QCString key=scope->name()+"+"+name+"+"+explicitScopePart;
  QCString key(scopeNameLen+nameLen+explicitPartLen+fileScopeLen+1);
  char *p=key.rawData();
  qstrcpy(p,scope->name()); *(p+scopeNameLen-1)='+';
  p+=scopeNameLen;
  qstrcpy(p,name); *(p+nameLen-1)='+';
  p+=nameLen;
  qstrcpy(p,explicitScopePart);
  p+=explicitPartLen;

  // if a file scope is given and it contains using statements we should
  // also use the file part in the key (as a class name can be in
  // two different namespaces and a using statement in a file can select 
  // one of them).
  if (hasUsingStatements)
  {
    // below is a more efficient coding of
    // key+="+"+fileScope->name();
    *p++='+';
    qstrcpy(p,fileScope->absFilePath());
    p+=fileScopeLen-1;
  }
  *p='\0';

  LookupInfo *pval=Doxygen::lookupCache->find(key);
  //printf("Searching for %s result=%p\n",key.data(),pval);
  if (pval)
  {
    //printf("LookupInfo %p %p '%s' %p\n", 
    //    pval->classDef, pval->typeDef, pval->templSpec.data(), 
    //    pval->resolvedType.data()); 
    if (pTemplSpec)    *pTemplSpec=pval->templSpec;
    if (pTypeDef)      *pTypeDef=pval->typeDef;
    if (pResolvedType) *pResolvedType=pval->resolvedType;
    //printf("] cachedMatch=%s\n",
    //    pval->classDef?pval->classDef->name().data():"<none>");
    //if (pTemplSpec) 
    //  printf("templSpec=%s\n",pTemplSpec->data());
    return pval->classDef; 
  }
  else // not found yet; we already add a 0 to avoid the possibility of 
    // endless recursion.
  {
    Doxygen::lookupCache->insert(key,new LookupInfo);
  }

  const ClassDef *bestMatch=0;
  const MemberDef *bestTypedef=0;
  QCString bestTemplSpec;
  QCString bestResolvedType;
  int minDistance=10000; // init at "infinite"

  if (di->definitionType()==DefinitionIntf::TypeSymbolList) // not a unique name
  {
    //printf("  name is not unique\n");
    DefinitionListIterator dli(*(DefinitionList*)di);
    Definition *d;
    int count=0;
    for (dli.toFirst();(d=dli.current());++dli,++count) // foreach definition
    {
      getResolvedSymbol(scope,fileScope,d,explicitScopePart,&actTemplParams,
                        minDistance,bestMatch,bestTypedef,bestTemplSpec,
                        bestResolvedType);
    }
  }
  else // unique name
  {
    //printf("  name is unique\n");
    Definition *d = (Definition *)di;
    getResolvedSymbol(scope,fileScope,d,explicitScopePart,&actTemplParams,
                      minDistance,bestMatch,bestTypedef,bestTemplSpec,
                      bestResolvedType);
  }

  if (pTypeDef) 
  {
    *pTypeDef = bestTypedef;
  }
  if (pTemplSpec)
  {
    *pTemplSpec = bestTemplSpec;
  }
  if (pResolvedType)
  {
    *pResolvedType = bestResolvedType;
  }
  //printf("getResolvedClassRec: bestMatch=%p pval->resolvedType=%s\n",
  //    bestMatch,bestResolvedType.data());

  pval=Doxygen::lookupCache->find(key);
  if (pval)
  {
    pval->classDef     = bestMatch;
    pval->typeDef      = bestTypedef;
    pval->templSpec    = bestTemplSpec;
    pval->resolvedType = bestResolvedType;
  }
  else
  {
    Doxygen::lookupCache->insert(key,new LookupInfo(bestMatch,bestTypedef,bestTemplSpec,bestResolvedType));
  }
  //printf("] bestMatch=%s distance=%d\n",
  //    bestMatch?bestMatch->name().data():"<none>",minDistance);
  //if (pTemplSpec) 
  //  printf("templSpec=%s\n",pTemplSpec->data());
  return bestMatch;
}

/* Find the fully qualified class name referred to by the input class
 * or typedef name against the input scope.
 * Loops through scope and each of its parent scopes looking for a
 * match against the input name. 
 */
const ClassDef *getResolvedClass(const Definition *scope,
    const FileDef *fileScope,
    const char *n,
    const MemberDef **pTypeDef,
    QCString *pTemplSpec,
    bool mayBeUnlinkable,
    bool mayBeHidden,
    QCString *pResolvedType
    )
{
  static bool optimizeOutputVhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  g_resolvedTypedefs.clear();
  if (scope==0 ||
      (scope->definitionType()!=Definition::TypeClass && 
       scope->definitionType()!=Definition::TypeNamespace
      ) ||
      (scope->getLanguage()==SrcLangExt_Java && QCString(n).find("::")!=-1)
     )
  {
    scope=Doxygen::globalScope;
  }
  //printf("------------ getResolvedClass(scope=%s,file=%s,name=%s,mayUnlinkable=%d)\n",
  //    scope?scope->name().data():"<global>",
  //    fileScope?fileScope->name().data():"<none>",
  //    n,
  //    mayBeUnlinkable
  //   );
  const ClassDef *result;
  if (optimizeOutputVhdl)
  {
    result = getClass(n);
  }
  else
  {
    result = getResolvedClassRec(scope,fileScope,n,pTypeDef,pTemplSpec,pResolvedType);
  }
  if (result==0) // for nested classes imported via tag files, the scope may not
                 // present, so we check the class name directly as well.
                 // See also bug701314
  {
    result = getClass(n);
  }
  if (!mayBeUnlinkable && result && !result->isLinkable()) 
  {
    if (!mayBeHidden || !result->isHidden())
    {
      //printf("result was %s\n",result?result->name().data():"<none>");
      result=0; // don't link to artificial/hidden classes unless explicitly allowed
    }
  }
  //printf("getResolvedClass(%s,%s)=%s\n",scope?scope->name().data():"<global>",
  //                                  n,result?result->name().data():"<none>");
  return result;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

static bool findOperator(const QCString &s,int i)
{
  int b = s.findRev("operator",i);
  if (b==-1) return FALSE; // not found
  b+=8;
  while (b<i) // check if there are only spaces in between 
    // the operator and the >
  {
    if (!isspace((uchar)s.at(b))) return FALSE;
    b++;
  }
  return TRUE;
}

static bool findOperator2(const QCString &s,int i)
{
  int b = s.findRev("operator",i);
  if (b==-1) return FALSE; // not found
  b+=8;
  while (b<i) // check if there are only non-ascii
              // characters in front of the operator
  {
    if (isId((uchar)s.at(b))) return FALSE;
    b++;
  }
  return TRUE;
}

static const char constScope[]    = { 'c', 'o', 'n', 's', 't', ':' };
static const char virtualScope[]  = { 'v', 'i', 'r', 't', 'u', 'a', 'l', ':' };
static const char operatorScope[] = { 'o', 'p', 'e', 'r', 'a', 't', 'o', 'r', '?', '?', '?' };

struct CharAroundSpace
{
  CharAroundSpace()
  {
    charMap['('].before=FALSE;
    charMap['='].before=FALSE;
    charMap['&'].before=FALSE;
    charMap['*'].before=FALSE;
    charMap['['].before=FALSE;
    charMap['|'].before=FALSE;
    charMap['+'].before=FALSE;
    charMap[';'].before=FALSE;
    charMap[':'].before=FALSE;
    charMap['/'].before=FALSE;

    charMap['='].after=FALSE;
    charMap[' '].after=FALSE;
    charMap['['].after=FALSE;
    charMap[']'].after=FALSE;
    charMap['\t'].after=FALSE;
    charMap['\n'].after=FALSE;
    charMap[')'].after=FALSE;
    charMap[','].after=FALSE;
    charMap['<'].after=FALSE;
    charMap['|'].after=FALSE;
    charMap['+'].after=FALSE;
    charMap['('].after=FALSE;
    charMap['/'].after=FALSE;
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
  static bool cliSupport = Config_getBool(CPP_CLI_SUPPORT);
  static bool vhdl = Config_getBool(OPTIMIZE_OUTPUT_VHDL);

  if (s.isEmpty() || vhdl) return s;

  // We use a static character array to
  // improve the performance of this function
  static char *growBuf = 0;
  static int growBufLen = 0;
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
  //printf("linkify='%s'\n",text);
  static QRegExp regExp("[a-z_A-Z\\x80-\\xFF][~!a-z_A-Z0-9$\\\\.:\\x80-\\xFF]*");
  static QRegExp regExpSplit("(?!:),");
  QCString txtStr=text;
  int strLen = txtStr.length();
  //printf("linkifyText scope=%s fileScope=%s strtxt=%s strlen=%d external=%d\n",
  //    scope?scope->name().data():"<none>",
  //    fileScope?fileScope->name().data():"<none>",
  //    txtStr.data(),strLen,external);
  int matchLen;
  int index=0;
  int newIndex;
  int skipIndex=0;
  int floatingIndex=0;
  if (strLen==0) return;
  // read a word from the text string
  while ((newIndex=regExp.match(txtStr,index,&matchLen))!=-1)
  {
    floatingIndex+=newIndex-skipIndex+matchLen;
    if (newIndex>0 && txtStr.at(newIndex-1)=='0') // ignore hex numbers (match x00 in 0x00)
    {
      out.writeString(txtStr.mid(skipIndex,newIndex+matchLen-skipIndex),keepSpaces);
      skipIndex=index=newIndex+matchLen;
      continue;
    }

    // add non-word part to the result
    bool insideString=FALSE; 
    int i;
    for (i=index;i<newIndex;i++) 
    { 
      if (txtStr.at(i)=='"') insideString=!insideString; 
    }

    //printf("floatingIndex=%d strlen=%d autoBreak=%d\n",floatingIndex,strLen,autoBreak);
    if (strLen>35 && floatingIndex>30 && autoBreak) // try to insert a split point
    {
      QCString splitText = txtStr.mid(skipIndex,newIndex-skipIndex);
      int splitLength = splitText.length();
      int offset=1;
      i=splitText.find(regExpSplit,0);
      if (i==-1) { i=splitText.find('<'); if (i!=-1) offset=0; }
      if (i==-1) i=splitText.find('>');
      if (i==-1) i=splitText.find(' ');
      //printf("splitText=[%s] len=%d i=%d offset=%d\n",splitText.data(),splitLength,i,offset);
      if (i!=-1) // add a link-break at i in case of Html output
      {
        out.writeString(splitText.left(i+offset),keepSpaces);
        out.writeBreak(indentLevel==0 ? 0 : indentLevel+1);
        out.writeString(splitText.right(splitLength-i-offset),keepSpaces);
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
      out.writeString(txtStr.mid(skipIndex,newIndex-skipIndex),keepSpaces); 
    }
    // get word from string
    QCString word=txtStr.mid(newIndex,matchLen);
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

      const MemberDef *typeDef=0;
      cd=getResolvedClass(scope,fileScope,matchWord,&typeDef);
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
                word);
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
            out.writeLink(cd->getReference(),cd->getOutputFileBase(),cd->anchor(),word);
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
            out.writeLink(cd->getReference(),cd->getOutputFileBase(),cd->anchor(),word);
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
                md->anchor(),word);
            //printf("found symbol %s\n",matchWord.data());
            found=TRUE;
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
  out.writeString(txtStr.right(txtStr.length()-skipIndex),keepSpaces);
}


void writeExample(OutputList &ol,ExampleSDict *ed)
{
  QCString exampleLine=theTranslator->trWriteList(ed->count());

  //bool latexEnabled = ol.isEnabled(OutputGenerator::Latex);
  //bool manEnabled   = ol.isEnabled(OutputGenerator::Man);
  //bool htmlEnabled  = ol.isEnabled(OutputGenerator::Html);
  QRegExp marker("@[0-9]+");
  int index=0,newIndex,matchLen;
  // now replace all markers in inheritLine with links to the classes
  while ((newIndex=marker.match(exampleLine,index,&matchLen))!=-1)
  {
    bool ok;
    ol.parseText(exampleLine.mid(index,newIndex-index));
    uint entryIndex = exampleLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
    Example *e=ed->at(entryIndex);
    if (ok && e) 
    {
      ol.pushGeneratorState();
      //if (latexEnabled) ol.disable(OutputGenerator::Latex);
      ol.disable(OutputGenerator::Latex);
      ol.disable(OutputGenerator::RTF);
      ol.disable(OutputGenerator::Docbook);
      // link for Html / man
      //printf("writeObjectLink(file=%s)\n",e->file.data());
      ol.writeObjectLink(0,e->file,e->anchor,e->name);
      ol.popGeneratorState();

      ol.pushGeneratorState();
      //if (latexEnabled) ol.enable(OutputGenerator::Latex);
      ol.disable(OutputGenerator::Man);
      ol.disable(OutputGenerator::Html);
      // link for Latex / pdf with anchor because the sources
      // are not hyperlinked (not possible with a verbatim environment).
      ol.writeObjectLink(0,e->file,0,e->name);
      //if (manEnabled) ol.enable(OutputGenerator::Man);
      //if (htmlEnabled) ol.enable(OutputGenerator::Html);
      ol.popGeneratorState();
    }
    index=newIndex+matchLen;
  } 
  ol.parseText(exampleLine.right(exampleLine.length()-index));
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
  if (al.constSpecifier) result+=" const";
  if (al.volatileSpecifier) result+=" volatile";
  if (al.refQualifier==RefQualifierLValue) result+=" &";
  else if (al.refQualifier==RefQualifierRValue) result+=" &&";
  if (!al.trailingReturnType.isEmpty()) result+=" -> "+al.trailingReturnType;
  if (al.pureSpecifier) result+=" =0";
  return removeRedundantWhiteSpace(result);
}

QCString tempArgListToString(const ArgumentList &al,SrcLangExt lang)
{
  QCString result;
  if (al.empty()) return result;
  result="<";
  auto it = al.begin();
  while (it!=al.end())
  {
    Argument a = *it;
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
    ++it;
    if (it!=al.end()) result+=", ";
  }
  result+=">";
  return removeRedundantWhiteSpace(result);
}


// compute the HTML anchors for a list of members
void setAnchors(MemberList *ml)
{
  //int count=0;
  if (ml==0) return;
  MemberListIterator mli(*ml);
  MemberDef *md;
  for (;(md=mli.current());++mli)
  {
    if (!md->isReference())
    {
      //QCString anchor;
      //if (groupId==-1)
      //  anchor.sprintf("%c%d",id,count++);
      //else
      //  anchor.sprintf("%c%d_%d",id,groupId,count++);
      //if (cd) anchor.prepend(escapeCharsInString(cd->name(),FALSE));
      md->setAnchor();
      //printf("setAnchors(): Member %s outputFileBase=%s anchor %s result %s\n",
      //    md->name().data(),md->getOutputFileBase().data(),anchor.data(),md->anchor().data());
    }
  }
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

static QCString getFilterFromList(const char *name,const QStrList &filterList,bool &found)
{
  found=FALSE;
  // compare the file name to the filter pattern list
  QStrListIterator sli(filterList);
  char* filterStr;
  for (sli.toFirst(); (filterStr = sli.current()); ++sli)
  {
    QCString fs = filterStr;
    int i_equals=fs.find('=');
    if (i_equals!=-1)
    {
      QCString filterPattern = fs.left(i_equals);
      QRegExp fpat(filterPattern,portable_fileSystemIsCaseSensitive(),TRUE); 
      if (fpat.match(name)!=-1) 
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

  QStrList& filterSrcList = Config_getList(FILTER_SOURCE_PATTERNS);
  QStrList& filterList    = Config_getList(FILTER_PATTERNS);

  QCString filterName;
  bool found=FALSE;
  if (isSourceCode && !filterSrcList.isEmpty())
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
      QCString contents(bSize);
      int totalSize=0;
      int size;
      while ((size=f.readBlock(contents.rawData()+totalSize,bSize))==bSize)
      {
        totalSize+=bSize;
        contents.resize(totalSize+bSize);
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

static QDateTime getCurrentDateTime()
{
  QDateTime current = QDateTime::currentDateTime();
  QCString sourceDateEpoch = portable_getenv("SOURCE_DATE_EPOCH");
  if (!sourceDateEpoch.isEmpty())
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
    else if (epoch>UINT_MAX)
    {
      static bool warnedOnce=FALSE;
      if (!warnedOnce)
      {
        warn_uncond("Environment variable SOURCE_DATE_EPOCH must have a value smaller than or equal to %llu; actual value %llu\n",UINT_MAX,epoch);
        warnedOnce=TRUE;
      }
    }
    else // all ok, replace current time with epoch value
    {
      current.setTimeUtc_t((ulong)epoch); // TODO: add support for 64bit epoch value
    }
  }
  return current;
}

QCString dateToString(bool includeTime)
{
  const QDateTime current = getCurrentDateTime();
  return theTranslator->trDateTime(current.date().year(),
                                   current.date().month(),
                                   current.date().day(),
                                   current.date().dayOfWeek(),
                                   current.time().hour(),
                                   current.time().minute(),
                                   current.time().second(),
                                   includeTime);
}

QCString yearToString()
{
  const QDateTime current = getCurrentDateTime();
  QCString result;
  result.sprintf("%d", current.date().year());
  return result;
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
  if (cd->baseClasses())
  {
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcdi;
    for (;(bcdi=bcli.current());++bcli)
    {
      int mc=minClassDistance(bcdi->classDef,bcd,level+1);
      if (mc<m) m=mc;
      if (m<0) break;
    }
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
  else if (cd->baseClasses())
  {
    BaseClassListIterator bcli(*cd->baseClasses());
    const BaseClassDef *bcdi;
    for (;(bcdi=bcli.current()) && prot!=Private;++bcli)
    {
      Protection baseProt = classInheritedProtectionLevel(bcdi->classDef,bcd,bcdi->prot,level+1);
      if (baseProt==Private)   prot=Private;
      else if (baseProt==Protected) prot=Protected;
    }
  }
exit:
  //printf("classInheritedProtectionLevel(%s,%s)=%d\n",cd->name().data(),bcd->name().data(),prot);
  return prot;
}

#ifndef NEWMATCH
// strip any template specifiers that follow className in string s
static QCString trimTemplateSpecifiers(
    const QCString &namespaceName,
    const QCString &className,
    const QCString &s
    )
{
  //printf("trimTemplateSpecifiers(%s,%s,%s)\n",namespaceName.data(),className.data(),s.data());
  QCString scopeName=mergeScopes(namespaceName,className);
  ClassDef *cd=getClass(scopeName);
  if (cd==0) return s; // should not happen, but guard anyway.

  QCString result=s;

  int i=className.length()-1;
  if (i>=0 && className.at(i)=='>') // template specialization
  {
    // replace unspecialized occurrences in s, with their specialized versions.
    int count=1;
    int cl=i+1;
    while (i>=0)
    {
      char c=className.at(i);
      if (c=='>') count++,i--;
      else if (c=='<') { count--; if (count==0) break; }
      else i--;
    }
    QCString unspecClassName=className.left(i);
    int l=i;
    int p=0;
    while ((i=result.find(unspecClassName,p))!=-1)
    {
      if (result.at(i+l)!='<') // unspecialized version
      {
        result=result.left(i)+className+result.right(result.length()-i-l);
        l=cl;
      }
      p=i+l;
    }
  }

  //printf("result after specialization: %s\n",result.data());

  QCString qualName=cd->qualifiedNameWithTemplateParameters();
  //printf("QualifiedName = %s\n",qualName.data());
  // We strip the template arguments following className (if any)
  if (!qualName.isEmpty()) // there is a class name
  {
    int is,ps=0;
    int p=0,l,i;

    while ((is=getScopeFragment(qualName,ps,&l))!=-1)
    {
      QCString qualNamePart = qualName.right(qualName.length()-is);
      //printf("qualNamePart=%s\n",qualNamePart.data());
      while ((i=result.find(qualNamePart,p))!=-1)
      {
        int ql=qualNamePart.length();
        result=result.left(i)+cd->name()+result.right(result.length()-i-ql);
        p=i+cd->name().length();
      }
      ps=is+l;
    }
  }
  //printf("result=%s\n",result.data());

  return result.stripWhiteSpace();
}

/*!
 * @param pattern pattern to look for
 * @param s string to search in
 * @param p position to start
 * @param len resulting pattern length
 * @returns position on which string is found, or -1 if not found
 */
static int findScopePattern(const QCString &pattern,const QCString &s,
    int p,int *len)
{
  int sl=s.length();
  int pl=pattern.length();
  int sp=0; 
  *len=0;
  while (p<sl)
  {
    sp=p; // start of match
    int pp=0; // pattern position
    while (p<sl && pp<pl)
    {
      if (s.at(p)=='<') // skip template arguments while matching
      {
        int bc=1;
        //printf("skipping pos=%d c=%c\n",p,s.at(p));
        p++;
        while (p<sl)
        {
          if (s.at(p)=='<') bc++;
          else if (s.at(p)=='>') 
          {
            bc--;
            if (bc==0) 
            {
              p++;
              break;
            }
          }
          //printf("skipping pos=%d c=%c\n",p,s.at(p));
          p++;
        }
      }
      else if (s.at(p)==pattern.at(pp))
      {
        //printf("match at position p=%d pp=%d c=%c\n",p,pp,s.at(p));
        p++;
        pp++;
      }
      else // no match
      {
        //printf("restarting at %d c=%c pat=%s\n",p,s.at(p),pattern.data());
        p=sp+1;
        break;
      }
    }
    if (pp==pl) // whole pattern matches
    {
      *len=p-sp;
      return sp;
    }
  }
  return -1;
}

static QCString trimScope(const QCString &name,const QCString &s)
{
  int scopeOffset=name.length();
  QCString result=s;
  do // for each scope
  {
    QCString tmp;
    QCString scope=name.left(scopeOffset)+"::";
    //printf("Trying with scope='%s'\n",scope.data());

    int i,p=0,l;
    while ((i=findScopePattern(scope,result,p,&l))!=-1) // for each occurrence
    {
      tmp+=result.mid(p,i-p); // add part before pattern
      p=i+l;
    }
    tmp+=result.right(result.length()-p); // add trailing part

    scopeOffset=name.findRev("::",scopeOffset-1);
    result = tmp;
  } while (scopeOffset>0);   
  //printf("trimScope(name=%s,scope=%s)=%s\n",name.data(),s.data(),result.data());
  return result;
}
#endif

void trimBaseClassScope(BaseClassList *bcl,QCString &s,int level=0)
{
  //printf("trimBaseClassScope level=%d '%s'\n",level,s.data());
  BaseClassListIterator bcli(*bcl);
  BaseClassDef *bcd;
  for (;(bcd=bcli.current());++bcli)
  {
    ClassDef *cd=bcd->classDef;
    //printf("Trying class %s\n",cd->name().data());
    int spos=s.find(cd->name()+"::");
    if (spos!=-1)
    {
      s = s.left(spos)+s.right(
          s.length()-spos-cd->name().length()-2
          );
    }
    //printf("base class '%s'\n",cd->name().data());
    if (cd->baseClasses())
      trimBaseClassScope(cd->baseClasses(),s,level+1); 
  }
}

#if 0
/*! if either t1 or t2 contains a namespace scope, then remove that
 *  scope. If neither or both have a namespace scope, t1 and t2 remain
 *  unchanged.
 */
static void trimNamespaceScope(QCString &t1,QCString &t2,const QCString &nsName)
{
  int p1=t1.length();
  int p2=t2.length();
  for (;;)
  {
    int i1=p1==0 ? -1 : t1.findRev("::",p1);
    int i2=p2==0 ? -1 : t2.findRev("::",p2);
    if (i1==-1 && i2==-1)
    {
      return;
    }
    if (i1!=-1 && i2==-1) // only t1 has a scope
    {
      QCString scope=t1.left(i1);
      replaceNamespaceAliases(scope,i1);

      int so=nsName.length();
      do
      {
        QCString fullScope=nsName.left(so);
        if (!fullScope.isEmpty() && !scope.isEmpty()) fullScope+="::";
        fullScope+=scope;
        if (!fullScope.isEmpty() && Doxygen::namespaceSDict[fullScope]!=0) // scope is a namespace
        {
          t1 = t1.right(t1.length()-i1-2);
          return;
        }
        if (so==0)
        {
          so=-1;
        }
        else if ((so=nsName.findRev("::",so-1))==-1)
        {
          so=0;
        }
      }
      while (so>=0);
    }
    else if (i1==-1 && i2!=-1) // only t2 has a scope
    {
      QCString scope=t2.left(i2);
      replaceNamespaceAliases(scope,i2);

      int so=nsName.length();
      do
      {
        QCString fullScope=nsName.left(so);
        if (!fullScope.isEmpty() && !scope.isEmpty()) fullScope+="::";
        fullScope+=scope;
        if (!fullScope.isEmpty() && Doxygen::namespaceSDict[fullScope]!=0) // scope is a namespace
        {
          t2 = t2.right(t2.length()-i2-2);
          return;
        }
        if (so==0)
        {
          so=-1;
        }
        else if ((so=nsName.findRev("::",so-1))==-1)
        {
          so=0;
        }
      }
      while (so>=0);
    }
    p1 = QMAX(i1-2,0);
    p2 = QMAX(i2-2,0);
  }
}
#endif

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

#ifndef NEWMATCH
static bool matchArgument(const Argument *srcA,const Argument *dstA,
    const QCString &className,
    const QCString &namespaceName,
    NamespaceSDict *usingNamespaces,
    SDict<Definition> *usingClasses)
{
  //printf("match argument start '%s|%s' <-> '%s|%s' using nsp=%p class=%p\n",
  //    srcA->type.data(),srcA->name.data(),
  //    dstA->type.data(),dstA->name.data(),
  //    usingNamespaces,
  //    usingClasses);

  // TODO: resolve any typedefs names that are part of srcA->type
  //       before matching. This should use className and namespaceName
  //       and usingNamespaces and usingClass to determine which typedefs
  //       are in-scope, so it will not be very efficient :-(

  QCString srcAType=trimTemplateSpecifiers(namespaceName,className,srcA->type);
  QCString dstAType=trimTemplateSpecifiers(namespaceName,className,dstA->type);
  QCString srcAName=srcA->name.stripWhiteSpace();
  QCString dstAName=dstA->name.stripWhiteSpace();
  srcAType.stripPrefix("class ");
  dstAType.stripPrefix("class ");

  // allow distinguishing "const A" from "const B" even though 
  // from a syntactic point of view they would be two names of the same 
  // type "const". This is not fool prove of course, but should at least 
  // catch the most common cases.
  if ((srcAType=="const" || srcAType=="volatile") && !srcAName.isEmpty())
  {
    srcAType+=" ";
    srcAType+=srcAName;
  } 
  if ((dstAType=="const" || dstAType=="volatile") && !dstAName.isEmpty())
  {
    dstAType+=" ";
    dstAType+=dstAName;
  }
  if (srcAName=="const" || srcAName=="volatile")
  {
    srcAType+=srcAName;
    srcAName.resize(0);
  }
  else if (dstA->name=="const" || dstA->name=="volatile")
  {
    dstAType+=dstA->name;
    dstAName.resize(0);
  }

  stripIrrelevantConstVolatile(srcAType);
  stripIrrelevantConstVolatile(dstAType);

  // strip typename keyword
  if (qstrncmp(srcAType,"typename ",9)==0)
  {
    srcAType = srcAType.right(srcAType.length()-9); 
  }
  if (qstrncmp(dstAType,"typename ",9)==0)
  {
    dstAType = dstAType.right(dstAType.length()-9); 
  }

  srcAType = removeRedundantWhiteSpace(srcAType);
  dstAType = removeRedundantWhiteSpace(dstAType);

  //srcAType=stripTemplateSpecifiersFromScope(srcAType,FALSE);
  //dstAType=stripTemplateSpecifiersFromScope(dstAType,FALSE);

  //printf("srcA='%s|%s' dstA='%s|%s'\n",srcAType.data(),srcAName.data(),
  //      dstAType.data(),dstAName.data());

  if (srcA->array!=dstA->array) // nomatch for char[] against char
  {
    NOMATCH
      return FALSE;
  }
  if (srcAType!=dstAType) // check if the argument only differs on name 
  {

    // remove a namespace scope that is only in one type 
    // (assuming a using statement was used)
    //printf("Trimming %s<->%s: %s\n",srcAType.data(),dstAType.data(),namespaceName.data());
    //trimNamespaceScope(srcAType,dstAType,namespaceName);
    //printf("After Trimming %s<->%s\n",srcAType.data(),dstAType.data());

    //QCString srcScope;
    //QCString dstScope;

    // strip redundant scope specifiers
    if (!className.isEmpty())
    {
      srcAType=trimScope(className,srcAType);
      dstAType=trimScope(className,dstAType);
      //printf("trimScope: '%s' <=> '%s'\n",srcAType.data(),dstAType.data());
      ClassDef *cd;
      if (!namespaceName.isEmpty())
        cd=getClass(namespaceName+"::"+className);
      else
        cd=getClass(className);
      if (cd && cd->baseClasses())
      {
        trimBaseClassScope(cd->baseClasses(),srcAType); 
        trimBaseClassScope(cd->baseClasses(),dstAType); 
      }
      //printf("trimBaseClassScope: '%s' <=> '%s'\n",srcAType.data(),dstAType.data());
    }
    if (!namespaceName.isEmpty())
    {
      srcAType=trimScope(namespaceName,srcAType);
      dstAType=trimScope(namespaceName,dstAType);
    }
    //printf("#usingNamespace=%d\n",usingNamespaces->count());
    if (usingNamespaces && usingNamespaces->count()>0)
    {
      NamespaceSDict::Iterator nli(*usingNamespaces);
      NamespaceDef *nd;
      for (;(nd=nli.current());++nli)
      {
        srcAType=trimScope(nd->name(),srcAType);
        dstAType=trimScope(nd->name(),dstAType);
      }
    }
    //printf("#usingClasses=%d\n",usingClasses->count());
    if (usingClasses && usingClasses->count()>0)
    {
      SDict<Definition>::Iterator cli(*usingClasses);
      Definition *cd;
      for (;(cd=cli.current());++cli)
      {
        srcAType=trimScope(cd->name(),srcAType);
        dstAType=trimScope(cd->name(),dstAType);
      }
    }

    //printf("2. srcA=%s|%s dstA=%s|%s\n",srcAType.data(),srcAName.data(),
    //    dstAType.data(),dstAName.data());

    if (!srcAName.isEmpty() && !dstA->type.isEmpty() &&
        (srcAType+" "+srcAName)==dstAType)
    {
      MATCH
      return TRUE;
    }
    else if (!dstAName.isEmpty() && !srcA->type.isEmpty() &&
        (dstAType+" "+dstAName)==srcAType)
    {
      MATCH
      return TRUE;
    }


    uint srcPos=0,dstPos=0; 
    bool equal=TRUE;
    while (srcPos<srcAType.length() && dstPos<dstAType.length() && equal)
    {
      equal=srcAType.at(srcPos)==dstAType.at(dstPos);
      if (equal) srcPos++,dstPos++; 
    }
    uint srcATypeLen=srcAType.length();
    uint dstATypeLen=dstAType.length();
    if (srcPos<srcATypeLen && dstPos<dstATypeLen)
    {
      // if nothing matches or the match ends in the middle or at the
      // end of a string then there is no match
      if (srcPos==0 || dstPos==0) 
      {
        NOMATCH
        return FALSE;
      }
      if (isId(srcAType.at(srcPos)) && isId(dstAType.at(dstPos)))
      {
        //printf("partial match srcPos=%d dstPos=%d!\n",srcPos,dstPos);
        // check if a name if already found -> if no then there is no match
        if (!srcAName.isEmpty() || !dstAName.isEmpty()) 
        {
          NOMATCH
          return FALSE;
        }
        // types only
        while (srcPos<srcATypeLen && isId(srcAType.at(srcPos))) srcPos++;
        while (dstPos<dstATypeLen && isId(dstAType.at(dstPos))) dstPos++;
        if (srcPos<srcATypeLen || 
            dstPos<dstATypeLen ||
            (srcPos==srcATypeLen && dstPos==dstATypeLen)
           ) 
        {
          NOMATCH
          return FALSE;
        }
      }
      else
      {
        // otherwise we assume that a name starts at the current position.
        while (srcPos<srcATypeLen && isId(srcAType.at(srcPos))) srcPos++;
        while (dstPos<dstATypeLen && isId(dstAType.at(dstPos))) dstPos++;

        // if nothing more follows for both types then we assume we have
        // found a match. Note that now 'signed int' and 'signed' match, but
        // seeing that int is not a name can only be done by looking at the
        // semantics.

        if (srcPos!=srcATypeLen || dstPos!=dstATypeLen) 
        { 
          NOMATCH
          return FALSE; 
        }
      }
    }
    else if (dstPos<dstAType.length())
    {
      if (!isspace((uchar)dstAType.at(dstPos))) // maybe the names differ
      {
        if (!dstAName.isEmpty()) // dst has its name separated from its type
        {
          NOMATCH
          return FALSE;
        }
        while (dstPos<dstAType.length() && isId(dstAType.at(dstPos))) dstPos++;
        if (dstPos!=dstAType.length()) 
        {
          NOMATCH
          return FALSE; // more than a difference in name -> no match
        }
      }
      else  // maybe dst has a name while src has not
      {
        dstPos++;
        while (dstPos<dstAType.length() && isId(dstAType.at(dstPos))) dstPos++;
        if (dstPos!=dstAType.length() || !srcAName.isEmpty()) 
        {
          NOMATCH
          return FALSE; // nope not a name -> no match
        }
      }
    }
    else if (srcPos<srcAType.length())
    {
      if (!isspace((uchar)srcAType.at(srcPos))) // maybe the names differ
      {
        if (!srcAName.isEmpty()) // src has its name separated from its type
        {
          NOMATCH
          return FALSE;
        }
        while (srcPos<srcAType.length() && isId(srcAType.at(srcPos))) srcPos++;
        if (srcPos!=srcAType.length()) 
        {
          NOMATCH
          return FALSE; // more than a difference in name -> no match
        }
      }
      else // maybe src has a name while dst has not
      {
        srcPos++;
        while (srcPos<srcAType.length() && isId(srcAType.at(srcPos))) srcPos++;
        if (srcPos!=srcAType.length() || !dstAName.isEmpty()) 
        {
          NOMATCH
          return FALSE; // nope not a name -> no match
        }
      }
    }
  }
  MATCH
  return TRUE;
}

#endif

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
  //DefinitionList *defList=0;
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
  //printf("getCanonicalTypeForIdentifier(%s,[%s->%s]) start\n",
  //    word.data(),tSpec?tSpec->data():"<none>",templSpec.data());

  const ClassDef *cd = 0;
  const MemberDef *mType = 0;
  QCString ts;
  QCString resolvedType;

  // lookup class / class template instance
  cd = getResolvedClass(d,fs,word+templSpec,&mType,&ts,TRUE,TRUE,&resolvedType);
  bool isTemplInst = cd && !templSpec.isEmpty();
  if (!cd && !templSpec.isEmpty())
  {
    // class template specialization not known, look up class template
    cd = getResolvedClass(d,fs,word,&mType,&ts,TRUE,TRUE,&resolvedType);
  }
  if (cd && cd->isUsedOnly()) cd=0; // ignore types introduced by usage relations

  //printf("cd=%p mtype=%p\n",cd,mType);
  //printf("  getCanonicalTypeForIdentifer: symbol=%s word=%s cd=%s d=%s fs=%s cd->isTemplate=%d\n",
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
      result = getCanonicalTypeForIdentifier(d,fs,mType->typeString(),tSpec,count+1);
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

  //static QRegExp id("[a-z_A-Z\\x80-\\xFF][:a-z_A-Z0-9\\x80-\\xFF]*");

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
      static QRegExp re("[a-z_A-Z\\x80-\\xFF][a-z_A-Z0-9\\x80-\\xFF]*");
      int tp=0,tl,ti;
      // for each identifier template specifier
      //printf("adding resolved %s to %s\n",templSpec.data(),canType.data());
      while ((ti=re.match(templSpec,tp,&tl))!=-1)
      {
        canType += templSpec.mid(tp,ti-tp);
        canType += getCanonicalTypeForIdentifier(d,fs,templSpec.mid(ti,tl),0);
        tp=ti+tl;
      }
      canType+=templSpec.right(templSpec.length()-tp);
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
  //    srcA->type.data(),srcA->name.data(),srcA->canType.data(),
  //    dstScope ? dstScope->name().data() : "",
  //    dstA->type.data(),dstA->name.data(),dstA->canType.data());

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

  if (srcA.canType.isEmpty())
  {
    srcA.canType = extractCanonicalArgType(srcScope,srcFileScope,srcA);
  }
  if (dstA.canType.isEmpty())
  {
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
bool matchArguments2(const Definition *srcScope,const FileDef *srcFileScope,const ArgumentList &inSrcAl,
                     const Definition *dstScope,const FileDef *dstFileScope,const ArgumentList &inDstAl,
                     bool checkCV)
{
  ASSERT(srcScope!=0 && dstScope!=0);

  ArgumentList srcAl = inSrcAl;
  ArgumentList dstAl = inDstAl;

  // handle special case with void argument
  if ( srcAl.empty() && dstAl.size()==1 && dstAl.front().type=="void" )
  { // special case for finding match between func() and func(void)
    Argument a;
    a.type = "void";
    srcAl.push_back(a);
    MATCH
    return TRUE;
  }
  if ( dstAl.empty() && srcAl.size()==1 && srcAl.front().type=="void" )
  { // special case for finding match between func(void) and func()
    Argument a;
    a.type = "void";
    dstAl.push_back(a);
    MATCH
    return TRUE;
  }

  if (srcAl.size() != dstAl.size())
  {
    NOMATCH
    return FALSE; // different number of arguments -> no match
  }

  if (checkCV)
  {
    if (srcAl.constSpecifier != dstAl.constSpecifier) 
    {
      NOMATCH
      return FALSE; // one member is const, the other not -> no match
    }
    if (srcAl.volatileSpecifier != dstAl.volatileSpecifier)
    {
      NOMATCH
      return FALSE; // one member is volatile, the other not -> no match
    }
  }

  if (srcAl.refQualifier != dstAl.refQualifier)
  {
    NOMATCH
    return FALSE; // one member is has a different ref-qualifier than the other
  }

  // so far the argument list could match, so we need to compare the types of
  // all arguments.
  auto srcIt = srcAl.begin();
  auto dstIt = dstAl.begin();
  for (;srcIt!=srcAl.end() && dstIt!=dstAl.end();++srcIt,++dstIt)
  {
    Argument &srcA = *srcIt;
    Argument &dstA = *dstIt;
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

static void findMembersWithSpecificName(MemberName *mn,
                                        const char *args,
                                        bool checkStatics,
                                        const FileDef *currentFile,
                                        bool checkCV,
                                        const char *forceTagFile,
                                        QList<MemberDef> &members)
{
  //printf("  Function with global scope name '%s' args='%s'\n",
  //       mn->memberName(),args);
  MemberNameIterator mli(*mn);
  const MemberDef *md = 0;
  for (mli.toFirst();(md=mli.current());++mli)
  {
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
      ArgumentList *argList=0;
      if (args && !md->isDefine() && qstrcmp(args,"()")!=0)
      {
        const ArgumentList &mdAl = md->argumentList();
        ArgumentList argList;
        stringToArgumentList(args,argList);
        match=matchArguments2(
            md->getOuterScope(),fd,mdAl,
            Doxygen::globalScope,fd,argList,
            checkCV);
      }
      if (match && (forceTagFile==0 || md->getReference()==forceTagFile)) 
      {
        //printf("Found match!\n");
        members.append(md);
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
             bool checkCV,
             const char *forceTagFile
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

  MemberName *mn = Doxygen::memberNameSDict->find(mName);
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

      const MemberDef *tmd=0;
      const ClassDef *fcd=getResolvedClass(Doxygen::globalScope,0,className,&tmd);
      if (fcd==0 && className.find('<')!=-1) // try without template specifiers as well
      {
         QCString nameWithoutTemplates = stripTemplateSpecifiersFromScope(className,FALSE);
         fcd=getResolvedClass(Doxygen::globalScope,0,nameWithoutTemplates,&tmd);
      }
      //printf("Trying class scope %s: fcd=%p tmd=%p\n",className.data(),fcd,tmd);
      // todo: fill in correct fileScope!
      if (fcd &&  // is it a documented class
          fcd->isLinkable() 
         )
      {
        //printf("  Found fcd=%p\n",fcd);
        MemberNameIterator mmli(*mn);
        MemberDef *mmd;
        int mdist=maxInheritanceDepth; 
        ArgumentList argList;
        if (args)
        {
          stringToArgumentList(args,argList);
        }
        for (mmli.toFirst();(mmd=mmli.current());++mmli)
        {
          if (!mmd->isStrongEnumValue())
          {
            const ArgumentList &mmdAl = mmd->argumentList();
            bool match=args==0 ||
              matchArguments2(mmd->getOuterScope(),mmd->getFileDef(),mmdAl,
                             fcd,                  fcd->getFileDef(),argList,
                             checkCV);
            //printf("match=%d\n",match);
            if (match)
            {
              ClassDef *mcd=mmd->getClassDef();
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
          for (mmli.toFirst();(mmd=mmli.current());++mmli)
          {
            //if (mmd->isLinkable())
            //{
            ClassDef *mcd=mmd->getClassDef();
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
        const MemberList *tml = tmd->enumFieldList();
        if (tml)
        {
          MemberListIterator tmi(*tml);
          MemberDef *emd;
          for (;(emd=tmi.current());++tmi)
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
    MemberNameIterator mmli(*mn);
    MemberDef *mmd, *fuzzy_mmd = 0;
    ArgumentList argList;
    bool hasEmptyArgs = args && qstrcmp(args, "()") == 0;

    if (args)
    {
      stringToArgumentList(args, argList);
    }

    for (mmli.toFirst(); (mmd = mmli.current()); ++mmli)
    {
      if (!mmd->isLinkable() || (!mmd->isRelated() && !mmd->isForeign()) ||
           !mmd->getClassDef())
      {
        continue;
      }

      if (!args)
      {
        break;
      }

      ArgumentList &mmdAl = mmd->argumentList();
      if (matchArguments2(mmd->getOuterScope(),mmd->getFileDef(),mmdAl,
            Doxygen::globalScope,mmd->getFileDef(),argList,
            checkCV
            )
         )
      {
        break;
      }

      if (!fuzzy_mmd && hasEmptyArgs)
      {
        fuzzy_mmd = mmd;
      }
    }

    mmd = mmd ? mmd : fuzzy_mmd;

    if (mmd && !mmd->isStrongEnumValue())
    {
      md = mmd;
      cd = mmd->getClassDef();
      return TRUE;
    }
  }


  // maybe an namespace, file or group member ?
  //printf("Testing for global symbol scopeName='%s' mScope='%s' :: mName='%s'\n",
  //              scopeName.data(),mScope.data(),mName.data());
  if ((mn=Doxygen::functionNameSDict->find(mName))) // name is known
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
          (fnd=Doxygen::namespaceSDict->find(namespaceName)) &&
          fnd->isLinkable()
         )
      {
        //printf("Symbol inside existing namespace '%s' count=%d\n",
        //    namespaceName.data(),mn->count());
        bool found=FALSE;
        MemberNameIterator mmli(*mn);
        const MemberDef *mmd;
        for (mmli.toFirst();((mmd=mmli.current()) && !found);++mmli)
        {
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
            ArgumentList argList;
            if (args && qstrcmp(args,"()")!=0)
            {
              const ArgumentList &mmdAl = mmd->argumentList();
              stringToArgumentList(args,argList);
              match=matchArguments2(
                  mmd->getOuterScope(),mmd->getFileDef(),mmdAl,
                  fnd,mmd->getFileDef(),argList,
                  checkCV);
            }
            if (match)
            {
              nd=fnd;
              md=mmd;
              found=TRUE;
            }
          }
        }
        if (!found && args && !qstrcmp(args,"()")) 
          // no exact match found, but if args="()" an arbitrary 
          // member will do
        {
          for (mmli.toFirst();((mmd=mmli.current()) && !found);++mmli)
          {
            if (mmd->getNamespaceDef()==fnd /*&& mmd->isLinkable() */ )
            {
              nd=fnd;
              md=mmd;
              found=TRUE;
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
        MemberNameIterator mmli(*mn);
        MemberDef *mmd;
        for (mmli.toFirst();(mmd=mmli.current());++mmli)
        {
          const MemberDef *tmd = mmd->getEnumScope();
          //printf("try member %s tmd=%s\n",mmd->name().data(),tmd?tmd->name().data():"<none>");
          int ni=namespaceName.findRev("::");
          //printf("namespaceName=%s ni=%d\n",namespaceName.data(),ni);
          bool notInNS = tmd && ni==-1 && tmd->getNamespaceDef()==0 && (mScope.isEmpty() || mScope==tmd->name());
          bool sameNS  = tmd && tmd->getNamespaceDef() && namespaceName.left(ni)==tmd->getNamespaceDef()->name();
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
      QList<MemberDef> members;
      // search for matches with strict static checking
      findMembersWithSpecificName(mn,args,TRUE,currentFile,checkCV,forceTagFile,members);
      if (members.count()==0) // nothing found
      {
        // search again without strict static checking
        findMembersWithSpecificName(mn,args,FALSE,currentFile,checkCV,forceTagFile,members);
      }
      //printf("found %d members\n",members.count());
      if (members.count()!=1 && args && !qstrcmp(args,"()"))
      {
        // no exact match found, but if args="()" an arbitrary
        // member will do
        MemberNameIterator mni(*mn);
        for (mni.toLast();(md=mni.current());--mni)
        {
          //printf("Found member '%s'\n",md->name().data());
          //printf("member is linkable md->name()='%s'\n",md->name().data());
          fd=md->getFileDef();
          gd=md->getGroupDef();
          const MemberDef *tmd = md->getEnumScope();
          if (
              (gd && gd->isLinkable()) || (fd && fd->isLinkable()) ||
              (tmd && tmd->isStrong())
             )
          {
            members.append(md);
          }
        }
      }
      //printf("found %d candidate members\n",members.count());
      if (members.count()>0) // at least one match
      {
        if (currentFile)
        {
          //printf("multiple results; pick one from file:%s\n", currentFile->name().data());
          QListIterator<MemberDef> mit(members);
          for (mit.toFirst();(md=mit.current());++mit)
          {
            if (md->getFileDef() && md->getFileDef()->name() == currentFile->name()) 
            {
              break; // found match in the current file
            }
          }
          if (!md) // member not in the current file
          {
            md=members.getLast();
          }
        }
        else
        {
          md=members.getLast();
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
    else if ((nd=Doxygen::namespaceSDict->find(fullName)) && nd->isLinkable())
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
  else if (inSeeBlock && !nameStr.isEmpty() && (gd=Doxygen::groupSDict->find(nameStr)))
  { // group link
    *resContext=gd;
    return TRUE;
  }
  else if (tsName.find('.')!=-1) // maybe a link to a file
  {
    bool ambig;
    fd=findFileDef(Doxygen::inputNameDict,tsName,ambig);
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
  SectionInfo *si=0;
  bool ambig;
  if (linkRef.isEmpty()) // no reference name!
  {
    return FALSE;
  }
  else if ((pd=Doxygen::pageSDict->find(linkRef))) // link to a page
  {
    const GroupDef *gd = pd->getGroupDef();
    if (gd)
    {
      if (!pd->name().isEmpty()) si=Doxygen::sectionDict->find(pd->name());
      *resContext=gd;
      if (si) resAnchor = si->label;
    }
    else
    {
      *resContext=pd;
    }
    return TRUE;
  }
  else if ((si=Doxygen::sectionDict->find(linkRef)))
  {
    *resContext=si->definition;
    resAnchor = si->label;
    return TRUE;
  }
  else if ((pd=Doxygen::exampleSDict->find(linkRef))) // link to an example
  {
    *resContext=pd;
    return TRUE;
  }
  else if ((gd=Doxygen::groupSDict->find(linkRef))) // link to a group
  {
    *resContext=gd;
    return TRUE;
  }
  else if ((fd=findFileDef(Doxygen::inputNameDict,linkRef,ambig)) // file link
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
  else if ((nd=Doxygen::namespaceSDict->find(linkRef)))
  {
    *resContext=nd;
    return TRUE;
  }
  else if ((dir=Doxygen::directories->find(QFileInfo(linkRef).absFilePath().utf8()+"/"))
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
        linkText=(dynamic_cast<const GroupDef *>(compound))->groupTitle(); // use group's title as link
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
  if ((fd=findFileDef(Doxygen::inputNameDict,name,ambig)) && 
      fd->isLinkable()) 
    // link to documented input file
    od.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,linkText);
  else
    od.docify(linkText); 
}

//----------------------------------------------------------------------

#if 0
QCString substituteClassNames(const QCString &s)
{
  int i=0,l,p;
  QCString result;
  if (s.isEmpty()) return result;
  QRegExp r("[a-z_A-Z][a-z_A-Z0-9]*");
  while ((p=r.match(s,i,&l))!=-1)
  {
    QCString *subst;
    if (p>i) result+=s.mid(i,p-i);
    if ((subst=substituteDict[s.mid(p,l)]))
    {
      result+=*subst;
    }
    else
    {
      result+=s.mid(p,l);
    }
    i=p+l;
  }
  result+=s.mid(i,s.length()-i);
  return result;
}
#endif

//----------------------------------------------------------------------

/** Cache element for the file name to FileDef mapping cache. */
struct FindFileCacheElem
{
  FindFileCacheElem(FileDef *fd,bool ambig) : fileDef(fd), isAmbig(ambig) {}
  FileDef *fileDef;
  bool isAmbig;
};

static QCache<FindFileCacheElem> g_findFileDefCache(5000);

FileDef *findFileDef(const FileNameDict *fnDict,const char *n,bool &ambig)
{
  ambig=FALSE;
  if (n==0) return 0;

  const int maxAddrSize = 20;
  char addr[maxAddrSize];
  qsnprintf(addr,maxAddrSize,"%p:",fnDict);
  QCString key = addr;
  key+=n;

  g_findFileDefCache.setAutoDelete(TRUE);
  FindFileCacheElem *cachedResult = g_findFileDefCache.find(key);
  //printf("key=%s cachedResult=%p\n",key.data(),cachedResult);
  if (cachedResult)
  {
    ambig = cachedResult->isAmbig;
    //printf("cached: fileDef=%p\n",cachedResult->fileDef);
    return cachedResult->fileDef;
  }
  else
  {
    cachedResult = new FindFileCacheElem(0,FALSE);
  }

  QCString name=QDir::cleanDirPath(n).utf8();
  QCString path;
  int slashPos;
  FileName *fn;
  if (name.isEmpty()) goto exit;
  slashPos=QMAX(name.findRev('/'),name.findRev('\\'));
  if (slashPos!=-1)
  {
    path=name.left(slashPos+1);
    name=name.right(name.length()-slashPos-1); 
    //printf("path=%s name=%s\n",path.data(),name.data());
  }
  if (name.isEmpty()) goto exit;
  if ((fn=(*fnDict)[name]))
  {
    //printf("fn->count()=%d\n",fn->count());
    if (fn->count()==1)
    {
      FileDef *fd = fn->getFirst();
#if defined(_WIN32) || defined(__MACOSX__) || defined(__CYGWIN__) // Windows or MacOSX
      bool isSamePath = fd->getPath().right(path.length()).lower()==path.lower();
#else // Unix
      bool isSamePath = fd->getPath().right(path.length())==path;
#endif
      if (path.isEmpty() || isSamePath)
      {
        cachedResult->fileDef = fd;
        g_findFileDefCache.insert(key,cachedResult);
        //printf("=1 ===> add to cache %p\n",fd);
        return fd;
      }
    }
    else // file name alone is ambiguous
    {
      int count=0;
      FileNameIterator fni(*fn);
      FileDef *fd;
      FileDef *lastMatch=0;
      QCString pathStripped = stripFromIncludePath(path);
      for (fni.toFirst();(fd=fni.current());++fni)
      {
        QCString fdStripPath = stripFromIncludePath(fd->getPath());
        if (path.isEmpty() || fdStripPath.right(pathStripped.length())==pathStripped) 
        { 
          count++; 
          lastMatch=fd; 
        }
      }
      //printf(">1 ===> add to cache %p\n",fd);

      ambig=(count>1);
      cachedResult->isAmbig = ambig;
      cachedResult->fileDef = lastMatch;
      g_findFileDefCache.insert(key,cachedResult);
      return lastMatch;
    }
  }
  else
  {
    //printf("not found!\n");
  }
exit:
  //printf("0  ===> add to cache %p: %s\n",cachedResult,n);
  g_findFileDefCache.insert(key,cachedResult);
  //delete cachedResult;
  return 0;
}

//----------------------------------------------------------------------

QCString showFileDefMatches(const FileNameDict *fnDict,const char *n)
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
  FileName *fn;
  if ((fn=(*fnDict)[name]))
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (fni.toFirst();(fd=fni.current());++fni)
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

/// substitute all occurrences of \a src in \a s by \a dst
QCString substitute(const QCString &s,const QCString &src,const QCString &dst)
{
  if (s.isEmpty() || src.isEmpty()) return s;
  const char *p, *q;
  int srcLen = src.length();
  int dstLen = dst.length();
  int resLen;
  if (srcLen!=dstLen)
  {
    int count;
    for (count=0, p=s.data(); (q=strstr(p,src))!=0; p=q+srcLen) count++;
    resLen = s.length()+count*(dstLen-srcLen);
  }
  else // result has same size as s
  {
    resLen = s.length();
  }
  QCString result(resLen+1);
  char *r;
  for (r=result.rawData(), p=s; (q=strstr(p,src))!=0; p=q+srcLen)
  {
    int l = (int)(q-p);
    memcpy(r,p,l);
    r+=l;

    if (dst) memcpy(r,dst,dstLen);
    r+=dstLen;
  }
  qstrcpy(r,p);
  //printf("substitute(%s,%s,%s)->%s\n",s,src,dst,result.data());
  return result;
}


/// substitute all occurrences of \a src in \a s by \a dst, but skip
/// each consecutive sequence of \a src where the number consecutive
/// \a src matches \a skip_seq; if \a skip_seq is negative, skip any
/// number of consecutive \a src
QCString substitute(const QCString &s,const QCString &src,const QCString &dst,int skip_seq)
{
  if (s.isEmpty() || src.isEmpty()) return s;
  const char *p, *q;
  int srcLen = src.length();
  int dstLen = dst.length();
  int resLen;
  if (srcLen!=dstLen)
  {
    int count;
    for (count=0, p=s.data(); (q=strstr(p,src))!=0; p=q+srcLen) count++;
    resLen = s.length()+count*(dstLen-srcLen);
  }
  else // result has same size as s
  {
    resLen = s.length();
  }
  QCString result(resLen+1);
  char *r;
  for (r=result.rawData(), p=s; (q=strstr(p,src))!=0; p=q+srcLen)
  {
    // search a consecutive sequence of src
    int seq = 0, skip = 0;
    if (skip_seq)
    {
      for (const char *n=q+srcLen; qstrncmp(n,src,srcLen)==0; seq=1+skip, n+=srcLen)
        ++skip; // number of consecutive src after the current one

      // verify the allowed number of consecutive src to skip
      if (skip_seq > 0 && skip_seq != seq)
        seq = skip = 0;
    }

    // skip a consecutive sequence of src when necessary
    int l = (int)((q + seq * srcLen)-p);
    memcpy(r,p,l);
    r+=l;

    if (skip)
    {
      // skip only the consecutive src found after the current one
      q += skip * srcLen;
      // the next loop will skip the current src, aka (p=q+srcLen)
      continue;
    }

    if (dst) memcpy(r,dst,dstLen);
    r+=dstLen;
  }
  qstrcpy(r,p);
  result.resize(strlen(result.data())+1);
  //printf("substitute(%s,%s,%s)->%s\n",s,src,dst,result.data());
  return result;
}

/// substitute all occurrences of \a srcChar in \a s by \a dstChar
QCString substitute(const QCString &s,char srcChar,char dstChar)
{
  int l=s.length();
  QCString result(l+1);
  char *q=result.rawData();
  if (l>0)
  {
    const char *p=s.data();
    char c;
    while ((c=*p++)) *q++ = (c==srcChar) ? dstChar : c;
  }
  *q='\0';
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
  result = substitute(result,"$doxygenversion",getVersion());
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
  static QStrList &sl = Config_getList(IGNORE_PREFIX);
  char *s = sl.first();
  while (s)
  {
    const char *ps=s;
    const char *pd=name.data();
    int i=0;
    while (*ps!=0 && *pd!=0 && *ps==*pd) ps++,pd++,i++;
    if (*ps==0 && *pd!=0)
    {
      return i;
    }
    s = sl.next();
  }
  return 0;
}

//----------------------------------------------------------------------------

static void initBaseClassHierarchy(BaseClassList *bcl)
{
  if (bcl==0) return;
  BaseClassListIterator bcli(*bcl);
  for ( ; bcli.current(); ++bcli)
  {
    ClassDef *cd=bcli.current()->classDef;
    if (cd->baseClasses()==0) // no base classes => new root
    {
      initBaseClassHierarchy(cd->baseClasses());
    }
    cd->setVisited(FALSE);
  }
}
//----------------------------------------------------------------------------

bool classHasVisibleChildren(const ClassDef *cd)
{
  BaseClassList *bcl;

  if (cd->getLanguage()==SrcLangExt_VHDL) // reverse baseClass/subClass relation
  {
    if (cd->baseClasses()==0) return FALSE;
    bcl=cd->baseClasses();
  }
  else 
  {
    if (cd->subClasses()==0) return FALSE;
    bcl=cd->subClasses();
  }

  BaseClassListIterator bcli(*bcl);
  for ( ; bcli.current() ; ++bcli)
  {
    if (bcli.current()->classDef->isVisibleInHierarchy())
    {
      return TRUE;
    }
  }
  return FALSE;
}


//----------------------------------------------------------------------------

void initClassHierarchy(ClassSDict *cl)
{
  ClassSDict::Iterator cli(*cl);
  ClassDef *cd;
  for ( ; (cd=cli.current()); ++cli)
  {
    cd->setVisited(FALSE);
    initBaseClassHierarchy(cd->baseClasses());
  }
}

//----------------------------------------------------------------------------

bool hasVisibleRoot(const BaseClassList *bcl)
{
  if (bcl)
  {
    BaseClassListIterator bcli(*bcl);
    for ( ; bcli.current(); ++bcli)
    {
      const ClassDef *cd=bcli.current()->classDef;
      if (cd->isVisibleInHierarchy()) return TRUE;
      hasVisibleRoot(cd->baseClasses());
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------

// note that this function is not reentrant due to the use of static growBuf!
QCString escapeCharsInString(const char *name,bool allowDots,bool allowUnderscore)
{
  static bool caseSenseNames = Config_getBool(CASE_SENSE_NAMES);
  static bool allowUnicodeNames = Config_getBool(ALLOW_UNICODE_NAMES);
  static GrowBuf growBuf;
  growBuf.clear();
  if (name==0) return "";
  char c;
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
      default: 
                if (c<0)
                {
                  char ids[5];
                  const unsigned char uc = (unsigned char)c;
                  bool doEscape = TRUE;
                  if (allowUnicodeNames && uc <= 0xf7)
                  {
                    const char* pt = p;
                    ids[ 0 ] = c;
                    int l = 0;
                    if ((uc&0xE0)==0xC0)
                    {
                      l=2; // 11xx.xxxx: >=2 byte character
                    }
                    if ((uc&0xF0)==0xE0)
                    {
                      l=3; // 111x.xxxx: >=3 byte character
                    }
                    if ((uc&0xF8)==0xF0)
                    {
                      l=4; // 1111.xxxx: >=4 byte character
                    }
                    doEscape = l==0;
                    for (int m=1; m<l && !doEscape; ++m)
                    {
                      unsigned char ct = (unsigned char)*pt;
                      if (ct==0 || (ct&0xC0)!=0x80) // invalid unicode character
                      {
                        doEscape=TRUE;
                      }
                      else
                      {
                        ids[ m ] = *pt++;
                      }
                    }
                    if ( !doEscape ) // got a valid unicode character
                    {
                      ids[ l ] = 0;
                      growBuf.addStr( ids );
                      p += l - 1;
                    }
                  }
                  if (doEscape) // not a valid unicode char or escaping needed
                  {
                    static char map[] = "0123456789ABCDEF";
                    unsigned char id = (unsigned char)c;
                    ids[0]='_';
                    ids[1]='x';
                    ids[2]=map[id>>4];
                    ids[3]=map[id&0xF];
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
                  growBuf.addChar(tolower(c)); 
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
               default: // unknown escape, just pass underscore character as-is
                 result+=c;
                 break;
             }
             break;
          default:
            if (!caseSenseNames && c>='a' && c<='z') // lower to upper case escape, _a -> 'A'
            {
              result+=toupper(*p);
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
    static QDict<int> usedNames(10007);
    usedNames.setAutoDelete(TRUE);
    static int count=1;

    int *value=usedNames.find(name);
    int num;
    if (value==0)
    {
      usedNames.insert(name,new int(count));
      num = count++;
    }
    else
    {
      num = *value;
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

#if MAP_ALGO==ALGO_COUNT 
    // old algorithm, has the problem that after regeneration the
    // output can be located in a different dir.
    if (Doxygen::htmlDirMap==0) 
    {
      Doxygen::htmlDirMap=new QDict<int>(100003);
      Doxygen::htmlDirMap->setAutoDelete(TRUE);
    }
    static int curDirNum=0;
    int *dirNum = Doxygen::htmlDirMap->find(result);
    if (dirNum==0) // new name
    {
      Doxygen::htmlDirMap->insert(result,new int(curDirNum)); 
      l1Dir = (curDirNum)&0xf;    // bits 0-3
      l2Dir = (curDirNum>>4)&0xff; // bits 4-11
      curDirNum++;
    }
    else // existing name
    {
      l1Dir = (*dirNum)&0xf;       // bits 0-3
      l2Dir = ((*dirNum)>>4)&0xff; // bits 4-11
    }
#elif MAP_ALGO==ALGO_CRC16
    // second algorithm based on CRC-16 checksum
    int dirNum = qChecksum(result,result.length());
    l1Dir = dirNum&0xf;
    l2Dir = (dirNum>>4)&0xff;
#elif MAP_ALGO==ALGO_MD5
    // third algorithm based on MD5 hash
    uchar md5_sig[16];
    MD5Buffer((const unsigned char *)result.data(),result.length(),md5_sig);
    l1Dir = md5_sig[14]&0xf;
    l2Dir = md5_sig[15];
#endif
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
      d.mkdir(QCString().sprintf("d%x",l1));
      for (l2=0;l2<256;l2++)
      {
        d.mkdir(QCString().sprintf("d%x/d%02x",l1,l2));
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

#if 0 // original version
/*! Strips the scope from a name. Examples: A::B will return A
 *  and A<T>::B<N::C<D> > will return A<T>.
 */
QCString stripScope(const char *name)
{
  QCString result = name;
  int l=result.length();
  int p=l-1;
  bool done;
  int count;

  while (p>=0)
  {
    char c=result.at(p);
    switch (c)
    {
      case ':': 
        //printf("stripScope(%s)=%s\n",name,result.right(l-p-1).data());
        return result.right(l-p-1);
      case '>':
        count=1;
        done=FALSE;
        //printf("pos < = %d\n",p);
        p--;
        while (p>=0 && !done)
        {
          c=result.at(p--);
          switch (c)
          {
            case '>': count++; break;
            case '<': count--; if (count<=0) done=TRUE; break;
            default: 
                      //printf("c=%c count=%d\n",c,count);
                      break;
          }
        }
        //printf("pos > = %d\n",p+1);
        break;
      default:
        p--;
    }
  }
  //printf("stripScope(%s)=%s\n",name,name);
  return name;
}
#endif

// new version by Davide Cesari which also works for Fortran
QCString stripScope(const char *name)
{
  QCString result = name;
  int l=result.length();
  int p;
  bool done = FALSE;
  bool skipBracket=FALSE; // if brackets do not match properly, ignore them altogether
  int count=0;

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
          if (p>0 && result.at(p-1)==':') return result.right(l-p-1);
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
                case '>': 
                  count++; 
                  break;
                case '<': 
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
  static const char hex[] = "0123456789ABCDEF";
  static GrowBuf growBuf;
  growBuf.clear();
  if (s==0) return "";
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

/*! Converts a string to an XML-encoded string */
QCString convertToXML(const char *s, bool keepEntities)
{
  static GrowBuf growBuf;
  growBuf.clear();
  if (s==0) return "";
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
  static GrowBuf growBuf;
  growBuf.clear();
  if (s==0) return "";
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
      case '\007':  growBuf.addStr("&#x2407;"); break;
      case  1: case  2: case  3: case  4: case  5: case  6:          case  8:
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
QCString convertToHtml(const char *s,bool keepEntities)
{
  static GrowBuf growBuf;
  growBuf.clear();
  if (s==0) return "";
  growBuf.addStr(getHtmlDirEmbedingChar(getTextDirByConfig(s)));
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
      default:   growBuf.addChar(c);       break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}

QCString convertToJSString(const char *s, bool applyTextDir)
{
  static GrowBuf growBuf;
  growBuf.clear();
  if (s==0) return "";
  if (applyTextDir)
    growBuf.addStr(getJsDirEmbedingChar(getTextDirByConfig(s)));
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
  static GrowBuf growBuf;
  growBuf.clear();
  if (s==0) return "";
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
  filterLatexString(t,s,insideTabbing,FALSE,FALSE,keepSpaces);
  return result.data();
}



QCString convertCharEntitiesToUTF8(const QCString &s)
{
  QCString result;
  static QRegExp entityPat("&[a-zA-Z]+[0-9]*;");

  if (s.length()==0) return result;
  static GrowBuf growBuf;
  growBuf.clear();
  int p,i=0,l;
  while ((p=entityPat.match(s,i,&l))!=-1)
  {
    if (p>i)
    {
      growBuf.addStr(s.mid(i,p-i));
    }
    QCString entity = s.mid(p,l);
    DocSymbol::SymType symType = HtmlEntityMapper::instance()->name2sym(entity);
    const char *code=0;
    if (symType!=DocSymbol::Sym_Unknown && (code=HtmlEntityMapper::instance()->utf8(symType)))
    {
      growBuf.addStr(code);
    }
    else
    {
      growBuf.addStr(s.mid(p,l));
    }
    i=p+l;
  }
  growBuf.addStr(s.mid(i,s.length()-i));
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
    MemberGroupSDict **ppMemberGroupSDict,
    const Definition *context)
{
  ASSERT(context!=0);
  //printf("addMemberToMemberGroup()\n");
  if (ml==0) return;
  MemberListIterator mli(*ml);
  MemberDef *md;
  uint index;
  for (index=0;(md=mli.current());)
  {
    if (md->isEnumerate()) // insert enum value of this enum into groups
    {
      const MemberList *fmdl=md->enumFieldList();
      if (fmdl!=0)
      {
        MemberListIterator fmli(*fmdl);
        MemberDef *fmd;
        for (fmli.toFirst();(fmd=fmli.current());++fmli)
        {
          int groupId=fmd->getMemberGroupId();
          if (groupId!=-1)
          {
            MemberGroupInfo *info = Doxygen::memGrpInfoDict[groupId];
            //QCString *pGrpHeader = Doxygen::memberHeaderDict[groupId];
            //QCString *pDocs      = Doxygen::memberDocDict[groupId];
            if (info)
            {
              if (*ppMemberGroupSDict==0)
              {
                *ppMemberGroupSDict = new MemberGroupSDict;
                (*ppMemberGroupSDict)->setAutoDelete(TRUE);
              }
              MemberGroup *mg = (*ppMemberGroupSDict)->find(groupId);
              if (mg==0)
              {
                mg = new MemberGroup(
                    groupId,
                    info->header,
                    info->doc,
                    info->docFile,
                    info->docLine
                    );
                (*ppMemberGroupSDict)->append(groupId,mg);
              }
              mg->insertMember(fmd); // insert in member group
              fmd->setMemberGroup(mg);
            }
          }
        }
      }
    }
    int groupId=md->getMemberGroupId();
    if (groupId!=-1)
    {
      MemberGroupInfo *info = Doxygen::memGrpInfoDict[groupId];
      //QCString *pGrpHeader = Doxygen::memberHeaderDict[groupId];
      //QCString *pDocs      = Doxygen::memberDocDict[groupId];
      if (info)
      {
        if (*ppMemberGroupSDict==0)
        {
          *ppMemberGroupSDict = new MemberGroupSDict;
          (*ppMemberGroupSDict)->setAutoDelete(TRUE);
        }
        MemberGroup *mg = (*ppMemberGroupSDict)->find(groupId);
        if (mg==0)
        {
          mg = new MemberGroup(
              groupId,
              info->header,
              info->doc,
              info->docFile,
              info->docLine
              );
          (*ppMemberGroupSDict)->append(groupId,mg);
        }
        md = ml->take(index); // remove from member list
        mg->insertMember(md); // insert in member group
        mg->setRefItems(info->m_sli);
        md->setMemberGroup(mg);
        continue;
      }
    }
    ++mli;++index;
  }
}

/*! Extracts a (sub-)string from \a type starting at \a pos that
 *  could form a class. The index of the match is returned and the found
 *  class \a name and a template argument list \a templSpec. If -1 is returned
 *  there are no more matches.
 */
int extractClassNameFromType(const QCString &type,int &pos,QCString &name,QCString &templSpec,SrcLangExt lang)
{
  static const QRegExp re_norm("[a-z_A-Z\\x80-\\xFF][a-z_A-Z0-9:\\x80-\\xFF]*");
  static const QRegExp re_ftn("[a-z_A-Z\\x80-\\xFF][()=_a-z_A-Z0-9:\\x80-\\xFF]*");
  QRegExp re;

  name.resize(0);
  templSpec.resize(0);
  int i,l;
  int typeLen=type.length();
  if (typeLen>0)
  {
    if (lang == SrcLangExt_Fortran)
    {
      if (type.at(pos)==',') return -1;
      if (type.left(4).lower()=="type")
      {
        re = re_norm;
      }
      else
      {
        re = re_ftn;
      }
    }
    else
    {
      re = re_norm;
    }

    if ((i=re.match(type,pos,&l))!=-1) // for each class name in the type
    {
      int ts=i+l;
      int te=ts;
      int tl=0;
      while (type.at(ts)==' ' && ts<typeLen) ts++,tl++; // skip any whitespace
      if (type.at(ts)=='<') // assume template instance
      {
        // locate end of template
        te=ts+1;
        int brCount=1;
        while (te<typeLen && brCount!=0)
        {
          if (type.at(te)=='<') 
          {
            if (te<typeLen-1 && type.at(te+1)=='<') te++; else brCount++;
          }
          if (type.at(te)=='>') 
          {
            if (te<typeLen-1 && type.at(te+1)=='>') te++; else brCount--;
          }
          te++;
        }
      }
      name = type.mid(i,l);
      if (te>ts) 
      {
        templSpec = type.mid(ts,te-ts),tl+=te-ts;
        pos=i+l+tl;
      }
      else // no template part
      {
        pos=i+l;
      }
      //printf("extractClassNameFromType([in] type=%s,[out] pos=%d,[out] name=%s,[out] templ=%s)=TRUE\n",
      //    type.data(),pos,name.data(),templSpec.data());
      return i;
    }
  }
  pos = typeLen;
  //printf("extractClassNameFromType([in] type=%s,[out] pos=%d,[out] name=%s,[out] templ=%s)=FALSE\n",
  //       type.data(),pos,name.data(),templSpec.data());
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

  static QRegExp re("[a-z:_A-Z\\x80-\\xFF][a-z:_A-Z0-9\\x80-\\xFF]*");
  int l,i;
  // for each identifier in the template part (e.g. B<T> -> T)
  while ((i=re.match(name,p,&l))!=-1)
  {
    result += name.mid(p,i-p);
    QCString n = name.mid(i,l);
    bool found=FALSE;
    for (const Argument formArg : formalArgs)
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
      const ClassDef *cd = getResolvedClass(context,0,n);
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
    p=i+l;
  }
  result+=name.right(name.length()-p);
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
    const QCString &name,
    const ArgumentList &formalArgs,
    const ArgumentList &actualArgs)
{
  //printf("substituteTemplateArgumentsInString(name=%s formal=%s actualArg=%s)\n",
  //    name.data(),argListToString(formalArgs).data(),argListToString(actualArgs).data());
  if (formalArgs.empty()) return name;
  QCString result;
  static QRegExp re("[a-z_A-Z\\x80-\\xFF][a-z_A-Z0-9\\x80-\\xFF]*");
  int p=0,l,i;
  // for each identifier in the base class name (e.g. B<T> -> B and T)
  while ((i=re.match(name,p,&l))!=-1)
  {
    result += name.mid(p,i-p);
    QCString n = name.mid(i,l);
    auto formIt = formalArgs.begin();
    auto actIt  = actualArgs.begin();

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
      if (actIt!=actualArgs.end())
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
        //printf(">> formArg->name='%s' actArg->type='%s' actArg->name='%s'\n",
        //    formArg->name.data(),actArg ? actArg->type.data() : "",actArg ? actArg->name.data() : ""
        //    );
        if (formArg.name==n && actIt!=actualArgs.end() && !actArg.type.isEmpty()) // base class is a template argument
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
                 actIt==actualArgs.end() &&
                 !formArg.defval.isEmpty() &&
                 formArg.defval!=name /* to prevent recursion */
            )
        {
          result += substituteTemplateArgumentsInString(formArg.defval,formalArgs,actualArgs)+" ";
          found=TRUE;
        }
      }
      else if (formArg.name==n &&
               actIt==actualArgs.end() &&
               !formArg.defval.isEmpty() &&
               formArg.defval!=name /* to prevent recursion */
              )
      {
        result += substituteTemplateArgumentsInString(formArg.defval,formalArgs,actualArgs)+" ";
        found=TRUE;
      }
      if (actIt!=actualArgs.end())
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
  result+=name.right(name.length()-p);
  //printf("      Inheritance relation %s -> %s\n",
  //    name.data(),result.data());
  return result.stripWhiteSpace();
}

#if 0
/*! Makes a deep copy of the list of argument lists \a srcLists. 
 *  Will allocate memory, that is owned by the caller.
 */
QList<ArgumentList> *copyArgumentLists(const QList<ArgumentList> *srcLists)
{
  ASSERT(srcLists!=0);
  QList<ArgumentList> *dstLists = new QList<ArgumentList>;
  dstLists->setAutoDelete(TRUE);
  QListIterator<ArgumentList> sli(*srcLists);
  ArgumentList *sl;
  for (;(sl=sli.current());++sli)
  {
    dstLists->append(sl->deepCopy());
  }
  return dstLists;
}
#endif

/*! Strips template specifiers from scope \a fullName, except those 
 *  that make up specialized classes. The switch \a parentOnly 
 *  determines whether or not a template "at the end" of a scope 
 *  should be considered, e.g. with \a parentOnly is \c TRUE, A<T>::B<S> will 
 *  try to strip \<T\> and not \<S\>, while \a parentOnly is \c FALSE will 
 *  strip both unless A<T> or B<S> are specialized template classes. 
 */
QCString stripTemplateSpecifiersFromScope(const QCString &fullName,
    bool parentOnly,
    QCString *pLastScopeStripped)
{
  QCString result;
  int p=0;
  int l=fullName.length();
  int i=fullName.find('<');
  while (i!=-1)
  {
    //printf("1:result+=%s\n",fullName.mid(p,i-p).data());
    int e=i+1;
    bool done=FALSE;
    int count=1;
    while (e<l && !done)
    {
      char c=fullName.at(e++);
      if (c=='<') 
      {
        count++;
      }
      else if (c=='>') 
      {
        count--;
        done = count==0;
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
          char c=s.at(sp++);
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
    const char *fileName,int startLine,
    const std::vector<ListItemInfo> &sli,
    GroupDef *gd,
    const TagInfo *tagInfo,
    bool xref,
    SrcLangExt lang
    )
{
  PageDef *pd=0;
  //printf("addRelatedPage(name=%s gd=%p)\n",name,gd);
  if ((pd=Doxygen::pageSDict->find(name)) && !tagInfo)
  {
    if (!xref) warn(fileName,startLine,"multiple use of page label '%s', (other occurrence: %s, line: %d)",
         name,pd->docFile().data(),pd->docLine());
    // append documentation block to the page.
    pd->setDocumentation(doc,fileName,startLine);
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

    QCString title=ptitle.stripWhiteSpace();
    pd=createPageDef(fileName,startLine,baseName,doc,title);

    pd->setRefItems(sli);
    pd->setLanguage(lang);

    if (tagInfo)
    {
      pd->setReference(tagInfo->tagName);
      pd->setFileName(tagInfo->fileName);
    }

    //printf("Appending page '%s'\n",baseName.data());
    Doxygen::pageSDict->append(baseName,pd);

    if (gd) gd->addPage(pd);

    if (pd->hasTitle())
    {
      //outputList->writeTitle(pi->name,pi->title);

      // a page name is a label as well!
      QCString file;
      if (gd)
      {
        file=gd->getOutputFileBase();
      }
      else 
      {
        file=pd->getOutputFileBase();
      }
      SectionInfo *si = Doxygen::sectionDict->find(pd->name());
      if (si)
      {
        if (si->lineNr != -1)
        {
          warn(file,-1,"multiple use of section label '%s', (first occurrence: %s, line %d)",pd->name().data(),si->fileName.data(),si->lineNr);
        }
        else
        {
          warn(file,-1,"multiple use of section label '%s', (first occurrence: %s)",pd->name().data(),si->fileName.data());
        }
      }
      else
      {
        si=new SectionInfo(
            file,-1,pd->name(),pd->title(),SectionInfo::Page,0,pd->getReference());
        //printf("si->label='%s' si->definition=%s si->fileName='%s'\n",
        //      si->label.data(),si->definition?si->definition->name().data():"<none>",
        //      si->fileName.data());
        //printf("  SectionInfo: sec=%p sec->fileName=%s\n",si,si->fileName.data());
        //printf("Adding section key=%s si->fileName=%s\n",pageName.data(),si->fileName.data());
        Doxygen::sectionDict->append(pd->name(),si);
      }
    }
  }
  return pd;
}

//----------------------------------------------------------------------------

void addRefItem(const std::vector<ListItemInfo> &sli,
    const char *key, 
    const char *prefix, const char *name,const char *title,const char *args,Definition *scope)
{
  //printf("addRefItem(sli=%p,key=%s,prefix=%s,name=%s,title=%s,args=%s)\n",sli,key,prefix,name,title,args);
  if (key && key[0]!='@') // check for @ to skip anonymous stuff (see bug427012)
  {
    for (const ListItemInfo &lii : sli)
    {
      RefList *refList = Doxygen::xrefLists->find(lii.type);
      if (refList
          &&
          (
           // either not a built-in list or the list is enabled
           (lii.type!="todo"       || Config_getBool(GENERATE_TODOLIST)) &&
           (lii.type!="test"       || Config_getBool(GENERATE_TESTLIST)) &&
           (lii.type!="bug"        || Config_getBool(GENERATE_BUGLIST))  &&
           (lii.type!="deprecated" || Config_getBool(GENERATE_DEPRECATEDLIST))
          )
         )
      {
        RefItem *item = refList->getRefItem(lii.itemId);
        ASSERT(item!=0);

        item->prefix = prefix;
        item->scope  = scope;
        item->name   = name;
        item->title  = title;
        item->args   = args;

        refList->insertIntoList(key,item);

      }
    }
  }
}

bool recursivelyAddGroupListToTitle(OutputList &ol,const Definition *d,bool root)
{
  GroupList *groups = d->partOfGroups();
  if (groups) // write list of group to which this definition belongs
  {
    if (root)
    {
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      ol.writeString("<div class=\"ingroups\">");
    }
    GroupListIterator gli(*groups);
    GroupDef *gd;
    bool first=true;
    for (gli.toFirst();(gd=gli.current());++gli)
    {
      if (recursivelyAddGroupListToTitle(ol, gd, FALSE))
      {
        ol.writeString(" &raquo; ");
      }
      if (!first) { ol.writeString(" &#124; "); } else first=FALSE;
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
    bool insideTabbing,bool insidePre,bool insideItem,bool keepSpaces)
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
        case '-':  t << "-\\/"; break;
        case '^':  (usedTableLevels()>0) ? t << "\\string^" : t << (char)c;    break;
        case '~':  t << "\\string~";    break;
        case ' ':  if (keepSpaces) t << "~"; else t << ' ';
                   break;
        default:
                   t << (char)c;
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
                       ((c>='A' && c<='Z' && pc!=' ' && pc!='\0' && *p) || (c==':' && pc!=':') || (pc=='.' && isId(c)))
                      )
                   {
                     t << "\\+";
                   }
                   t << (char)c;
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
        filterLatexString(t,tmp,TRUE);
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
        filterLatexString(t,tmp.data(),TRUE);
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
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '#':  t << "\\#"; break;
      case '%':  t << "\\%"; break;
      case '\\':  t << "\\\\"; break;
      default:
        t << c;
        break;
    }
  }
  return result.data();
}


QCString rtfFormatBmkStr(const char *name)
{
  static QCString g_nextTag( "AAAAAAAAAA" );
  static QDict<QCString> g_tagDict( 5003 );

  g_tagDict.setAutoDelete(TRUE);

  // To overcome the 40-character tag limitation, we
  // substitute a short arbitrary string for the name
  // supplied, and keep track of the correspondence
  // between names and strings.
  QCString key( name );
  QCString* tag = g_tagDict.find( key );
  if ( !tag )
  {
    // This particular name has not yet been added
    // to the list. Add it, associating it with the
    // next tag value, and increment the next tag.
    tag = new QCString( g_nextTag.copy() ); // Make sure to use a deep copy!
    g_tagDict.insert( key, tag );

    // This is the increment part
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

  return *tag;
}

bool checkExtension(const char *fName, const char *ext)
{
  return (QCString(fName).right(QCString(ext).length())==ext);
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
    QCString *s = Doxygen::namespaceAliasDict[ns];
    if (s)
    {
      scope=*s+scope.right(scope.length()-i);
      i=s->length();
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
bool containsWord(const QCString &s,const QCString &word)
{
  static QRegExp wordExp("[a-z_A-Z\\x80-\\xFF]+");
  int p=0,i,l;
  while ((i=wordExp.match(s,p,&l))!=-1)
  {
    if (s.mid(i,l)==word) return TRUE;
    p=i+l;
  }
  return FALSE;
}

bool findAndRemoveWord(QCString &s,const QCString &word)
{
  static QRegExp wordExp("[a-z_A-Z\\x80-\\xFF]+");
  int p=0,i,l;
  while ((i=wordExp.match(s,p,&l))!=-1)
  {
    if (s.mid(i,l)==word) 
    {
      if (i>0 && isspace((uchar)s.at(i-1))) 
        i--,l++;
      else if (i+l<(int)s.length() && isspace((uchar)s.at(i+l)))
        l++;
      s = s.left(i)+s.mid(i+l); // remove word + spacing
      return TRUE;
    }
    p=i+l;
  }
  return FALSE;
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
  while ((c=*p++))
  {
    if (c==' ' || c=='\t' || c=='\r') i++;
    else if (c=='\n') i++,li=i,docLine++;
    else break;
  }

  // search for trailing empty lines
  int b=l-1,bi=-1;
  p=s.data()+b;
  while (b>=0)
  {
    c=*p; p--;
    if (c==' ' || c=='\t' || c=='\r') b--;
    else if (c=='\n') bi=b,b--;
    else break;
  }

  // return whole string if no leading or trailing lines where found
  if (li==-1 && bi==-1) return s;

  // return substring
  if (bi==-1) bi=l;
  if (li==-1) li=0;
  if (bi<=li) return 0; // only empty lines
  return s.mid(li,bi-li);
}

#if 0
void stringToSearchIndex(const QCString &docBaseUrl,const QCString &title,
    const QCString &str,bool priority,const QCString &anchor)
{
  static bool searchEngine = Config_getBool(SEARCHENGINE);
  if (searchEngine)
  {
    Doxygen::searchIndex->setCurrentDoc(title,docBaseUrl,anchor);
    static QRegExp wordPattern("[a-z_A-Z\\x80-\\xFF][a-z_A-Z0-9\\x80-\\xFF]*");
    int i,p=0,l;
    while ((i=wordPattern.match(str,p,&l))!=-1)
    {
      Doxygen::searchIndex->addWord(str.mid(i,l),priority);
      p=i+l;
    }
  }
}
#endif

//--------------------------------------------------------------------------

static QDict<int> g_extLookup;

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
  { "tcl",         "tcl",           SrcLangExt_Tcl      },
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
  if (g_extLookup.find(extension)!=0) // language was already register for this ext
  {
    g_extLookup.remove(extension);
  }
  //printf("registering extension %s\n",extName.data());
  g_extLookup.insert(extName,new int(parserId));
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
  g_extLookup.setAutoDelete(TRUE);
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
  updateLanguageMapping(".vhd",      "vhdl");
  updateLanguageMapping(".vhdl",     "vhdl");
  updateLanguageMapping(".tcl",      "tcl");
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
    int *pVal=g_extLookup.find(extName.data());
    if (pVal) // listed extension
    {
      //printf("getLanguageFromFileName(%s)=%x\n",fi.extension().data(),*pVal);
      return (SrcLangExt)*pVal;
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

MemberDef *getMemberFromSymbol(const Definition *scope,const FileDef *fileScope, 
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

  DefinitionIntf *di = Doxygen::symbolMap->find(name);
  if (di==0)
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

  if (di->definitionType()==DefinitionIntf::TypeSymbolList)
  {
    //printf("multiple matches!\n");
    // find the closest closest matching definition
    DefinitionListIterator dli(*(DefinitionList*)di);
    Definition *d;
    for (dli.toFirst();(d=dli.current());++dli)
    {
      if (d->definitionType()==Definition::TypeMember)
      {
        g_visitedNamespaces.clear();
        int distance = isAccessibleFromWithExpScope(scope,fileScope,d,explicitScopePart);
        if (distance!=-1 && distance<minDistance)
        {
          minDistance = distance;
          bestMatch = dynamic_cast<MemberDef *>(d);
          //printf("new best match %s distance=%d\n",bestMatch->qualifiedName().data(),distance);
        }
      }
    }
  }
  else if (di->definitionType()==Definition::TypeMember)
  {
    //printf("unique match!\n");
    Definition *d = (Definition *)di;
    g_visitedNamespaces.clear();
    int distance = isAccessibleFromWithExpScope(scope,fileScope,d,explicitScopePart);
    if (distance!=-1 && distance<minDistance)
    {
      minDistance = distance;
      bestMatch = dynamic_cast<MemberDef *>(d);
      //printf("new best match %s distance=%d\n",bestMatch->qualifiedName().data(),distance);
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
  char c=*s++;
  t << c;
  if (c<0) // multibyte character
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

int nextUtf8CharPosition(const QCString &utf8Str,int len,int startPos)
{
  int bytes=1;
  if (startPos>=len) return len;
  char c = utf8Str[startPos];
  if (c<0) // multibyte utf-8 character
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
    static QRegExp re1("&#[0-9]+;");     // numerical entity
    static QRegExp re2("&[A-Z_a-z]+;");  // named entity
    int l1,l2;
    int i1 = re1.match(utf8Str,startPos,&l1);
    int i2 = re2.match(utf8Str,startPos,&l2);
    if (i1!=-1)
    {
      bytes=l1;
    }
    else if (i2!=-1)
    {
      bytes=l2;
    }
  }
  return startPos+bytes;
}

QCString parseCommentAsText(const Definition *scope,const MemberDef *md,
    const QCString &doc,const QCString &fileName,int lineNr)
{
  QGString s;
  if (doc.isEmpty()) return s.data();
  FTextStream t(&s);
  DocNode *root = validatingParseDoc(fileName,lineNr,
      (Definition*)scope,(MemberDef*)md,doc,FALSE,FALSE);
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

static QDict<void> aliasesProcessed;

static QCString expandAliasRec(const QCString s,bool allowRecursion=FALSE);

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
    i+=p-s;
  }
  return i;
}

/** Replaces the markers in an alias definition \a aliasValue 
 *  with the corresponding values found in the comma separated argument 
 *  list \a argList and the returns the result after recursive alias expansion.
 */
static QCString replaceAliasArguments(const QCString &aliasValue,const QCString &argList)
{
  //printf("----- replaceAliasArguments(val=[%s],args=[%s])\n",aliasValue.data(),argList.data());

  // first make a list of arguments from the comma separated argument list
  QList<QCString> args;
  args.setAutoDelete(TRUE);
  int i,l=(int)argList.length();
  int s=0;
  for (i=0;i<l;i++)
  {
    char c = argList.at(i);
    if (c==',' && (i==0 || argList.at(i-1)!='\\')) 
    {
      args.append(new QCString(argList.mid(s,i-s)));
      s=i+1; // start of next argument
    }
    else if (c=='@' || c=='\\')
    {
      // check if this is the start of another aliased command (see bug704172)
      i+=findEndOfCommand(argList.data()+i+1);
    }
  }
  if (l>s) args.append(new QCString(argList.right(l-s)));
  //printf("found %d arguments\n",args.count());

  // next we look for the positions of the markers and add them to a list
  QList<Marker> markerList;
  markerList.setAutoDelete(TRUE);
  l = aliasValue.length();
  int markerStart=0;
  int markerEnd=0;
  for (i=0;i<l;i++)
  {
    if (markerStart==0 && aliasValue.at(i)=='\\') // start of a \xx marker
    {
      markerStart=i+1;
    }
    else if (markerStart>0 && aliasValue.at(i)>='0' && aliasValue.at(i)<='9')
    {
      // read digit that make up the marker number
      markerEnd=i+1;
    }
    else
    {
      if (markerStart>0 && markerEnd>markerStart) // end of marker
      {
        int markerLen = markerEnd-markerStart;
        markerList.append(new Marker(markerStart-1, // include backslash
                    atoi(aliasValue.mid(markerStart,markerLen)),markerLen+1));
        //printf("found marker at %d with len %d and number %d\n",
        //    markerStart-1,markerLen+1,atoi(aliasValue.mid(markerStart,markerLen)));
      }
      markerStart=0; // outside marker
      markerEnd=0;
    }
  }
  if (markerStart>0)
  {
    markerEnd=l;
  }
  if (markerStart>0 && markerEnd>markerStart)
  {
     int markerLen = markerEnd-markerStart;
     markerList.append(new Marker(markerStart-1, // include backslash
                 atoi(aliasValue.mid(markerStart,markerLen)),markerLen+1));
     //printf("found marker at %d with len %d and number %d\n",
     //    markerStart-1,markerLen+1,atoi(aliasValue.mid(markerStart,markerLen)));
  }

  // then we replace the markers with the corresponding arguments in one pass
  QCString result;
  int p=0;
  for (i=0;i<(int)markerList.count();i++)
  {
    Marker *m = markerList.at(i);
    result+=aliasValue.mid(p,m->pos-p);
    //printf("part before marker %d: '%s'\n",i,aliasValue.mid(p,m->pos-p).data());
    if (m->number>0 && m->number<=(int)args.count()) // valid number
    {
      result+=expandAliasRec(*args.at(m->number-1),TRUE);
      //printf("marker index=%d pos=%d number=%d size=%d replacement %s\n",i,m->pos,m->number,m->size,
      //    args.at(m->number-1)->data());
    }
    p=m->pos+m->size; // continue after the marker
  }
  result+=aliasValue.right(l-p); // append remainder
  //printf("string after replacement of markers: '%s'\n",result.data());

  // expand the result again
  result = substitute(result,"\\{","{");
  result = substitute(result,"\\}","}");
  result = expandAliasRec(substitute(result,"\\,",","));

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

static QCString expandAliasRec(const QCString s,bool allowRecursion)
{
  QCString result;
  static QRegExp cmdPat("[\\\\@][a-z_A-Z][a-z_A-Z0-9]*");
  QCString value=s;
  int i,p=0,l;
  while ((i=cmdPat.match(value,p,&l))!=-1)
  {
    result+=value.mid(p,i-p);
    QCString args = extractAliasArgs(value,i+l);
    bool hasArgs = !args.isEmpty();            // found directly after command
    int argsLen = args.length();
    QCString cmd = value.mid(i+1,l-1);
    QCString cmdNoArgs = cmd;
    int numArgs=0;
    if (hasArgs)
    {
      numArgs = countAliasArguments(args);
      cmd += QCString().sprintf("{%d}",numArgs);  // alias name + {n}
    }
    QCString *aliasText=Doxygen::aliasDict.find(cmd);
    if (numArgs>1 && aliasText==0) 
    { // in case there is no command with numArgs parameters, but there is a command with 1 parameter, 
      // we also accept all text as the argument of that command (so you don't have to escape commas)
      aliasText=Doxygen::aliasDict.find(cmdNoArgs+"{1}");
      if (aliasText)
      {
        cmd = cmdNoArgs+"{1}";
        args = escapeCommas(args); // escape , so that everything is seen as one argument
      }
    }
    //printf("Found command s='%s' cmd='%s' numArgs=%d args='%s' aliasText=%s\n",
    //    s.data(),cmd.data(),numArgs,args.data(),aliasText?aliasText->data():"<none>");
    if ((allowRecursion || aliasesProcessed.find(cmd)==0) && aliasText) // expand the alias
    {
      //printf("is an alias!\n");
      if (!allowRecursion) aliasesProcessed.insert(cmd,(void *)0x8);
      QCString val = *aliasText;
      if (hasArgs)
      {
        val = replaceAliasArguments(val,args);
        //printf("replace '%s'->'%s' args='%s'\n",
        //       aliasText->data(),val.data(),args.data());
      }
      result+=expandAliasRec(val);
      if (!allowRecursion) aliasesProcessed.remove(cmd);
      p=i+l;
      if (hasArgs) p+=argsLen+2;
    }
    else // command is not an alias
    {
      //printf("not an alias!\n");
      result+=value.mid(i,l);
      p=i+l;
    }
  }
  result+=value.right(value.length()-p);

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
  aliasesProcessed.clear();
  //printf("Expanding: '%s'\n",aliasCmd.data());
  result = expandAliasRec(aliasCmd);
  //printf("Expanding result: '%s'->'%s'\n",aliasCmd.data(),result.data());
  return result;
}

QCString expandAlias(const QCString &aliasName,const QCString &aliasValue)
{
  QCString result;
  aliasesProcessed.clear();
  // avoid expanding this command recursively
  aliasesProcessed.insert(aliasName,(void *)0x8);
  // expand embedded commands
  //printf("Expanding: '%s'->'%s'\n",aliasName.data(),aliasValue.data());
  result = expandAliasRec(aliasValue);
  //printf("Expanding result: '%s'->'%s'\n",aliasName.data(),result.data());
  return result;
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
    ol.generateDoc(d->docFile(),d->docLine(),d,0,a.docs,TRUE,FALSE);
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
  if (FILE *fp = popen(cmd, "r"))
  {
    char resBuf[512];
    while (size_t len = fread(resBuf, 1, sizeof(resBuf), fp))
    {
      fwrite(resBuf, 1, len, stderr);
    }
    pclose(fp);
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
    err("unsupported character conversion: '%s'->'%s': %s\n"
        "Check the INPUT_ENCODING setting in the config file!\n",
        inputEncoding,outputEncoding,strerror(errno));
    exit(1);
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
    err("%s: failed to translate characters from %s to %s: check INPUT_ENCODING\n",
        fileName,inputEncoding,outputEncoding);
    exit(1);
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
    FILE *f=portable_popen(cmd,"r");
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
    portable_pclose(f);
    inBuf.at(inBuf.curPos()) ='\0';
    Debug::print(Debug::FilterOutput, 0, "Filter output\n");
    Debug::print(Debug::FilterOutput,0,"-------------\n%s\n-------------\n",qPrint(inBuf));
  }

  int start=0;
  if (size>=2 &&
      (((uchar)inBuf.at(0)==0xFF && (uchar)inBuf.at(1)==0xFE) || // Little endian BOM
       ((uchar)inBuf.at(0)==0xFE && (uchar)inBuf.at(1)==0xFF)    // big endian BOM
      )
     ) // UCS-2 encoded file
  {
    transcodeCharacterBuffer(fileName,inBuf,inBuf.curPos(),
        "UCS-2","UTF-8");
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
QCString filterTitle(const QCString &title)
{
  QCString tf;
  static QRegExp re("%[A-Z_a-z]");
  int p=0,i,l;
  while ((i=re.match(title,p,&l))!=-1)
  {
    tf+=title.mid(p,i-p);
    tf+=title.mid(i+1,l-1); // skip %
    p=i+l;
  }
  tf+=title.right(title.length()-p);
  return tf;
}

//----------------------------------------------------------------------------
// returns TRUE if the name of the file represented by 'fi' matches
// one of the file patterns in the 'patList' list.

bool patternMatch(const QFileInfo &fi,const QStrList *patList)
{
  static bool caseSenseNames = Config_getBool(CASE_SENSE_NAMES);
  bool found = FALSE;

  // For Windows/Mac, always do the case insensitive match
#if defined(_WIN32) || defined(__MACOSX__) || defined(__CYGWIN__)
  caseSenseNames = FALSE;
#endif

  if (patList)
  {
    QStrListIterator it(*patList);
    QCString pattern;

    QCString fn = fi.fileName().data();
    QCString fp = fi.filePath().data();
    QCString afp= fi.absFilePath().data();

    for (it.toFirst();(pattern=it.current());++it)
    {
      if (!pattern.isEmpty())
      {
        int i=pattern.find('=');
        if (i!=-1) pattern=pattern.left(i); // strip of the extension specific filter name

        QRegExp re(pattern,caseSenseNames,TRUE);

        found = re.match(fn)!=-1 ||
                re.match(fp)!=-1 ||
                re.match(afp)!=-1;
        if (found) break;
        //printf("Matching '%s' against pattern '%s' found=%d\n",
        //    fi->fileName().data(),pattern.data(),found);
      }
    }
  }
  return found;
}

#if 0 // move to HtmlGenerator::writeSummaryLink
void writeSummaryLink(OutputList &ol,const char *label,const char *title,
                      bool &first,const char *file)
{
  if (first)
  {
    ol.writeString("  <div class=\"summary\">\n");
    first=FALSE;
  }
  else
  {
    ol.writeString(" &#124;\n");
  }
  if (file)
  {
    ol.writeString("<a href=\"");
    ol.writeString(file);
    ol.writeString(Doxygen::htmlFileExtension);
  }
  else
  {
    ol.writeString("<a href=\"#");
    ol.writeString(label);
  }
  ol.writeString("\">");
  ol.writeString(title);
  ol.writeString("</a>");
}
#endif

QCString externalLinkTarget()
{
  static bool extLinksInWindow = Config_getBool(EXT_LINKS_IN_WINDOW);
  if (extLinksInWindow) return "target=\"_blank\" "; else return "";
}

QCString externalRef(const QCString &relPath,const QCString &ref,bool href)
{
  QCString result;
  if (!ref.isEmpty())
  {
    QCString *dest = Doxygen::tagDestinationDict[ref];
    if (dest)
    {
      result = *dest;
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
  QCString result;
  QCString s=str;
  if (s.isEmpty()) return result;
  static QRegExp re("##[0-9A-Fa-f][0-9A-Fa-f]");
  static const char hex[] = "0123456789ABCDEF";
  static int hue   = Config_getInt(HTML_COLORSTYLE_HUE);
  static int sat   = Config_getInt(HTML_COLORSTYLE_SAT);
  static int gamma = Config_getInt(HTML_COLORSTYLE_GAMMA);
  int i,l,sl=s.length(),p=0;
  while ((i=re.match(s,p,&l))!=-1)
  {
    result+=s.mid(p,i-p);
    QCString lumStr = s.mid(i+2,l-2);
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
  result+=s.right(sl-p);
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
    case SrcLangExt_JS:       return "Javascript";
    case SrcLangExt_Python:   return "Python";
    case SrcLangExt_Fortran:  return "Fortran";
    case SrcLangExt_VHDL:     return "VHDL";
    case SrcLangExt_XML:      return "XML";
    case SrcLangExt_SQL:      return "SQL";
    case SrcLangExt_Tcl:      return "Tcl";
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
QCString replaceScopeSeparator(QCString str)
{
  // we don't know about the language so we have to go for the worse
  return substitute(substitute(str,"\\","::"),".","::");  // PHP and Java, CSharp, VHDL, Python
}
/** Checks whether the given url starts with a supported protocol */
bool isURL(const QCString &url)
{
  QCString loc_url = url.stripWhiteSpace();
  return loc_url.left(5)=="http:" || loc_url.left(6)=="https:" || 
         loc_url.left(4)=="ftp:"  || loc_url.left(5)=="file:";
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

void addDocCrossReference(MemberDef *src,MemberDef *dst)
{
  //printf("--> addDocCrossReference src=%s,dst=%s\n",src->name().data(),dst->name().data());
  if (dst->isTypedef() || dst->isEnumerate()) return; // don't add types
  if ((dst->hasReferencedByRelation() || dst->hasCallerGraph()) && 
      src->showInCallGraph()
     )
  {
    dst->addSourceReferencedBy(src);
    MemberDef *mdDef = dst->memberDefinition();
    if (mdDef)
    {
      mdDef->addSourceReferencedBy(src);
    }
    MemberDef *mdDecl = dst->memberDeclaration();
    if (mdDecl)
    {
      mdDecl->addSourceReferencedBy(src);
    }
  }
  if ((src->hasReferencesRelation() || src->hasCallGraph()) && 
      src->showInCallGraph()
     )
  {
    src->addSourceReferences(dst);
    MemberDef *mdDef = src->memberDefinition();
    if (mdDef)
    {
      mdDef->addSourceReferences(dst);
    }
    MemberDef *mdDecl = src->memberDeclaration();
    if (mdDecl)
    {
      mdDecl->addSourceReferences(dst);
    }
  }
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

bool namespaceHasVisibleChild(const NamespaceDef *nd,bool includeClasses,bool filterClasses,ClassDef::CompoundType ct)
{
  if (nd->getNamespaceSDict())
  {
    NamespaceSDict::Iterator cnli(*nd->getNamespaceSDict());
    const NamespaceDef *cnd;
    for (cnli.toFirst();(cnd=cnli.current());++cnli)
    {
      if (cnd->isLinkableInProject() && !cnd->isAnonymous())
      {
        return TRUE;
      }
      else if (namespaceHasVisibleChild(cnd,includeClasses,filterClasses,ct))
      {
        return TRUE;
      }
    }
  }
  if (includeClasses)
  {
    const ClassSDict *d = nd->getClassSDict();
    if (filterClasses)
    {
      if (ct == ClassDef::Interface)
      {
        d = nd->getInterfaceSDict();
      }
      else if (ct == ClassDef::Struct)
      {
        d = nd->getStructSDict();
      }
      else if (ct == ClassDef::Exception)
      {
        d = nd->getExceptionSDict();
      }
    }

    if (d)
    {
      ClassSDict::Iterator cli(*d);
      const ClassDef *cd;
      for (;(cd=cli.current());++cli)
      {
        if (cd->isLinkableInProject() && cd->templateMaster()==0)
        {
          return TRUE;
        }
      }
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

QCString extractDirection(QCString &docs)
{
  QRegExp re("\\[[^\\]]+\\]"); // [...]
  int l=0;
  if (re.match(docs,0,&l)==0)
  {
    int  inPos  = docs.find("in", 1,FALSE);
    int outPos  = docs.find("out",1,FALSE);
    bool input  =  inPos!=-1 &&  inPos<l;
    bool output = outPos!=-1 && outPos<l;
    if (input || output) // in,out attributes
    {
      docs = docs.mid(l); // strip attributes
      if (input && output) return "[in,out]";
      else if (input)      return "[in]";
      else if (output)     return "[out]";
    }
  }
  return QCString();
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

QCString getDotImageExtension(void)
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
  QStrList &extraPackages = Config_getList(EXTRA_PACKAGES);
  if (!extraPackages.isEmpty()) 
  {
    t << "% Packages requested by user\n";
    const char *pkgName=extraPackages.first();
    while (pkgName)
    {
      if ((pkgName[0] == '[') || (pkgName[0] == '{'))
        t << "\\usepackage" << pkgName << "\n";
      else
        t << "\\usepackage{" << pkgName << "}\n";
      pkgName=extraPackages.next();
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

static int g_usedTableLevels = 0;

void incUsedTableLevels()
{
  g_usedTableLevels++;
}
void decUsedTableLevels()
{
  g_usedTableLevels--;
}
int usedTableLevels()
{
  return g_usedTableLevels;
}

//------------------------------------------------------

void mergeGraphSettings(QCString nam,graphSettings &dst,graphSettings gs,const char *cmdShow,const char *cmdHide)
{
  if (!dst.isExplicit)
  {
    dst=gs;
  }
  else if (gs.isExplicit)
  {
    // both are explicit
    if (dst.hasGraph != gs.hasGraph)
    {
      warn_uncond("'\\%s' and '\\%s' given for file '%s', using '\\%s'.\n",
                  cmdShow,cmdHide,nam.data(),cmdShow);
      if (gs.hasGraph) dst = gs;
    }
    else if (gs.hasGraph)
    {
      // both have hasGraph
      if (dst.maxDepth != gs.maxDepth)
      {
        warn_uncond("Multile definitions of maximal depth with '\\%s' for file '%s', using maximum.\n",
                    cmdShow,nam.data());
        dst.maxDepth = QMAX(dst.maxDepth,gs.maxDepth);
      }
      if (dst.maxNodes != gs.maxNodes)
      {
        warn_uncond("Multile definitions of maximal number of nodes with '\\%s' for file '%s', using maximum.\n",
                    cmdShow,nam.data());
        dst.maxNodes = QMAX(dst.maxNodes,gs.maxNodes);
      }
    }
  }
}

