/*****************************************************************************
 *
 * 
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

#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include "md5.h"

#include "qtbc.h"
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
#include "portable.h"
#include "parserintf.h"
#include "bufstr.h"
#include "image.h"
#include "growbuf.h"
#include "entry.h"
#include "arguments.h"

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
  //printf("removeAnonymousScopes(`%s')=`%s'\n",s.data(),result.data());
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
  //printf("replaceAnonymousScopes(`%s')=`%s'\n",s.data(),result.data());
  return result;
}


// strip anonymous left hand side part of the scope
QCString stripAnonymousNamespaceScope(const QCString &s)
{
  int i,p=0,l;
  QCString newScope;
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
    else
    {
      if (!newScope.isEmpty()) newScope+="::";
      newScope+=s.right(s.length()-i);
      goto done;
    }
    p=i+l;
  }
done:
  //printf("stripAnonymousNamespaceScope(`%s')=`%s'\n",s.data(),newScope.data());
  return newScope;
}

void writePageRef(OutputDocInterface &od,const char *cn,const char *mn)
{
  od.pushGeneratorState();
  
  od.disable(OutputGenerator::Html);
  od.disable(OutputGenerator::Man);
  if (Config_getBool("PDF_HYPERLINKS")) od.disable(OutputGenerator::Latex);
  if (Config_getBool("RTF_HYPERLINKS")) od.disable(OutputGenerator::RTF);
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
  QCString result;
  result.sprintf("@%d",id);
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
        stricmp(path.left(prefix.length()),prefix)==0) // case insensitive compare
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
 *  one of the paths in the Config_getList("STRIP_FROM_PATH") list
 */
QCString stripFromPath(const QCString &path)
{
  return stripFromPath(path,Config_getList("STRIP_FROM_PATH"));
}

/*! strip part of \a path if it matches
 *  one of the paths in the Config_getList("INCLUDE_PATH") list
 */
QCString stripFromIncludePath(const QCString &path)
{
  return stripFromPath(path,Config_getList("STRIP_FROM_INC_PATH"));
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
      n.right(2)==".M"    ||
      n.right(3)==".mm"   ||
      n.right(3)==".ii"   || // inline
      n.right(4)==".ixx"  ||
      n.right(4)==".ipp"  ||
      n.right(4)==".i++"  ||
      n.right(4)==".inl"  ||
      n.right(4)==".xml" 
     ) return Entry::SOURCE_SEC;
  if (n.right(2)==".h"   || // header
      n.right(3)==".hh"  ||
      n.right(4)==".hxx" ||
      n.right(4)==".hpp" ||
      n.right(4)==".h++" ||
      n.right(4)==".idl" ||
      n.right(4)==".ddl" ||
      n.right(5)==".pidl"
     ) return Entry::HEADER_SEC;
  return 0;
}

QCString resolveTypeDef(Definition *context,const QCString &qualifiedName,
                        Definition **typedefContext)
{
  //printf("<<resolveTypeDef(%s,%s)\n",
  //          context ? context->name().data() : "<none>",qualifiedName.data());
  QCString result;
  if (qualifiedName.isEmpty()) 
  {
    //printf("  qualified name empty!\n");
    return result;
  }

  Definition *mContext=context;
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
    Definition *resScope=mContext;
    if (scopeIndex!=-1) 
    {
      // split-off scope part
      QCString resScopeName = qualifiedName.left(scopeIndex);
      //printf("resScopeName=`%s'\n",resScopeName.data());

      // look-up scope in context
      int is,ps=0;
      int l;
      while ((is=getScopeFragment(resScopeName,ps,&l))!=-1)
      {
        QCString qualScopePart = resScopeName.mid(is,l);
        QCString tmp = resolveTypeDef(mContext,qualScopePart);
        if (!tmp.isEmpty()) qualScopePart=tmp;
        resScope = resScope->findInnerCompound(qualScopePart);
        //printf("qualScopePart=`%s' resScope=%p\n",qualScopePart.data(),resScope);
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
    //printf(">>resolveTypeDef: Found typedef name `%s' in scope `%s' value=`%s' args='%s'\n",
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
    //printf(">>resolveTypeDef: Typedef `%s' not found in scope `%s'!\n",
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
      err("warning: possible recursive namespace alias detected for %s!\n",name);
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
static ClassDef *getResolvedClassRec(Definition *scope,
                              FileDef *fileScope,
                              const char *n,
                              MemberDef **pTypeDef,
                              QCString *pTemplSpec,
                              QCString *pResolvedType
                             );
int isAccessibleFromWithExpScope(Definition *scope,FileDef *fileScope,Definition *item,
                     const QCString &explicitScopePart);

/*! Returns the class representing the value of the typedef represented by \a md
 *  within file \a fileScope.
 *
 *  Example: typedef A T; will return the class representing A if it is a class.
 * 
 *  Example: typedef int T; will return 0, since "int" is not a class.
 */
ClassDef *newResolveTypedef(FileDef *fileScope,MemberDef *md,
                            MemberDef **pMemType,QCString *pTemplSpec,
                            QCString *pResolvedType,
                            ArgumentList *actTemplParams)
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
  
  ClassDef *typeClass = md->getClassDef();
  QCString type = md->typeString(); // get the "value" of the typedef
  if (typeClass && typeClass->isTemplate() && 
      actTemplParams && actTemplParams->count()>0)
  {
    type = substituteTemplateArgumentsInString(type,
            typeClass->templateArguments(),actTemplParams);
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
  MemberDef *memTypeDef = 0;
  ClassDef  *result = getResolvedClassRec(md->getOuterScope(),
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
    md->cacheTypedefVal(result,
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
static QCString substTypedef(Definition *scope,FileDef *fileScope,const QCString &name,
            MemberDef **pTypeDef=0)
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
        MemberDef *md = (MemberDef *)d;
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
    MemberDef *md = (MemberDef *)di;
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

static Definition *endOfPathIsUsedClass(SDict<Definition> *cl,const QCString &localName)
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
static Definition *followPath(Definition *start,FileDef *fileScope,const QCString &path)
{
  int is,ps;
  int l;
  Definition *current=start;
  ps=0;
  //printf("followPath: start='%s' path='%s'\n",start?start->name().data():"<none>",path.data());
  // for each part of the explicit scope
  while ((is=getScopeFragment(path,ps,&l))!=-1)
  {
    // try to resolve the part if it is a typedef
    MemberDef *typeDef=0;
    QCString qualScopePart = substTypedef(current,fileScope,path.mid(is,l),&typeDef);
    //printf("      qualScopePart=%s\n",qualScopePart.data());
    if (typeDef)
    {
      ClassDef *type = newResolveTypedef(fileScope,typeDef);
      if (type)
      {
        //printf("Found type %s\n",type->name().data());
        return type;
      }
    }
    Definition *next = current->findInnerCompound(qualScopePart);
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
            ((NamespaceDef *)current)->getUsedClasses(),qualScopePart);
      }
      else if (current->definitionType()==Definition::TypeFile)
      {
        next = endOfPathIsUsedClass(
            ((FileDef *)current)->getUsedClasses(),qualScopePart);
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
                             FileDef *fileScope,
                             Definition *item,
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
      Definition *sc = explicitScopePartEmpty ? ucd : followPath(ucd,fileScope,explicitScopePart);
      if (sc && sc==item) return TRUE; 
      //printf("Try via used class done\n");
    }
  }
  return FALSE;
}

bool accessibleViaUsingNamespace(const NamespaceSDict *nl,
                                 FileDef *fileScope,
                                 Definition *item,
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
      Definition *sc = explicitScopePart.isEmpty() ? und : followPath(und,fileScope,explicitScopePart);
      if (sc && item->getOuterScope()==sc) 
      {
        //printf("] found it\n");
        return TRUE; 
      }
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
    void push(Definition *scope,FileDef *fileScope,Definition *item)
    {
      if (m_index<MAX_STACK_SIZE)
      {
        m_elements[m_index].scope     = scope;
        m_elements[m_index].fileScope = fileScope;
        m_elements[m_index].item      = item;
        m_index++;
      }
    }
    void push(Definition *scope,FileDef *fileScope,Definition *item,const QCString &expScope)
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
    bool find(Definition *scope,FileDef *fileScope, Definition *item)
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
    bool find(Definition *scope,FileDef *fileScope, Definition *item,const QCString &expScope)
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
      Definition *scope;
      FileDef *fileScope;
      Definition *item;
      QCString expScope;
    };
    int m_index;
    AccessElem m_elements[MAX_STACK_SIZE];
};

/* Returns the "distance" (=number of levels up) from item to scope, or -1
 * if item in not inside scope. 
 */
int isAccessibleFrom(Definition *scope,FileDef *fileScope,Definition *item)
{
  //printf("<isAccesibleFrom(scope=%s,item=%s itemScope=%s)\n",
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

  if ( 
      itemScope==scope ||                                                  // same thing
      (item->definitionType()==Definition::TypeMember &&                   // a member
       itemScope && itemScope->definitionType()==Definition::TypeClass  && // of a class
       scope->definitionType()==Definition::TypeClass &&                   // accessible
       ((ClassDef*)scope)->isAccessibleMember((MemberDef *)item)           // from scope
      ) ||
      (item->definitionType()==Definition::TypeClass &&                    // a nested class
       itemScope && itemScope->definitionType()==Definition::TypeClass &&  // inside a base 
       scope->definitionType()==Definition::TypeClass &&                   // class of scope
       ((ClassDef*)scope)->isBaseClass((ClassDef*)itemScope,TRUE)          
      )
     ) 
  {
    //printf("> found it\n");
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
      NamespaceDef *nscope = (NamespaceDef*)scope;
      //printf("  %s is namespace with %d used classes\n",nscope->name().data(),nscope->getUsedClasses());
      SDict<Definition> *cl = nscope->getUsedClasses();
      if (accessibleViaUsingClass(cl,fileScope,item)) 
      {
        //printf("> found via used class\n");
        goto done;
      }
      NamespaceSDict *nl = nscope->getUsedNamespaces();
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
int isAccessibleFromWithExpScope(Definition *scope,FileDef *fileScope,
                     Definition *item,const QCString &explicitScopePart)
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
  Definition *newScope = followPath(scope,fileScope,explicitScopePart);
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
             ((ClassDef*)newScope)->isBaseClass((ClassDef*)itemScope,TRUE,0)
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
        NamespaceDef *nscope = (NamespaceDef*)newScope;
        SDict<Definition> *cl = nscope->getUsedClasses();
        if (cl)
        {
          SDict<Definition>::Iterator cli(*cl);
          Definition *cd;
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
        NamespaceSDict *nl = nscope->getUsedNamespaces();
        if (nl)
        {
          NamespaceSDict::Iterator nli(*nl);
          NamespaceDef *nd;
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
      NamespaceDef *nscope = (NamespaceDef*)scope;
      NamespaceSDict *nl = nscope->getUsedNamespaces();
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
        NamespaceSDict *nl = fileScope->getUsedNamespaces();
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

static void getResolvedSymbol(Definition *scope,
                       FileDef *fileScope,
                       Definition *d, 
                       const QCString &explicitScopePart,
                       ArgumentList *actTemplParams,
                       int &minDistance,
                       ClassDef *&bestMatch,
                       MemberDef *&bestTypedef,
                       QCString &bestTemplSpec,
                       QCString &bestResolvedType
                      )
{
  //printf("  => found type %x name=%s d=%p\n",
  //       d->definitionType(),d->name().data(),d);

  // only look at classes and members that are enums or typedefs
  if (d->definitionType()==Definition::TypeClass ||
      (d->definitionType()==Definition::TypeMember && 
       (((MemberDef*)d)->isTypedef() || ((MemberDef*)d)->isEnumerate()) 
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
        ClassDef *cd = (ClassDef *)d;
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
        MemberDef *md = (MemberDef *)d;
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
              MemberDef *enumType = 0;
              ClassDef *cd = newResolveTypedef(fileScope,md,&enumType,&spec,&type,actTemplParams);
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
static ClassDef *getResolvedClassRec(Definition *scope,
    FileDef *fileScope,
    const char *n,
    MemberDef **pTypeDef,
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
    stringToArgumentList(strippedTemplateParams,&actTemplParams);
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
  char *p=key.data();
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

  ClassDef *bestMatch=0;
  MemberDef *bestTypedef=0;
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
ClassDef *getResolvedClass(Definition *scope,
    FileDef *fileScope,
    const char *n,
    MemberDef **pTypeDef,
    QCString *pTemplSpec,
    bool mayBeUnlinkable,
    bool mayBeHidden,
    QCString *pResolvedType
    )
{
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
  ClassDef *result = getResolvedClassRec(scope,fileScope,n,pTypeDef,pTemplSpec,pResolvedType);
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

static const char constScope[]   = { 'c', 'o', 'n', 's', 't', ':' };
static const char virtualScope[] = { 'v', 'i', 'r', 't', 'u', 'a', 'l', ':' };

// Note: this function is not reentrant due to the use of static buffer!
QCString removeRedundantWhiteSpace(const QCString &s)
{
  static bool cliSupport = Config_getBool("CPP_CLI_SUPPORT");
  if (s.isEmpty()) return s;
  static GrowBuf growBuf;
  //int resultLen = 1024;
  //int resultPos = 0;
  //QCString result(resultLen);
  // we use growBuf.addChar(c) instead of result+=c to 
  // improve the performance of this function
  growBuf.clear();
  uint i;
  uint l=s.length();
  uint csp=0;
  uint vsp=0;
  for (i=0;i<l;i++)
  {
nextChar:
    char c=s.at(i);

    // search for "const"
    if (csp<6 && c==constScope[csp] && // character matches substring "const"
         (csp>0 ||                     // if it is the first character 
          i==0  ||                     // the previous may not be a digit
          !isId(s.at(i-1))
         )
       )
      csp++; 
    else // reset counter
      csp=0;

    // search for "virtual"
    if (vsp<8 && c==virtualScope[vsp] && // character matches substring "virtual"
         (vsp>0 ||                       // if it is the first character
          i==0  ||                       // the previous may not be a digit 
          !isId(s.at(i-1))
         )
       )
      vsp++;
    else // reset counter
      vsp=0;

    if (c=='"') // quoted string
    {
      i++;
      growBuf.addChar(c);
      while (i<l)
      {
        char cc=s.at(i);
        growBuf.addChar(cc);
        if (cc=='\\') // escaped character
        { 
          growBuf.addChar(s.at(i+1));
          i+=2; 
        }
        else if (cc=='"') // end of string
        { i++; goto nextChar; }
        else // any other character
        { i++; }
      }
    }
    else if (i<l-2 && c=='<' &&  // current char is a <
        (isId(s.at(i+1)) || isspace((uchar)s.at(i+1))) && // next char is an id char or space
        (i<8 || !findOperator(s,i)) // string in front is not "operator"
        )
    {
      growBuf.addChar('<');
      growBuf.addChar(' ');
    }
    else if (i>0 && c=='>' && // current char is a >
        (isId(s.at(i-1)) || isspace((uchar)s.at(i-1)) || s.at(i-1)=='*' || s.at(i-1)=='&') && // prev char is an id char or space
        (i<8 || !findOperator(s,i)) // string in front is not "operator"
        )
    {
      growBuf.addChar(' ');
      growBuf.addChar('>');
    }
    else if (i>0 && c==',' && !isspace((uchar)s.at(i-1))
        && ((i<l-1 && isId(s.at(i+1)))
          || (i<l-2 && s.at(i+1)=='$' && isId(s.at(i+2)))  // for PHP
          || (i<l-3 && s.at(i+1)=='&' && s.at(i+2)=='$' && isId(s.at(i+3)))))  // for PHP
    {
      growBuf.addChar(',');
      growBuf.addChar(' ');
    }
    else if (i>0 && 
         ((isId(s.at(i)) && s.at(i-1)==')') || 
          (s.at(i)=='\''  && s.at(i-1)==' ')
         )
        )
    {
      growBuf.addChar(' ');
      growBuf.addChar(s.at(i));
    }
    else if (c=='t' && csp==5 /*&& (i<5 || !isId(s.at(i-5)))*/ &&
             !(isId(s.at(i+1)) /*|| s.at(i+1)==' '*/ || 
               s.at(i+1)==')' || 
               s.at(i+1)==',' || 
               s.at(i+1)=='\0'
              )
            ) 
      // prevent const ::A from being converted to const::A
    {
      growBuf.addChar('t');
      growBuf.addChar(' ');
      if (s.at(i+1)==' ') i++;
      csp=0;
    }
    else if (c==':' && csp==6 /*&& (i<6 || !isId(s.at(i-6)))*/) 
      // replace const::A by const ::A
    {
      growBuf.addChar(' ');
      growBuf.addChar(':');
      csp=0;
    }
    else if (c=='l' && vsp==7 /*&& (i<7 || !isId(s.at(i-7)))*/ &&
             !(isId(s.at(i+1)) /*|| s.at(i+1)==' '*/ || 
               s.at(i+1)==')' || 
               s.at(i+1)==',' || 
               s.at(i+1)=='\0'
              )
            ) 
      // prevent virtual ::A from being converted to virtual::A
    {
      growBuf.addChar('l');
      growBuf.addChar(' ');
      if (s.at(i+1)==' ') i++;
      vsp=0;
    }
    else if (c==':' && vsp==8 /*&& (i<8 || !isId(s.at(i-8)))*/) 
      // replace virtual::A by virtual ::A
    {
      growBuf.addChar(' ');
      growBuf.addChar(':');
      vsp=0;
    }
    else if (!isspace((uchar)c) || // not a space
          ( i>0 && i<l-1 &&          // internal character
            (isId(s.at(i-1)) || s.at(i-1)==')' || s.at(i-1)==',' || s.at(i-1)=='>' || s.at(i-1)==']') && 
            (isId(s.at(i+1)) || 
             (i<l-2 && s.at(i+1)=='$' && isId(s.at(i+2))) || 
             (i<l-3 && s.at(i+1)=='&' && s.at(i+2)=='$' && isId(s.at(i+3)))
            )
          ) 
        )
    {
      if (c=='*' || c=='&' || c=='@' || c=='$')
      {  
        //uint rl=result.length();
        uint rl=growBuf.getPos();
        if ((rl>0 && (isId(growBuf.at(rl-1)) || growBuf.at(rl-1)=='>')) &&
            ((c!='*' && c!='&') || !findOperator2(s,i)) // avoid splitting operator* and operator->* and operator&
           ) 
        {
          growBuf.addChar(' ');
        }
      }
      else if (c=='-')
      {
        uint rl=growBuf.getPos();
        if (rl>0 && growBuf.at(rl-1)==')' && i<l-1 && s.at(i+1)=='>') // trailing return type ')->' => ') ->'
        {
          growBuf.addChar(' ');
        }
      }
      growBuf.addChar(c);
      if (cliSupport &&
          (c=='^' || c=='%') && i>1 && isId(s.at(i-1)) &&
          !findOperator(s,i)
         ) 
      {
        growBuf.addChar(' '); // C++/CLI: Type^ name and Type% name
      }
    }
  }
  //printf("removeRedundantWhiteSpace(`%s')=`%s'\n",s.data(),result.data());
  growBuf.addChar(0);
  //result.resize(resultPos);
  return growBuf.get();
}  

bool rightScopeMatch(const QCString &scope, const QCString &name)
{
  return (name==scope || // equal 
      (scope.right(name.length())==name && // substring 
       scope.at(scope.length()-name.length()-1)==':' // scope
      ) 
      );
}

bool leftScopeMatch(const QCString &scope, const QCString &name)
{
  return (name==scope || // equal 
      (scope.left(name.length())==name && // substring 
       scope.at(name.length())==':' // scope
      ) 
      );
}


void linkifyText(const TextGeneratorIntf &out,Definition *scope,
    FileDef *fileScope,Definition *self,
    const char *text, bool autoBreak,bool external,
    bool keepSpaces,int indentLevel)
{
  //printf("linkify=`%s'\n",text);
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
  while ((newIndex=regExp.match(txtStr,index,&matchLen))!=-1 && 
      (newIndex==0 || !(txtStr.at(newIndex-1)>='0' && txtStr.at(newIndex-1)<='9')) // avoid matching part of hex numbers
      )
  {
    // add non-word part to the result
    floatingIndex+=newIndex-skipIndex+matchLen;
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
      ClassDef     *cd=0;
      FileDef      *fd=0;
      MemberDef    *md=0;
      NamespaceDef *nd=0;
      GroupDef     *gd=0;
      //printf("** Match word '%s'\n",matchWord.data());

      MemberDef *typeDef=0;
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
          out.writeLink(md->getReference(),md->getOutputFileBase(),
              md->anchor(),word);
          //printf("found symbol %s\n",matchWord.data());
          found=TRUE;
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


QCString argListToString(ArgumentList *al,bool useCanonicalType,bool showDefVals)
{
  QCString result;
  if (al==0) return result;
  Argument *a=al->first();
  result+="(";
  while (a)
  {
    QCString type1 = useCanonicalType && !a->canType.isEmpty() ? 
      a->canType : a->type;
    QCString type2;
    int i=type1.find(")("); // hack to deal with function pointers
    if (i!=-1)
    {
      type2=type1.mid(i);
      type1=type1.left(i);
    }
    if (!a->attrib.isEmpty())
    {
      result+=a->attrib+" ";
    }
    if (!a->name.isEmpty() || !a->array.isEmpty())
    {
      result+= type1+" "+a->name+type2+a->array;
    }
    else
    {
      result+= type1+type2;
    }
    if (!a->defval.isEmpty() && showDefVals)
    {
      result+="="+a->defval;
    }
    a = al->next();
    if (a) result+=", "; 
  }
  result+=")";
  if (al->constSpecifier) result+=" const";
  if (al->volatileSpecifier) result+=" volatile";
  if (!al->trailingReturnType.isEmpty()) result+=" -> "+al->trailingReturnType;
  if (al->pureSpecifier) result+=" =0";
  return removeRedundantWhiteSpace(result);
}

QCString tempArgListToString(ArgumentList *al)
{
  QCString result;
  if (al==0) return result;
  result="<";
  Argument *a=al->first();
  while (a)
  {
    if (!a->name.isEmpty()) // add template argument name
    {
      if (a->type.left(4)=="out") // C# covariance
      {
        result+="out ";
      }
      else if (a->type.left(3)=="in") // C# contravariance
      {
        result+="in ";
      }
      result+=a->name;
    }
    else // extract name from type
    {
      int i=a->type.length()-1;
      while (i>=0 && isId(a->type.at(i))) i--;
      if (i>0)
      {
        result+=a->type.right(a->type.length()-i-1);
      }
      else // nothing found -> take whole name
      {
        result+=a->type;
      }
    }
    a=al->next();
    if (a) result+=", ";
  }
  result+=">";
  return removeRedundantWhiteSpace(result);
}


// compute the HTML anchors for a list of members
void setAnchors(ClassDef *cd,char id,MemberList *ml,int groupId)
{
  int count=0;
  if (ml==0) return;
  MemberListIterator mli(*ml);
  MemberDef *md;
  for (;(md=mli.current());++mli)
  {
    if (!md->isReference())
    {
      QCString anchor;
      if (groupId==-1)
        anchor.sprintf("%c%d",id,count++);
      else
        anchor.sprintf("%c%d_%d",id,groupId,count++);
      if (cd) anchor.prepend(escapeCharsInString(cd->name(),FALSE));
      md->setAnchor(anchor);
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

  QStrList& filterSrcList = Config_getList("FILTER_SOURCE_PATTERNS");
  QStrList& filterList    = Config_getList("FILTER_PATTERNS");

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
    return Config_getString("INPUT_FILTER");
  }
  else
  {
    return filterName;
  }
}


QCString transcodeCharacterStringToUTF8(const QCString &input)
{
  bool error=FALSE;
  static QCString inputEncoding = Config_getString("INPUT_ENCODING");
  const char *outputEncoding = "UTF-8";
  if (inputEncoding.isEmpty() || qstricmp(inputEncoding,outputEncoding)==0) return input;
  int inputSize=input.length();
  int outputSize=inputSize*4+1;
  QCString output(outputSize);
  void *cd = portable_iconv_open(outputEncoding,inputEncoding);
  if (cd==(void *)(-1)) 
  {
    err("error: unsupported character conversion: '%s'->'%s'\n",
        inputEncoding.data(),outputEncoding);
    error=TRUE;
  }
  if (!error)
  {
    size_t iLeft=inputSize;
    size_t oLeft=outputSize;
    char *inputPtr = input.data();
    char *outputPtr = output.data();
    if (!portable_iconv(cd, &inputPtr, &iLeft, &outputPtr, &oLeft))
    {
      outputSize-=oLeft;
      output.resize(outputSize+1);
      output.at(outputSize)='\0';
      //printf("iconv: input size=%d output size=%d\n[%s]\n",size,newSize,srcBuf.data());
    }
    else
    {
      err("error: failed to translate characters from %s to %s: check INPUT_ENCODING\ninput=[%s]\n",
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
      while ((size=f.readBlock(contents.data()+totalSize,bSize))==bSize)
      {
        totalSize+=bSize;
        contents.resize(totalSize+bSize); 
      }
      totalSize = filterCRLF(contents.data(),totalSize+size)+2;
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
      err("error: file `%s' not found\n",name);
      return "";
    }
    QCString filterName = getFileFilter(name,isSourceCode);
    if (filterName.isEmpty() || !filter)
    {
      f.setName(name);
      fileOpened=f.open(IO_ReadOnly);
      if (fileOpened)
      {
        int fsize=f.size();
        QCString contents(fsize+2);
        f.readBlock(contents.data(),fsize);
        if (fsize==0 || contents[fsize-1]=='\n') 
          contents[fsize]='\0';
        else
          contents[fsize]='\n'; // to help the scanner
        contents[fsize+1]='\0';
        f.close();
        int newSize = filterCRLF(contents.data(),fsize+2);
        if (newSize!=fsize+2) 
        {
          contents.resize(newSize);
        }
        return transcodeCharacterStringToUTF8(contents);
      }
    }
    else // filter the input
    {
      QCString cmd=filterName+" \""+name+"\"";
      Debug::print(Debug::ExtCmd,0,"Executing popen(`%s`)\n",cmd.data());
      FILE *f=portable_popen(cmd,"r");
      if (!f)
      {
        err("error: could not execute filter %s\n",filterName.data());
        return "";
      }
      const int bSize=4096;
      QCString contents(bSize);
      int totalSize=0;
      int size;
      while ((size=fread(contents.data()+totalSize,1,bSize,f))==bSize)
      {
        totalSize+=bSize;
        contents.resize(totalSize+bSize); 
      }
      totalSize = filterCRLF(contents.data(),totalSize+size)+2;
      contents.resize(totalSize);
      contents.at(totalSize-2)='\n'; // to help the scanner
      contents.at(totalSize-1)='\0';
      portable_pclose(f);
      Debug::print(Debug::FilterOutput, 0, "Filter output\n");
      Debug::print(Debug::FilterOutput,0,"-------------\n%s\n-------------\n",contents.data());
      return transcodeCharacterStringToUTF8(contents);
    }
  }
  if (!fileOpened)  
  {
    err("error: cannot open file `%s' for reading\n",name);
  }
  return "";
}

QCString dateToString(bool includeTime)
{
  QDateTime current = QDateTime::currentDateTime();
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
  const QDate &d=QDate::currentDate();
  QCString result;
  result.sprintf("%d", d.year());
  return result;
}

//----------------------------------------------------------------------
// recursive function that returns the number of branches in the 
// inheritance tree that the base class `bcd' is below the class `cd'

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
    err("error: Internal inconsistency: found class %s seem to have a recursive "
        "inheritance relation! Please send a bug report to dimitri@stack.nl\n",cd->name().data());
    return -1;
  }
  int m=maxInheritanceDepth; 
  if (cd->baseClasses())
  {
    BaseClassDef *bcdi = cd->baseClasses()->first();
    while (bcdi)
    {
      int mc=minClassDistance(bcdi->classDef,bcd,level+1);
      if (mc<m) m=mc;
      if (m<0) break;
      bcdi = cd->baseClasses()->next();
    }
  }
  return m;
}

Protection classInheritedProtectionLevel(ClassDef *cd,ClassDef *bcd,Protection prot,int level)
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
    err("error: Internal inconsistency: found class %s seem to have a recursive "
        "inheritance relation! Please send a bug report to dimitri@stack.nl\n",cd->name().data());
  }
  else if (cd->baseClasses())
  {
    BaseClassDef *bcdi = cd->baseClasses()->first();
    while (bcdi && prot!=Private)
    {
      Protection baseProt = classInheritedProtectionLevel(bcdi->classDef,bcd,bcdi->prot,level+1);
      if (baseProt==Private)   prot=Private;
      else if (baseProt==Protected) prot=Protected;
      bcdi = cd->baseClasses()->next();
    }
  }
exit:
  //printf("classInheritedProtectionLevel(%s,%s)=%d\n",cd->name().data(),bcd->name().data(),prot);
  return prot;
}

//static void printArgList(ArgumentList *al)
//{
//  if (al==0) return;
//  ArgumentListIterator ali(*al);
//  Argument *a;
//  printf("(");
//  for (;(a=ali.current());++ali)
//  {
//    printf("t=`%s' n=`%s' v=`%s' ",a->type.data(),!a->name.isEmpty()>0?a->name.data():"",!a->defval.isEmpty()>0?a->defval.data():""); 
//  }
//  printf(")");
//}

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
    //printf("Trying with scope=`%s'\n",scope.data());

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
  //printf("trimBaseClassScope level=%d `%s'\n",level,s.data());
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
    //printf("base class `%s'\n",cd->name().data());
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
  //printf("match argument start `%s|%s' <-> `%s|%s' using nsp=%p class=%p\n",
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
  if (strncmp(srcAType,"typename ",9)==0)
  {
    srcAType = srcAType.right(srcAType.length()-9); 
  }
  if (strncmp(dstAType,"typename ",9)==0)
  {
    dstAType = dstAType.right(dstAType.length()-9); 
  }

  srcAType = removeRedundantWhiteSpace(srcAType);
  dstAType = removeRedundantWhiteSpace(dstAType);

  //srcAType=stripTemplateSpecifiersFromScope(srcAType,FALSE);
  //dstAType=stripTemplateSpecifiersFromScope(dstAType,FALSE);

  //printf("srcA=`%s|%s' dstA=`%s|%s'\n",srcAType.data(),srcAName.data(),
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
      //printf("trimScope: `%s' <=> `%s'\n",srcAType.data(),dstAType.data());
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
      //printf("trimBaseClassScope: `%s' <=> `%s'\n",srcAType.data(),dstAType.data());
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
        // found a match. Note that now `signed int' and `signed' match, but
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


/*!
 * Matches the arguments list srcAl with the argument list dstAl
 * Returns TRUE if the argument lists are equal. Two argument list are 
 * considered equal if the number of arguments is equal and the types of all 
 * arguments are equal. Furthermore the const and volatile specifiers 
 * stored in the list should be equal.
 */
bool matchArguments(ArgumentList *srcAl,ArgumentList *dstAl,
    const char *cl,const char *ns,bool checkCV,
    NamespaceSDict *usingNamespaces,
    SDict<Definition> *usingClasses)
{
  QCString className=cl;
  QCString namespaceName=ns;

  // strip template specialization from class name if present
  //int til=className.find('<'),tir=className.find('>');
  //if (til!=-1 && tir!=-1 && tir>til) 
  //{
  //  className=className.left(til)+className.right(className.length()-tir-1);
  //}

  //printf("matchArguments(%s,%s) className=%s namespaceName=%s checkCV=%d usingNamespaces=%d usingClasses=%d\n",
  //    srcAl ? argListToString(srcAl).data() : "",
  //    dstAl ? argListToString(dstAl).data() : "",
  //    cl,ns,checkCV,
  //    usingNamespaces?usingNamespaces->count():0,
  //    usingClasses?usingClasses->count():0
  //    );

  if (srcAl==0 || dstAl==0)
  {
    bool match = srcAl==dstAl; // at least one of the members is not a function
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
  if ( srcAl->count()==0 && dstAl->count()==1 && 
      dstAl->getFirst()->type=="void" )
  { // special case for finding match between func() and func(void)
    Argument *a=new Argument;
    a->type = "void";
    srcAl->append(a);
    MATCH
    return TRUE;
  }
  if ( dstAl->count()==0 && srcAl->count()==1 &&
      srcAl->getFirst()->type=="void" )
  { // special case for finding match between func(void) and func()
    Argument *a=new Argument;
    a->type = "void";
    dstAl->append(a);
    MATCH
    return TRUE;
  }

  if (srcAl->count() != dstAl->count())
  {
    NOMATCH
    return FALSE; // different number of arguments -> no match
  }

  if (checkCV)
  {
    if (srcAl->constSpecifier != dstAl->constSpecifier) 
    {
      NOMATCH
      return FALSE; // one member is const, the other not -> no match
    }
    if (srcAl->volatileSpecifier != dstAl->volatileSpecifier)
    {
      NOMATCH
      return FALSE; // one member is volatile, the other not -> no match
    }
  }

  // so far the argument list could match, so we need to compare the types of
  // all arguments.
  ArgumentListIterator srcAli(*srcAl),dstAli(*dstAl);
  Argument *srcA,*dstA;
  for (;(srcA=srcAli.current(),dstA=dstAli.current());++srcAli,++dstAli)
  { 
    if (!matchArgument(srcA,dstA,className,namespaceName,
          usingNamespaces,usingClasses))
    {
      NOMATCH
      return FALSE;
    }
  }
  MATCH
  return TRUE; // all arguments match 
}

#endif

#if 0
static QCString resolveSymbolName(FileDef *fs,Definition *symbol,QCString &templSpec)
{
  ASSERT(symbol!=0);
  if (symbol->definitionType()==Definition::TypeMember && 
      ((MemberDef*)symbol)->isTypedef()) // if symbol is a typedef then try
    // to resolve it
  {
    MemberDef *md = 0;
    ClassDef *cd = newResolveTypedef(fs,(MemberDef*)symbol,&md,&templSpec);
    if (cd)
    {
      return cd->qualifiedName()+templSpec;
    }
    else if (md)
    {
      return md->qualifiedName();
    }
  }
  return symbol->qualifiedName();
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
static QCString extractCanonicalType(Definition *d,FileDef *fs,QCString type);

QCString getCanonicalTemplateSpec(Definition *d,FileDef *fs,const QCString& spec)
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
    Definition *d,FileDef *fs,const QCString &word,
    QCString *tSpec,int count=0)
{
  if (count>10) return word; // oops recursion

  QCString symName,scope,result,templSpec,tmpName;
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

  ClassDef *cd = 0;
  MemberDef *mType = 0;
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
      result = resolvedType;
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

static QCString extractCanonicalType(Definition *d,FileDef *fs,QCString type)
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

static QCString extractCanonicalArgType(Definition *d,FileDef *fs,const Argument *arg)
{
  QCString type = arg->type.stripWhiteSpace();
  QCString name = arg->name;
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
  if (!arg->array.isEmpty())
  {
    type+=arg->array;
  }

  return extractCanonicalType(d,fs,type);
}

static bool matchArgument2(
    Definition *srcScope,FileDef *srcFileScope,Argument *srcA,
    Definition *dstScope,FileDef *dstFileScope,Argument *dstA
    )
{
  //printf(">> match argument: %s::`%s|%s' (%s) <-> %s::`%s|%s' (%s)\n",
  //    srcScope ? srcScope->name().data() : "",
  //    srcA->type.data(),srcA->name.data(),srcA->canType.data(),
  //    dstScope ? dstScope->name().data() : "",
  //    dstA->type.data(),dstA->name.data(),dstA->canType.data());

  //if (srcA->array!=dstA->array) // nomatch for char[] against char
  //{
  //  NOMATCH
  //  return FALSE;
  //}
  QCString sSrcName = " "+srcA->name;
  QCString sDstName = " "+dstA->name;
  QCString srcType  = srcA->type;
  QCString dstType  = dstA->type;
  stripIrrelevantConstVolatile(srcType);
  stripIrrelevantConstVolatile(dstType);
  //printf("'%s'<->'%s'\n",sSrcName.data(),dstType.right(sSrcName.length()).data());
  //printf("'%s'<->'%s'\n",sDstName.data(),srcType.right(sDstName.length()).data());
  if (sSrcName==dstType.right(sSrcName.length()))
  { // case "unsigned int" <-> "unsigned int i"
    srcA->type+=sSrcName;
    srcA->name="";
    srcA->canType=""; // invalidate cached type value
  }
  else if (sDstName==srcType.right(sDstName.length()))
  { // case "unsigned int i" <-> "unsigned int"
    dstA->type+=sDstName;
    dstA->name="";
    dstA->canType=""; // invalidate cached type value
  }

  if (srcA->canType.isEmpty())
  {
    srcA->canType = extractCanonicalArgType(srcScope,srcFileScope,srcA);
  }
  if (dstA->canType.isEmpty())
  {
    dstA->canType = extractCanonicalArgType(dstScope,dstFileScope,dstA);
  }

  if (srcA->canType==dstA->canType)
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
bool matchArguments2(Definition *srcScope,FileDef *srcFileScope,ArgumentList *srcAl,
    Definition *dstScope,FileDef *dstFileScope,ArgumentList *dstAl,
    bool checkCV
    )
{
  //printf("*** matchArguments2\n");
  ASSERT(srcScope!=0 && dstScope!=0);

  if (srcAl==0 || dstAl==0)
  {
    bool match = srcAl==dstAl; // at least one of the members is not a function
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
  if ( srcAl->count()==0 && dstAl->count()==1 && 
      dstAl->getFirst()->type=="void" )
  { // special case for finding match between func() and func(void)
    Argument *a=new Argument;
    a->type = "void";
    srcAl->append(a);
    MATCH
    return TRUE;
  }
  if ( dstAl->count()==0 && srcAl->count()==1 &&
      srcAl->getFirst()->type=="void" )
  { // special case for finding match between func(void) and func()
    Argument *a=new Argument;
    a->type = "void";
    dstAl->append(a);
    MATCH
    return TRUE;
  }

  if (srcAl->count() != dstAl->count())
  {
    NOMATCH
    return FALSE; // different number of arguments -> no match
  }

  if (checkCV)
  {
    if (srcAl->constSpecifier != dstAl->constSpecifier) 
    {
      NOMATCH
      return FALSE; // one member is const, the other not -> no match
    }
    if (srcAl->volatileSpecifier != dstAl->volatileSpecifier)
    {
      NOMATCH
      return FALSE; // one member is volatile, the other not -> no match
    }
  }

  // so far the argument list could match, so we need to compare the types of
  // all arguments.
  ArgumentListIterator srcAli(*srcAl),dstAli(*dstAl);
  Argument *srcA,*dstA;
  for (;(srcA=srcAli.current(),dstA=dstAli.current());++srcAli,++dstAli)
  { 
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
void mergeArguments(ArgumentList *srcAl,ArgumentList *dstAl,bool forceNameOverwrite)
{
  //printf("mergeArguments `%s', `%s'\n",
  //    argListToString(srcAl).data(),argListToString(dstAl).data());

  if (srcAl==0 || dstAl==0 || srcAl->count()!=dstAl->count())
  {
    return; // invalid argument lists -> do not merge
  }

  ArgumentListIterator srcAli(*srcAl),dstAli(*dstAl);
  Argument *srcA,*dstA;
  for (;(srcA=srcAli.current(),dstA=dstAli.current());++srcAli,++dstAli)
  {
    if (srcA->defval.isEmpty() && !dstA->defval.isEmpty())
    {
      //printf("Defval changing `%s'->`%s'\n",srcA->defval.data(),dstA->defval.data());
      srcA->defval=dstA->defval.copy();
    }
    else if (!srcA->defval.isEmpty() && dstA->defval.isEmpty())
    {
      //printf("Defval changing `%s'->`%s'\n",dstA->defval.data(),srcA->defval.data());
      dstA->defval=srcA->defval.copy();
    }

    // fix wrongly detected const or volatile specifiers before merging.
    // example: "const A *const" is detected as type="const A *" name="const"
    if (srcA->name=="const" || srcA->name=="volatile")
    {
      srcA->type+=" "+srcA->name;
      srcA->name.resize(0);
    }
    if (dstA->name=="const" || dstA->name=="volatile")
    {
      dstA->type+=" "+dstA->name;
      dstA->name.resize(0);
    }

    if (srcA->type==dstA->type)
    {
      //printf("1. merging %s:%s <-> %s:%s\n",srcA->type.data(),srcA->name.data(),dstA->type.data(),dstA->name.data());
      if (srcA->name.isEmpty() && !dstA->name.isEmpty())
      {
        //printf("type: `%s':=`%s'\n",srcA->type.data(),dstA->type.data());
        //printf("name: `%s':=`%s'\n",srcA->name.data(),dstA->name.data());
        srcA->type = dstA->type.copy();
        srcA->name = dstA->name.copy();
      }
      else if (!srcA->name.isEmpty() && dstA->name.isEmpty())
      {
        //printf("type: `%s':=`%s'\n",dstA->type.data(),srcA->type.data());
        //printf("name: `%s':=`%s'\n",dstA->name.data(),srcA->name.data());
        dstA->type = srcA->type.copy();
        dstA->name = dstA->name.copy();
      }
      else if (!srcA->name.isEmpty() && !dstA->name.isEmpty())
      {
        //printf("srcA->name=%s dstA->name=%s\n",srcA->name.data(),dstA->name.data());
        if (forceNameOverwrite)
        {
          srcA->name = dstA->name;
        }
        else
        {
          if (srcA->docs.isEmpty() && !dstA->docs.isEmpty())
          {
            srcA->name = dstA->name;
          }
          else if (!srcA->docs.isEmpty() && dstA->docs.isEmpty())
          {
            dstA->name = srcA->name;
          }
        }
      }
    }
    else
    {
      //printf("2. merging '%s':'%s' <-> '%s':'%s'\n",srcA->type.data(),srcA->name.data(),dstA->type.data(),dstA->name.data());
      srcA->type=srcA->type.stripWhiteSpace();
      dstA->type=dstA->type.stripWhiteSpace();
      if (srcA->type+" "+srcA->name==dstA->type) // "unsigned long:int" <-> "unsigned long int:bla"
      {
        srcA->type+=" "+srcA->name;
        srcA->name=dstA->name;
      }
      else if (dstA->type+" "+dstA->name==srcA->type) // "unsigned long int bla" <-> "unsigned long int"
      {
        dstA->type+=" "+dstA->name;
        dstA->name=srcA->name;
      }
      else if (srcA->name.isEmpty() && !dstA->name.isEmpty())
      {
        srcA->name = dstA->name;
      }
      else if (dstA->name.isEmpty() && !srcA->name.isEmpty())
      {
        dstA->name = srcA->name;
      }
    }
    int i1=srcA->type.find("::"),
        i2=dstA->type.find("::"),
        j1=srcA->type.length()-i1-2,
        j2=dstA->type.length()-i2-2;
    if (i1!=-1 && i2==-1 && srcA->type.right(j1)==dstA->type)
    {
      //printf("type: `%s':=`%s'\n",dstA->type.data(),srcA->type.data());
      //printf("name: `%s':=`%s'\n",dstA->name.data(),srcA->name.data());
      dstA->type = srcA->type.left(i1+2)+dstA->type;
      dstA->name = dstA->name.copy();
    }
    else if (i1==-1 && i2!=-1 && dstA->type.right(j2)==srcA->type)
    {
      //printf("type: `%s':=`%s'\n",srcA->type.data(),dstA->type.data());
      //printf("name: `%s':=`%s'\n",dstA->name.data(),srcA->name.data());
      srcA->type = dstA->type.left(i2+2)+srcA->type;
      srcA->name = dstA->name.copy();
    }
    if (srcA->docs.isEmpty() && !dstA->docs.isEmpty())
    {
      srcA->docs = dstA->docs.copy();
    }
    else if (dstA->docs.isEmpty() && !srcA->docs.isEmpty())
    {
      dstA->docs = srcA->docs.copy();
    }
    //printf("Merge argument `%s|%s' `%s|%s'\n",
    //  srcA->type.data(),srcA->name.data(),
    //  dstA->type.data(),dstA->name.data());
  }
}

static void findMembersWithSpecificName(MemberName *mn,
                                        const char *args,
                                        bool checkStatics,
                                        FileDef *currentFile,
                                        bool checkCV,
                                        const char *forceTagFile,
                                        QList<MemberDef> &members)
{
  //printf("  Function with global scope name `%s' args=`%s'\n",
  //       mn->memberName(),args);
  MemberListIterator mli(*mn);
  MemberDef *md;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    FileDef  *fd=md->getFileDef();
    GroupDef *gd=md->getGroupDef();
    //printf("  md->name()=`%s' md->args=`%s' fd=%p gd=%p current=%p ref=%s\n",
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
      if (args && !md->isDefine() && strcmp(args,"()")!=0)
      {
        argList=new ArgumentList;
        LockingPtr<ArgumentList> mdAl = md->argumentList();
        stringToArgumentList(args,argList);
        match=matchArguments2(
            md->getOuterScope(),fd,mdAl.pointer(),
            Doxygen::globalScope,fd,argList,
            checkCV); 
        delete argList; argList=0;
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
 * Searches for a member definition given its name `memberName' as a string.
 * memberName may also include a (partial) scope to indicate the scope
 * in which the member is located.
 *
 * The parameter `scName' is a string representing the name of the scope in 
 * which the link was found.
 *
 * In case of a function args contains a string representation of the 
 * argument list. Passing 0 means the member has no arguments. 
 * Passing "()" means any argument list will do, but "()" is preferred.
 *
 * The function returns TRUE if the member is known and documented or
 * FALSE if it is not.
 * If TRUE is returned parameter `md' contains a pointer to the member 
 * definition. Furthermore exactly one of the parameter `cd', `nd', or `fd' 
 * will be non-zero:
 *   - if `cd' is non zero, the member was found in a class pointed to by cd.
 *   - if `nd' is non zero, the member was found in a namespace pointed to by nd.
 *   - if `fd' is non zero, the member was found in the global namespace of
 *     file fd.
 */
bool getDefs(const QCString &scName,
             const QCString &memberName, 
             const char *args,
             MemberDef *&md, 
             ClassDef *&cd, 
             FileDef *&fd, 
             NamespaceDef *&nd, 
             GroupDef *&gd,
             bool forceEmptyScope,
             FileDef *currentFile,
             bool checkCV,
             const char *forceTagFile
            )
{
  fd=0, md=0, cd=0, nd=0, gd=0;
  if (memberName.isEmpty()) return FALSE; /* empty name => nothing to link */

  QCString scopeName=scName;
  scopeName = substitute(scopeName,"\\","::"); // for PHP
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

  //printf("mScope=`%s' mName=`%s'\n",mScope.data(),mName.data());

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

      ClassDef *fcd=getResolvedClass(Doxygen::globalScope,0,className);
      //printf("Trying class scope %s: %p\n",className.data(),fcd);
      // todo: fill in correct fileScope!
      if (fcd &&  // is it a documented class
          fcd->isLinkable() 
         )
      {
        //printf("  Found fcd=%p\n",fcd);
        MemberListIterator mmli(*mn);
        MemberDef *mmd;
        int mdist=maxInheritanceDepth; 
        ArgumentList *argList=0;
        if (args)
        {
          argList=new ArgumentList;
          stringToArgumentList(args,argList);
        }
        for (mmli.toFirst();(mmd=mmli.current());++mmli)
        {
          //if (mmd->isLinkable())
          //{
          LockingPtr<ArgumentList> mmdAl = mmd->argumentList();
          bool match=args==0 || 
            matchArguments2(mmd->getOuterScope(),mmd->getFileDef(),mmdAl.pointer(),
                fcd,fcd->getFileDef(),argList,
                checkCV
                );  
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
          //}
        }
        if (argList)
        {
          delete argList; argList=0;
        }
        if (mdist==maxInheritanceDepth && args && strcmp(args,"()")==0)
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
        //printf("  >Succes=%d\n",mdist<maxInheritanceDepth);
        if (mdist<maxInheritanceDepth) 
        {
          if (!md->isLinkable()) 
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
    MemberListIterator mmli(*mn);
    MemberDef *mmd, *fuzzy_mmd = 0;
    ArgumentList *argList = 0;
    bool hasEmptyArgs = args && strcmp(args, "()") == 0;

    if (args)
      stringToArgumentList(args, argList = new ArgumentList);

    for (mmli.toFirst(); (mmd = mmli.current()); ++mmli)
    {
      if (!mmd->isLinkable() || (!mmd->isRelated() && !mmd->isForeign()) ||
           !mmd->getClassDef())
        continue;

      if (!args) break;

      QCString className = mmd->getClassDef()->name();

      LockingPtr<ArgumentList> mmdAl = mmd->argumentList();
      if (matchArguments2(mmd->getOuterScope(),mmd->getFileDef(),mmdAl.pointer(),
            Doxygen::globalScope,mmd->getFileDef(),argList,
            checkCV
            )
         ) break;

      if (!fuzzy_mmd && hasEmptyArgs)
        fuzzy_mmd = mmd;
    }

    if (argList) delete argList, argList = 0;

    mmd = mmd ? mmd : fuzzy_mmd;

    if (mmd)
    {
      md = mmd;
      cd = mmd->getClassDef();
      return TRUE;
    }
  }


  // maybe an namespace, file or group member ?
  //printf("Testing for global symbol scopeName=`%s' mScope=`%s' :: mName=`%s'\n",
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
        //printf("Function inside existing namespace `%s'\n",namespaceName.data());
        bool found=FALSE;
        MemberListIterator mmli(*mn);
        MemberDef *mmd;
        for (mmli.toFirst();((mmd=mmli.current()) && !found);++mmli)
        {
          //printf("mmd->getNamespaceDef()=%p fnd=%p\n",
          //    mmd->getNamespaceDef(),fnd);
          if (mmd->getNamespaceDef()==fnd /* && mmd->isLinkable() */ )
          { // namespace is found
            bool match=TRUE;
            ArgumentList *argList=0;
            if (args && strcmp(args,"()")!=0)
            {
              argList=new ArgumentList;
              LockingPtr<ArgumentList> mmdAl = mmd->argumentList();
              stringToArgumentList(args,argList);
              match=matchArguments2(
                  mmd->getOuterScope(),mmd->getFileDef(),mmdAl.pointer(),
                  fnd,mmd->getFileDef(),argList,
                  checkCV); 
            }
            if (match)
            {
              nd=fnd;
              md=mmd;
              found=TRUE;
            }
            if (args)
            {
              delete argList; argList=0;
            }
          }
        }
        if (!found && args && !strcmp(args,"()")) 
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
            gd=md->getGroupDef();
            if (gd && gd->isLinkable()) nd=0; else gd=0;
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
      if (members.count()!=1 && args && !strcmp(args,"()"))
      {
        // no exact match found, but if args="()" an arbitrary 
        // member will do
        md=mn->last();
        while (md /* && md->isLinkable()*/)
        {
          //printf("Found member `%s'\n",md->name().data());
          //printf("member is linkable md->name()=`%s'\n",md->name().data());
          fd=md->getFileDef();
          gd=md->getGroupDef();
          if (
              (gd && gd->isLinkable()) || (fd && fd->isLinkable()) 
             )
          {
            members.append(md);
          }
          md=mn->prev();
        }
      }
      //printf("found %d candidate members\n",members.count());
      if (members.count()>0) // at least one match
      {
        md=members.last();
      }
      if (md) // found a matching global member
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
  //printf("getScopeDefs: docScope=`%s' scope=`%s'\n",docScope,scope);
  if (scopeName.isEmpty()) return FALSE;

  bool explicitGlobalScope=FALSE;
  if (scopeName.at(0)==':' && scopeName.at(1)==':')
  {
    scopeName=scopeName.right(scopeName.length()-2);  
    explicitGlobalScope=TRUE;
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
    /* out */ Definition **resContext,
    /* out */ MemberDef  **resMember,
    bool lookForSpecialization,
    FileDef *currentFile,
    bool checkScope
    )
{
  QCString tsName = name;
  //bool memberScopeFirst = tsName.find('#')!=-1;
  QCString fullName = substitute(tsName,"#","::");
  fullName = removeRedundantWhiteSpace(substitute(fullName,".","::"));

  int bracePos=fullName.findRev('('); // reverse is needed for operator()(...)
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

  MemberDef    *md = 0;
  ClassDef     *cd = 0;
  FileDef      *fd = 0;
  NamespaceDef *nd = 0;
  GroupDef     *gd = 0;

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
  //printf("resolveRef: %s not found!\n",name);

  return FALSE;
}

QCString linkToText(SrcLangExt lang,const char *link,bool isFileName)
{
  //static bool optimizeOutputJava = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
  QCString result=link;
  if (!result.isEmpty())
  {
    // replace # by ::
    result=substitute(result,"#","::");
    // replace . by ::
    if (!isFileName) result=substitute(result,".","::");
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
 * `scName' is the name of the scope that contains the documentation 
 * string that is returned.
 * `name' is the name that we want to link to.
 * `name' may have five formats:
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
    /* out */ Definition **resContext,
    /* out */ QCString &resAnchor
    )
{
  *resContext=0;

  QCString linkRef=lr;
  //printf("ResolveLink linkRef=%s inSee=%d\n",lr,inSeeBlock);
  FileDef  *fd;
  GroupDef *gd;
  PageDef  *pd;
  ClassDef *cd;
  DirDef   *dir;
  NamespaceDef *nd;
  bool ambig;
  if (linkRef.isEmpty()) // no reference name!
  {
    return FALSE;
  }
  else if ((pd=Doxygen::pageSDict->find(linkRef))) // link to a page
  {
    GroupDef *gd = pd->getGroupDef();
    if (gd)
    {
      SectionInfo *si=0;
      if (!pd->name().isEmpty()) si=Doxygen::sectionDict[pd->name()];
      *resContext=gd;
      if (si) resAnchor = si->label;
    }
    else
    {
      *resContext=pd;
    }
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
    MemberDef *md;
    bool res = resolveRef(scName,lr,TRUE,resContext,&md);
    if (md) resAnchor=md->anchor();
    return res;
  }
}


//----------------------------------------------------------------------
// General function that generates the HTML code for a reference to some
// file, class or member from text `lr' within the context of class `clName'. 
// This link has the text 'lt' (if not 0), otherwise `lr' is used as a
// basis for the link's text.
// returns TRUE if a link could be generated.

bool generateLink(OutputDocInterface &od,const char *clName,
    const char *lr,bool inSeeBlock,const char *lt)
{
  //printf("generateLink(clName=%s,lr=%s,lr=%s)\n",clName,lr,lt);
  Definition *compound;
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
        linkText=((GroupDef *)compound)->groupTitle(); // use group's title as link
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

  QCString key;
  key.sprintf("%p:",fnDict);
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

  QCString name=convertToQCString(QDir::cleanDirPath(n));
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
#if defined(_WIN32) || defined(__MACOSX__) // Windows or MacOSX
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

QCString substituteKeywords(const QCString &s,const char *title,
         const char *projName,const char *projNum,const char *projBrief)
{
  QCString result = s;
  if (title) result = substitute(result,"$title",title);
  result = substitute(result,"$datetime",dateToString(TRUE));
  result = substitute(result,"$date",dateToString(FALSE));
  result = substitute(result,"$year",yearToString());
  result = substitute(result,"$doxygenversion",versionString);
  result = substitute(result,"$projectname",projName);
  result = substitute(result,"$projectnumber",projNum);
  result = substitute(result,"$projectbrief",projBrief);
  result = substitute(result,"$projectlogo",stripPath(Config_getString("PROJECT_LOGO")));
  return result;
}

//----------------------------------------------------------------------

/*! Returns the character index within \a name of the first prefix
 *  in Config_getList("IGNORE_PREFIX") that matches \a name at the left hand side,
 *  or zero if no match was found
 */ 
int getPrefixIndex(const QCString &name)
{
  if (name.isEmpty()) return 0;
  static QStrList &sl = Config_getList("IGNORE_PREFIX");
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
    cd->visited=FALSE;
  }
}

//----------------------------------------------------------------------------

void initClassHierarchy(ClassSDict *cl)
{
  ClassSDict::Iterator cli(*cl);
  ClassDef *cd;
  for ( ; (cd=cli.current()); ++cli)
  {
    cd->visited=FALSE;
    initBaseClassHierarchy(cd->baseClasses());
  }
}

//----------------------------------------------------------------------------

bool hasVisibleRoot(BaseClassList *bcl)
{
  if (bcl)
  {
    BaseClassListIterator bcli(*bcl);
    for ( ; bcli.current(); ++bcli)
    {
      ClassDef *cd=bcli.current()->classDef;
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
  static bool caseSenseNames = Config_getBool("CASE_SENSE_NAMES");
  static GrowBuf growBuf;
  growBuf.clear();
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
      case '=': growBuf.addStr("_0A"); break;
      case '$': growBuf.addStr("_0B"); break;
      case '\\': growBuf.addStr("_0C"); break;
      default: 
                if (c<0)
                {
                  static char map[] = "0123456789ABCDEF";
                  char ids[5];
                  unsigned char id = (unsigned char)c;
                  ids[0]='_';
                  ids[1]='x';
                  ids[2]=map[id>>4];
                  ids[3]=map[id&0xF];
                  ids[4]=0;
                  growBuf.addStr(ids);
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

/*! This function determines the file name on disk of an item
 *  given its name, which could be a class name with template 
 *  arguments, so special characters need to be escaped.
 */
QCString convertNameToFile(const char *name,bool allowDots,bool allowUnderscore)
{
  static bool shortNames = Config_getBool("SHORT_NAMES");
  static bool createSubdirs = Config_getBool("CREATE_SUBDIRS");
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
      MD5SigToString(md5_sig,sigStr.data(),33);
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
  if (Config_getBool("CREATE_SUBDIRS"))
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
  if (Config_getBool("CREATE_SUBDIRS"))
  {
    // create 4096 subdirectories
    int l1,l2;
    for (l1=0;l1<16;l1++)
    {
      d.mkdir(QString().sprintf("d%x",l1));
      for (l2=0;l2<256;l2++)
      {
        d.mkdir(QString().sprintf("d%x/d%02x",l1,l2));
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
  //printf("extractNamespace `%s' => `%s|%s'\n",scopeName.data(),
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
        ((cd=getClass(scope.left(si)))==0 || cd->templateArguments()==0) 
        ) 
    { 
      //printf("Tried `%s'\n",(scope.left(si)+templ).data()); 
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
  //printf("insertTemplateSpecifierInScope(`%s',`%s')=%s\n",
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


/*! Converts a string to an XML-encoded string */
QCString convertToXML(const char *s)
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
      case '&':  growBuf.addStr("&amp;");  break;
      case '\'': growBuf.addStr("&apos;"); break; 
      case '"':  growBuf.addStr("&quot;"); break;
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

QCString convertToJSString(const char *s)
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
      case '"':  growBuf.addStr("\\\""); break;
      case '\\': growBuf.addStr("\\\\"); break;
      default:   growBuf.addChar(c);   break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}


QCString convertCharEntitiesToUTF8(const QCString &s)
{
  static QDict<char> entityMap(127);
  static bool init=TRUE;
  QCString result;
  static QRegExp entityPat("&[a-zA-Z]+;");

  if (init)
  {
    entityMap.insert("copy",       "\xC2\xA9");
    entityMap.insert("tm",         "\xE2\x84\xA2");
    entityMap.insert("trade",      "\xE2\x84\xA2");
    entityMap.insert("reg",        "\xC2\xAE");
    entityMap.insert("lsquo",      "\xE2\x80\x98");
    entityMap.insert("rsquo",      "\xE2\x80\x99");
    entityMap.insert("ldquo",      "\xE2\x80\x9C");
    entityMap.insert("rdquo",      "\xE2\x80\x9D");
    entityMap.insert("ndash",      "\xE2\x80\x93");
    entityMap.insert("mdash",      "\xE2\x80\x94");
    entityMap.insert("Auml",       "\xC3\x84");
    entityMap.insert("Euml",       "\xC3\x8B");
    entityMap.insert("Iuml",       "\xC3\x8F");
    entityMap.insert("Ouml",       "\xC3\x96");
    entityMap.insert("Uuml",       "\xC3\x9C");
    entityMap.insert("Yuml",       "\xC5\xB8");
    entityMap.insert("auml",       "\xC3\xA4");
    entityMap.insert("euml",       "\xC3\xAB");
    entityMap.insert("iuml",       "\xC3\xAF");
    entityMap.insert("ouml",       "\xC3\xB6");
    entityMap.insert("uuml",       "\xC3\xBC");
    entityMap.insert("yuml",       "\xC3\xBF");
    entityMap.insert("Aacute",     "\xC3\x81");
    entityMap.insert("Eacute",     "\xC3\x89");
    entityMap.insert("Iacute",     "\xC3\x8D");
    entityMap.insert("Oacute",     "\xC3\x93");
    entityMap.insert("Uacute",     "\xC3\x9A");
    entityMap.insert("aacute",     "\xC3\xA1");
    entityMap.insert("eacute",     "\xC3\xA9");
    entityMap.insert("iacute",     "\xC3\xAD");
    entityMap.insert("oacute",     "\xC3\xB3");
    entityMap.insert("uacute",     "\xC3\xBA");
    entityMap.insert("Agrave",     "\xC3\x80");
    entityMap.insert("Egrave",     "\xC3\x88");
    entityMap.insert("Igrave",     "\xC3\x8C");
    entityMap.insert("Ograve",     "\xC3\x92");
    entityMap.insert("Ugrave",     "\xC3\x99");
    entityMap.insert("agrave",     "\xC3\xA0");
    entityMap.insert("egrave",     "\xC3\xA8");
    entityMap.insert("igrave",     "\xC3\xAC");
    entityMap.insert("ograve",     "\xC3\xB2");
    entityMap.insert("ugrave",     "\xC3\xB9");
    entityMap.insert("Acirc",      "\xC3\x82");
    entityMap.insert("Ecirc",      "\xC3\x8A");
    entityMap.insert("Icirc",      "\xC3\x8E");
    entityMap.insert("Ocirc",      "\xC3\x94");
    entityMap.insert("Ucirc",      "\xC3\x9B");
    entityMap.insert("acirc",      "\xC3\xA2");
    entityMap.insert("ecirc",      "\xC3\xAA");
    entityMap.insert("icirc",      "\xC3\xAE");
    entityMap.insert("ocirc",      "\xC3\xB4");
    entityMap.insert("ucirc",      "\xC3\xBB");
    entityMap.insert("Atilde",     "\xC3\x83");
    entityMap.insert("Ntilde",     "\xC3\x91");
    entityMap.insert("Otilde",     "\xC3\x95");
    entityMap.insert("atilde",     "\xC3\xA3");
    entityMap.insert("ntilde",     "\xC3\xB1");
    entityMap.insert("otilde",     "\xC3\xB5");
    entityMap.insert("szlig",      "\xC3\x9F");
    entityMap.insert("Ccedil",     "\xC3\x87");
    entityMap.insert("ccedil",     "\xC3\xA7");
    entityMap.insert("Aring",      "\xC3\x85");
    entityMap.insert("aring",      "\xC3\xA5");
    entityMap.insert("nbsp",       "\xC2\xA0");
    entityMap.insert("Gamma",      "\xCE\x93");
    entityMap.insert("Delta",      "\xCE\x94");
    entityMap.insert("Theta",      "\xCE\x98");
    entityMap.insert("Lambda",     "\xCE\x9B");
    entityMap.insert("Xi",         "\xCE\x9E");
    entityMap.insert("Pi",         "\xCE\xA0");
    entityMap.insert("Sigma",      "\xCE\xA3");
    entityMap.insert("Upsilon",    "\xCE\xA5");
    entityMap.insert("Phi",        "\xCE\xA6");
    entityMap.insert("Psi",        "\xCE\xA8");
    entityMap.insert("Omega",      "\xCE\xA9");
    entityMap.insert("alpha",      "\xCE\xB1");
    entityMap.insert("beta",       "\xCE\xB2");
    entityMap.insert("gamma",      "\xCE\xB3");
    entityMap.insert("delta",      "\xCE\xB4");
    entityMap.insert("epsilon",    "\xCE\xB5");
    entityMap.insert("zeta",       "\xCE\xB6");
    entityMap.insert("eta",        "\xCE\xB8");
    entityMap.insert("theta",      "\xCE\xB8");
    entityMap.insert("iota",       "\xCE\xB9");
    entityMap.insert("kappa",      "\xCE\xBA");
    entityMap.insert("lambda",     "\xCE\xBB");
    entityMap.insert("mu",         "\xCE\xBC");
    entityMap.insert("nu",         "\xCE\xBD");
    entityMap.insert("xi",         "\xCE\xBE");
    entityMap.insert("pi",         "\xCF\x80");
    entityMap.insert("rho",        "\xCF\x81");
    entityMap.insert("sigma",      "\xCF\x83");
    entityMap.insert("tau",        "\xCF\x84");
    entityMap.insert("upsilon",    "\xCF\x85");
    entityMap.insert("phi",        "\xCF\x86");
    entityMap.insert("chi",        "\xCF\x87");
    entityMap.insert("psi",        "\xCF\x88");
    entityMap.insert("omega",      "\xCF\x89");
    entityMap.insert("sigmaf",     "\xCF\x82");
    entityMap.insert("sect",       "\xC2\xA7");
    entityMap.insert("deg",        "\xC2\xB0");
    entityMap.insert("prime",      "\xE2\x80\xB2");
    entityMap.insert("Prime",      "\xE2\x80\xB2");
    entityMap.insert("infin",      "\xE2\x88\x9E");
    entityMap.insert("empty",      "\xE2\x88\x85");
    entityMap.insert("plusmn",     "\xC2\xB1");
    entityMap.insert("times",      "\xC3\x97");
    entityMap.insert("minus",      "\xE2\x88\x92");
    entityMap.insert("sdot",       "\xE2\x8B\x85");
    entityMap.insert("part",       "\xE2\x88\x82");
    entityMap.insert("nabla",      "\xE2\x88\x87");
    entityMap.insert("radic",      "\xE2\x88\x9A");
    entityMap.insert("perp",       "\xE2\x8A\xA5");
    entityMap.insert("sum",        "\xE2\x88\x91");
    entityMap.insert("int",        "\xE2\x88\xAB");
    entityMap.insert("prod",       "\xE2\x88\x8F");
    entityMap.insert("sim",        "\xE2\x88\xBC");
    entityMap.insert("asymp",      "\xE2\x89\x88");
    entityMap.insert("ne",         "\xE2\x89\xA0");
    entityMap.insert("equiv",      "\xE2\x89\xA1");
    entityMap.insert("prop",       "\xE2\x88\x9D");
    entityMap.insert("le",         "\xE2\x89\xA4");
    entityMap.insert("ge",         "\xE2\x89\xA5");
    entityMap.insert("larr",       "\xE2\x86\x90");
    entityMap.insert("rarr",       "\xE2\x86\x92");
    entityMap.insert("isin",       "\xE2\x88\x88");
    entityMap.insert("notin",      "\xE2\x88\x89");
    entityMap.insert("lceil",      "\xE2\x8C\x88");
    entityMap.insert("rceil",      "\xE2\x8C\x89");
    entityMap.insert("lfloor",     "\xE2\x8C\x8A");
    entityMap.insert("rfloor",     "\xE2\x8C\x8B");
    init=FALSE;
  }

  if (s==0) return result;
  int p,i=0,l;
  while ((p=entityPat.match(s,i,&l))!=-1)
  {
    if (p>i) result+=s.mid(i,p-i);
    QCString entity = s.mid(p+1,l-2);
    char *code = entityMap.find(entity);
    if (code)
    {
      result+=code;
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
    Definition *context)
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
      LockingPtr<MemberList> fmdl=md->enumFieldList();
      if (fmdl!=0)
      {
        MemberDef *fmd=fmdl->first();
        while (fmd)
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
                    context,
                    groupId,
                    info->header,
                    info->doc,
                    info->docFile
                    );
                (*ppMemberGroupSDict)->append(groupId,mg);
              }
              mg->insertMember(fmd); // insert in member group
              fmd->setMemberGroup(mg);
            }
          }
          fmd=fmdl->next();
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
              context,
              groupId,
              info->header,
              info->doc,
              info->docFile
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

  name.resize(0);
  templSpec.resize(0);
  int i,l;
  int typeLen=type.length();
  if (typeLen>0)
  {
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

/*! Substitutes any occurrence of a formal argument from argument list
 *  \a formalArgs in \a name by the corresponding actual argument in
 *  argument list \a actualArgs. The result after substitution
 *  is returned as a string. The argument \a name is used to
 *  prevent recursive substitution.
 */
QCString substituteTemplateArgumentsInString(
    const QCString &name,
    ArgumentList *formalArgs,
    ArgumentList *actualArgs)
{
  //printf("substituteTemplateArgumentsInString(name=%s formal=%s actualArg=%s)\n",
  //    name.data(),argListToString(formalArgs).data(),argListToString(actualArgs).data());
  if (formalArgs==0) return name;
  QCString result;
  static QRegExp re("[a-z_A-Z\\x80-\\xFF][a-z_A-Z0-9\\x80-\\xFF]*");
  int p=0,l,i;
  // for each identifier in the base class name (e.g. B<T> -> B and T)
  while ((i=re.match(name,p,&l))!=-1)
  {
    result += name.mid(p,i-p);
    QCString n = name.mid(i,l);
    ArgumentListIterator formAli(*formalArgs);
    Argument *formArg;
    Argument *actArg=actualArgs->first();

    // if n is a template argument, then we substitute it
    // for its template instance argument.
    bool found=FALSE;
    for (formAli.toFirst();
        (formArg=formAli.current()) && !found;
        ++formAli,actArg=actualArgs->next()
        )
    {
      if (formArg->type.left(6)=="class " && formArg->name.isEmpty())
      {
        formArg->name = formArg->type.mid(6);
        formArg->type = "class";
      }
      if (formArg->type.left(9)=="typename " && formArg->name.isEmpty())
      {
        formArg->name = formArg->type.mid(9);
        formArg->type = "typename";
      }
      if (formArg->type=="class" || formArg->type=="typename" || formArg->type.left(8)=="template")
      {
        //printf("n=%s formArg->type='%s' formArg->name='%s' formArg->defval='%s'\n",
        //  n.data(),formArg->type.data(),formArg->name.data(),formArg->defval.data());
        //printf(">> formArg->name='%s' actArg->type='%s' actArg->name='%s'\n",
        //    formArg->name.data(),actArg->type.data(),actArg->name.data()
        //    );
        if (formArg->name==n && actArg && !actArg->type.isEmpty()) // base class is a template argument
        {
          // replace formal argument with the actual argument of the instance
          if (!leftScopeMatch(actArg->type,n)) 
            // the scope guard is to prevent recursive lockup for 
            // template<class A> class C : public<A::T>, 
            // where A::T would become A::T::T here, 
            // since n==A and actArg->type==A::T
            // see bug595833 for an example
          {
            if (actArg->name.isEmpty())
            {
              result += actArg->type+" "; 
              found=TRUE;
            }
            else 
              // for case where the actual arg is something like "unsigned int"
              // the "int" part is in actArg->name.
            {
              result += actArg->type+" "+actArg->name+" "; 
              found=TRUE;
            }
          }
        }
        else if (formArg->name==n && 
                 actArg==0 && 
                 !formArg->defval.isEmpty() &&
                 formArg->defval!=name /* to prevent recursion */
            )
        {
          result += substituteTemplateArgumentsInString(formArg->defval,formalArgs,actualArgs)+" ";
          found=TRUE;
        }
      }
      else if (formArg->name==n && 
               actArg==0 && 
               !formArg->defval.isEmpty() &&
               formArg->defval!=name /* to prevent recursion */
              )
      {
        result += substituteTemplateArgumentsInString(formArg->defval,formalArgs,actualArgs)+" ";
        found=TRUE;
      }
    }
    if (!found) result += n;
    p=i+l;
  }
  result+=name.right(name.length()-p);
  //printf("      Inheritance relation %s -> %s\n",
  //    name.data(),result.data());
  return result.stripWhiteSpace();
}

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
  while ((i=leftScope.findRev("::",p))!=-1)
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
    QList<SectionInfo> * /*anchors*/,
    const char *fileName,int startLine,
    const QList<ListItemInfo> *sli,
    GroupDef *gd,
    TagInfo *tagInfo,
    SrcLangExt lang
    )
{
  PageDef *pd=0;
  //printf("addRelatedPage(name=%s gd=%p)\n",name,gd);
  if ((pd=Doxygen::pageSDict->find(name)) && !tagInfo)
  {
    // append documentation block to the page.
    pd->setDocumentation(doc,fileName,startLine);
    //printf("Adding page docs `%s' pi=%p name=%s\n",doc.data(),pi,name);
  }
  else // new page
  {
    QCString baseName=name;
    if (baseName.right(4)==".tex") 
      baseName=baseName.left(baseName.length()-4);
    else if (baseName.right(Doxygen::htmlFileExtension.length())==Doxygen::htmlFileExtension)
      baseName=baseName.left(baseName.length()-Doxygen::htmlFileExtension.length());

    QCString title=ptitle.stripWhiteSpace();
    pd=new PageDef(fileName,startLine,baseName,doc,title);

    pd->setRefItems(sli);
    pd->setLanguage(lang);

    if (tagInfo)
    {
      pd->setReference(tagInfo->tagName);
    }

    pd->setFileName(convertNameToFile(pd->name(),FALSE,TRUE));

    //printf("Appending page `%s'\n",baseName.data());
    Doxygen::pageSDict->append(baseName,pd);

    if (gd) gd->addPage(pd);

    if (!pd->title().isEmpty())
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
      SectionInfo *si=new SectionInfo(
          file,pd->name(),pd->title(),SectionInfo::Page,0,pd->getReference());
      //printf("si->label=`%s' si->definition=%s si->fileName=`%s'\n",
      //      si->label.data(),si->definition?si->definition->name().data():"<none>",
      //      si->fileName.data());
      //printf("  SectionInfo: sec=%p sec->fileName=%s\n",si,si->fileName.data());
      //printf("Adding section key=%s si->fileName=%s\n",pageName.data(),si->fileName.data());
      Doxygen::sectionDict.append(pd->name(),si);
    }
  }
  return pd;
}

//----------------------------------------------------------------------------

void addRefItem(const QList<ListItemInfo> *sli,
    const char *key, 
    const char *prefix, const char *name,const char *title,const char *args)
{
  //printf("addRefItem(sli=%p,key=%s,prefix=%s,name=%s,title=%s,args=%s)\n",sli,key,prefix,name,title,args);
  if (sli)
  {
    QListIterator<ListItemInfo> slii(*sli);
    ListItemInfo *lii;
    for (slii.toFirst();(lii=slii.current());++slii)
    {
      RefList *refList = Doxygen::xrefLists->find(lii->type);
      if (refList
          &&
          (
           // either not a built-in list or the list is enabled
           (lii->type!="todo"       || Config_getBool("GENERATE_TODOLIST")) &&
           (lii->type!="test"       || Config_getBool("GENERATE_TESTLIST")) &&
           (lii->type!="bug"        || Config_getBool("GENERATE_BUGLIST"))  &&
           (lii->type!="deprecated" || Config_getBool("GENERATE_DEPRECATEDLIST"))
          )
         )
      {
        RefItem *item = refList->getRefItem(lii->itemId);
        ASSERT(item!=0);

        item->prefix = prefix;
        item->name   = name;
        item->title  = title;
        item->args   = args;

        refList->insertIntoList(key,item);

      }
    }
  }
}

void addGroupListToTitle(OutputList &ol,Definition *d)
{
  LockingPtr<GroupList> groups = d->partOfGroups();
  if (groups!=0) // write list of group to which this definition belongs
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.writeString("<div class=\"ingroups\">");
    GroupListIterator gli(*groups);
    GroupDef *gd;
    bool first=TRUE;
    for (gli.toFirst();(gd=gli.current());++gli)
    {
      if (!first) { ol.writeString(" &#124; "); } else first=FALSE; 
      ol.writeObjectLink(gd->getReference(),
          gd->getOutputFileBase(),0,gd->groupTitle());
    }
    ol.writeString("</div>");
    ol.popGeneratorState();
  }
}

void filterLatexString(FTextStream &t,const char *str,
    bool insideTabbing,bool insidePre,bool insideItem)
{
  if (str==0) return;
  //printf("filterLatexString(%s)\n",str);
  //if (strlen(str)<2) stackTrace();
  const unsigned char *p=(const unsigned char *)str;
  unsigned char c;
  unsigned char pc='\0';
  while (*p)
  {
    c=*p++;

    if (insidePre)
    {
      switch(c)
      {
        case '\\': t << "\\(\\backslash\\)"; break;
        case '{':  t << "\\{"; break;
        case '}':  t << "\\}"; break;
        case '_':  t << "\\_"; break;
        default: 
                   t << (char)c;
      }
    }
    else
    {
      switch(c)
      {
        case '#':  t << "\\#";           break;
        case '$':  t << "\\$";           break;
        case '%':  t << "\\%";           break;
        case '^':  t << "$^\\wedge$";    break;
        case '&':  t << "\\&";           break;
        case '*':  t << "$\\ast$";       break;
        case '_':  if (!insideTabbing) t << "\\-";  
                   t << "\\_"; 
                   if (!insideTabbing) t << "\\-";  
                   break;
        case '{':  t << "\\{";           break;
        case '}':  t << "\\}";           break;
        case '<':  t << "$<$";           break;
        case '>':  t << "$>$";           break;
        case '|':  t << "$|$";           break;
        case '~':  t << "$\\sim$";       break;
        case '[':  if (Config_getBool("PDF_HYPERLINKS") || insideItem) 
                     t << "\\mbox{[}"; 
                   else
                     t << "[";
                   break;
        case ']':  if (pc=='[') t << "$\\,$";
                     if (Config_getBool("PDF_HYPERLINKS") || insideItem)
                       t << "\\mbox{]}";
                     else
                       t << "]";             
                   break;
        case '-':  t << "-\\/";
                   break;
        case '\\': if (*p=='<') 
                   { t << "$<$"; p++; }
                   else if (*p=='>')
                   { t << "$>$"; p++; } 
                   else  
                   { t << "\\textbackslash{}"; }
                   break;           
        case '"':  { t << "\\char`\\\"{}"; }
                   break;

        default:   
                   //if (!insideTabbing && forceBreaks && c!=' ' && *p!=' ')
                   if (!insideTabbing && 
                       ((c>='A' && c<='Z' && pc!=' ' && pc!='\0') || (c==':' && pc!=':') || (pc=='.' && isId(c)))
                      )
                   {
                     t << "\\-";
                   }
                   t << (char)c;
      }
    }
    pc = c;
  }
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
    char* nxtTag = g_nextTag.data() + g_nextTag.length() - 1;
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

QCString stripExtension(const char *fName)
{
  QCString result=fName;
  if (result.right(Doxygen::htmlFileExtension.length())==Doxygen::htmlFileExtension)
  {
    result=result.left(result.length()-Doxygen::htmlFileExtension.length());
  }
  return result;
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
      else if (i+l<(int)s.length() && isspace(s.at(i+l))) 
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
  static bool searchEngine = Config_getBool("SEARCHENGINE");
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
//  language       parser     parser option
  { "idl",         "c",       SrcLangExt_IDL      },
  { "java",        "c",       SrcLangExt_Java     },
  { "javascript",  "c",       SrcLangExt_JS       },
  { "csharp",      "c",       SrcLangExt_CSharp   },
  { "d",           "c",       SrcLangExt_D        },
  { "php",         "c",       SrcLangExt_PHP      },
  { "objective-c", "c",       SrcLangExt_ObjC     },
  { "c",           "c",       SrcLangExt_Cpp      },
  { "c++",         "c",       SrcLangExt_Cpp      },
  { "python",      "python",  SrcLangExt_Python   },
  { "fortran",     "fortran", SrcLangExt_Fortran  },
  { "vhdl",        "vhdl",    SrcLangExt_VHDL     },
  { "dbusxml",     "dbusxml", SrcLangExt_XML      },
  { "tcl",         "tcl",     SrcLangExt_Tcl      },
  { "md",          "md",      SrcLangExt_Markdown },
  { 0,             0,        (SrcLangExt)0        }
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
  g_extLookup.setAutoDelete(TRUE);
  //                  extension      parser id
  updateLanguageMapping(".idl",      "idl"); 
  updateLanguageMapping(".ddl",      "idl"); 
  updateLanguageMapping(".odl",      "idl"); 
  updateLanguageMapping(".java",     "java");
  updateLanguageMapping(".as",       "javascript"); 
  updateLanguageMapping(".js",       "javascript");
  updateLanguageMapping(".cs",       "csharp");
  updateLanguageMapping(".d",        "d");
  updateLanguageMapping(".php",      "php"); 
  updateLanguageMapping(".php4",     "php");
  updateLanguageMapping(".php5",     "php");
  updateLanguageMapping(".inc",      "php");
  updateLanguageMapping(".phtml",    "php");
  updateLanguageMapping(".m",        "objective-c");
  updateLanguageMapping(".M",        "objective-c");
  updateLanguageMapping(".mm",       "objective-c");
  updateLanguageMapping(".py",       "python");
  updateLanguageMapping(".f",        "fortran");
  updateLanguageMapping(".for",      "fortran");
  updateLanguageMapping(".f90",      "fortran");
  updateLanguageMapping(".vhd",      "vhdl");
  updateLanguageMapping(".vhdl",     "vhdl");
  updateLanguageMapping(".tcl",      "tcl");
  updateLanguageMapping(".ucf",      "vhdl");
  updateLanguageMapping(".qsf",      "vhdl");
  updateLanguageMapping(".md",       "md");
  updateLanguageMapping(".markdown", "md");

  //updateLanguageMapping(".xml",   "dbusxml");
}

SrcLangExt getLanguageFromFileName(const QCString fileName)
{
  int i = fileName.findRev('.');
  if (i!=-1) // name has an extension
  {
    QCString extStr=fileName.right(fileName.length()-i).lower();
    if (!extStr.isEmpty()) // non-empty extension
    {
      int *pVal=g_extLookup.find(extStr);
      if (pVal) // listed extension
      {
        //printf("getLanguageFromFileName(%s)=%x\n",extStr.data(),*pVal);
        return (SrcLangExt)*pVal; 
      }
    }
  }
  //printf("getLanguageFromFileName(%s) not found!\n",fileName.data());
  return SrcLangExt_Cpp; // not listed => assume C-ish language.
}

//--------------------------------------------------------------------------

MemberDef *getMemberFromSymbol(Definition *scope,FileDef *fileScope, 
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
          bestMatch = (MemberDef *)d;
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
      bestMatch = (MemberDef *)d;
      //printf("new best match %s distance=%d\n",bestMatch->qualifiedName().data(),distance);
    }
  }
  return bestMatch;
}

/*! Returns true iff the given name string appears to be a typedef in scope. */
bool checkIfTypedef(Definition *scope,FileDef *fileScope,const char *n)
{
  MemberDef *bestMatch = getMemberFromSymbol(scope,fileScope,n);

  if (bestMatch && bestMatch->isTypedef())
    return TRUE; // closest matching symbol is a typedef
  else
    return FALSE;
}


int nextUtf8CharPosition(const QCString &utf8Str,int len,int startPos)
{
  int bytes=1;
  if (startPos>=len) return len;
  char c = utf8Str[startPos];
  if (c<0) // multibyte utf-8 character
  {
    bytes++;   // 1xxx.xxxx: >=2 byte character
    if (((uchar)c&0xE0)==0xE0)
    {
      bytes++; // 111x.xxxx: >=3 byte character
    }
    if (((uchar)c&0xF0)==0xF0)
    {
      bytes++; // 1111.xxxx: 4 byte character
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
  QCString result = convertCharEntitiesToUTF8(s.data());
  int i=0;
  int charCnt=0;
  int l=result.length();
  bool addEllipsis=FALSE;
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
      if (result.at(i)>=0 && isspace(result.at(i)))
      {
        addEllipsis=TRUE;
      }
      else if (result.at(i)==',' || 
               result.at(i)=='.' || 
               result.at(i)=='?')
      {
        break;
      }
    }
  }
  if (addEllipsis || charCnt==100) result=result.left(i)+"...";
  return result.data();
}

//--------------------------------------------------------------------------------------

static QDict<void> aliasesProcessed;

static QCString expandAliasRec(const QCString s);

struct Marker
{
  Marker(int p, int n,int s) : pos(p),number(n),size(s) {}
  int pos; // position in the string
  int number; // argument number
  int size; // size of the marker
};

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
    if (argList.at(i)==',' && (i==0 || argList.at(i-1)!='\\')) 
    {
      args.append(new QCString(argList.mid(s,i-s)));
      s=i+1; // start of next argument
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
      result+=*args.at(m->number-1);
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

static QCString expandAliasRec(const QCString s)
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
    if (aliasesProcessed.find(cmd)==0 && aliasText) // expand the alias
    {
      //printf("is an alias!\n");
      aliasesProcessed.insert(cmd,(void *)0x8);
      QCString val = *aliasText;
      if (hasArgs)
      {
        val = replaceAliasArguments(val,args);
        //printf("replace '%s'->'%s' args='%s'\n",
        //       aliasText->data(),val.data(),args.data());
      }
      result+=expandAliasRec(val);
      aliasesProcessed.remove(cmd);
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
    if (argList.at(i)==',' && (i==0 || argList.at(i-1)!='\\')) count++;
  }
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

void writeTypeConstraints(OutputList &ol,Definition *d,ArgumentList *al)
{
  if (al==0) return;
  ol.startConstraintList(theTranslator->trTypeConstraints()); 
  ArgumentListIterator ali(*al);
  Argument *a;
  for (;(a=ali.current());++ali)
  {
    ol.startConstraintParam();
    ol.parseText(a->name);
    ol.endConstraintParam();
    ol.startConstraintType();
    linkifyText(TextGeneratorOLImpl(ol),d,0,0,a->type);
    ol.endConstraintType();
    ol.startConstraintDocs();
    ol.parseDoc(d->docFile(),d->docLine(),d,0,a->docs,TRUE,FALSE);
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
    err("error: unsupported character conversion: '%s'->'%s': %s\n"
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
    newSize = tmpBufSize-oLeft;
    srcBuf.shrink(newSize);
    strncpy(srcBuf.data(),tmpBuf.data(),newSize);
    //printf("iconv: input size=%d output size=%d\n[%s]\n",size,newSize,srcBuf.data());
  }
  else
  {
    err("%s: error: failed to translate characters from %s to %s: check INPUT_ENCODING\n",
        fileName,inputEncoding,outputEncoding);
    exit(1);
  }
  portable_iconv_close(cd);
  return newSize;
}

//! read a file name \a fileName and optionally filter and transcode it
bool readInputFile(const char *fileName,BufStr &inBuf)
{
  // try to open file
  int size=0;
  //uint oldPos = dest.curPos();
  //printf(".......oldPos=%d\n",oldPos);

  QFileInfo fi(fileName);
  if (!fi.exists()) return FALSE;
  QCString filterName = getFileFilter(fileName,FALSE);
  if (filterName.isEmpty())
  {
    QFile f(fileName);
    if (!f.open(IO_ReadOnly))
    {
      err("error: could not open file %s\n",fileName);
      return FALSE;
    }
    size=fi.size();
    // read the file
    inBuf.skip(size);
    if (f.readBlock(inBuf.data()/*+oldPos*/,size)!=size)
    {
      err("error: problems while reading file %s\n",fileName);
      return FALSE;
    }
  }
  else
  {
    QCString cmd=filterName+" \""+fileName+"\"";
    Debug::print(Debug::ExtCmd,0,"Executing popen(`%s`)\n",cmd.data());
    FILE *f=portable_popen(cmd,"r");
    if (!f)
    {
      err("error: could not execute filter %s\n",filterName.data());
      return FALSE;
    }
    const int bufSize=1024;
    char buf[bufSize];
    int numRead;
    while ((numRead=fread(buf,1,bufSize,f))>0) 
    {
      //printf(">>>>>>>>Reading %d bytes\n",numRead);
      inBuf.addArray(buf,numRead),size+=numRead;
    }
    portable_pclose(f);
    inBuf.at(inBuf.curPos()) ='\0';
    Debug::print(Debug::FilterOutput, 0, "Filter output\n");
    Debug::print(Debug::FilterOutput,0,"-------------\n%s\n-------------\n",inBuf.data());
  }

  int start=0;
  if (size>=2 &&
      ((inBuf.at(0)==-1 && inBuf.at(1)==-2) || // Litte endian BOM
       (inBuf.at(0)==-2 && inBuf.at(1)==-1)    // big endian BOM
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
        Config_getString("INPUT_ENCODING"),"UTF-8");
  }

  inBuf.addChar('\n'); /* to prevent problems under Windows ? */

  // and translate CR's
  size=inBuf.curPos()-start;
  int newSize=filterCRLF(inBuf.data()+start,size);
  //printf("filter char at %p size=%d newSize=%d\n",dest.data()+oldPos,size,newSize);
  if (newSize!=size) // we removed chars
  {
    inBuf.shrink(newSize); // resize the array
    //printf(".......resizing from %d to %d result=[%s]\n",oldPos+size,oldPos+newSize,dest.data());
  }
  inBuf.at(inBuf.curPos())='\0';
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
// returns TRUE if the name of the file represented by `fi' matches
// one of the file patterns in the `patList' list.

bool patternMatch(const QFileInfo &fi,const QStrList *patList)
{
  bool found=FALSE;
  if (patList)
  { 
    QStrListIterator it(*patList);
    QCString pattern;
    for (it.toFirst();(pattern=it.current());++it)
    {
      if (!pattern.isEmpty() && !found)
      {
        int i=pattern.find('=');
        if (i!=-1) pattern=pattern.left(i); // strip of the extension specific filter name

#if defined(_WIN32) || defined(__MACOSX__) // Windows or MacOSX
        QRegExp re(pattern,FALSE,TRUE); // case insensitive match 
#else                // unix
        QRegExp re(pattern,TRUE,TRUE);  // case sensitive match
#endif
        found = found || re.match(fi.fileName().data())!=-1 || 
                         re.match(fi.filePath().data())!=-1 ||
                         re.match(fi.absFilePath().data())!=-1;
        //printf("Matching `%s' against pattern `%s' found=%d\n",
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
  static bool extLinksInWindow = Config_getBool("EXT_LINKS_IN_WINDOW");
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
      }
      if (!href) result.prepend("doxygen=\""+ref+":");
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

/** Writes the intensity only bitmap representated by \a data as an image to 
 *  directory \a dir using the colors defined by HTML_COLORSTYLE_*.
 */
void writeColoredImgData(const char *dir,ColoredImgDataItem data[])
{
  static int hue   = Config_getInt("HTML_COLORSTYLE_HUE");
  static int sat   = Config_getInt("HTML_COLORSTYLE_SAT");
  static int gamma = Config_getInt("HTML_COLORSTYLE_GAMMA");
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
    Doxygen::indexList.addImageFile(data->name);
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
  static int hue   = Config_getInt("HTML_COLORSTYLE_HUE");
  static int sat   = Config_getInt("HTML_COLORSTYLE_SAT");
  static int gamma = Config_getInt("HTML_COLORSTYLE_GAMMA");
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
      err("error: could not write to file %s\n",dest.data());
      return FALSE;
    }
  }
  else
  {
    err("error: could not open user specified file %s\n",src.data());
    return FALSE;
  }
  return TRUE;
}

/** Returns the section of text, in between a pair of markers. 
 *  Full lines are returned, excluding the lines on which the markers appear.
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
    }
  }
  //printf("text=[%s]\n",text.mid(l1,l2-l1).data());
  return text.mid(l1,l2-l1);
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
    case SrcLangExt_Tcl:      return "Tcl";
    case SrcLangExt_Markdown: return "Markdown";
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

/** Corrects URL \a url according to the relative path \a relPath.
 *  Returns the corrected URL. For absolute URLs no correction will be done.
 */
QCString correctURL(const QCString &url,const QCString &relPath)
{
  QCString result = url;
  if (!relPath.isEmpty() && 
      url.left(5)!="http:" && url.left(6)!="https:" && 
      url.left(4)!="ftp:"  && url.left(5)!="file:")
  {
    result.prepend(relPath);
  }
  return result;
}

//---------------------------------------------------------------------------

bool protectionLevelVisible(Protection prot)
{
  static bool extractPrivate = Config_getBool("EXTRACT_PRIVATE");
  static bool extractPackage = Config_getBool("EXTRACT_PACKAGE");

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
  static int tabSize=Config_getInt("TAB_SIZE");
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


bool fileVisibleInIndex(FileDef *fd,bool &genSourceFile)
{
  static bool allExternals = Config_getBool("ALLEXTERNALS");
  bool isDocFile = fd->isDocumentationFile();
  genSourceFile = !isDocFile && fd->generateSourceFile();
  return ( ((allExternals && fd->isLinkable()) ||
            fd->isLinkableInProject()
           ) && 
           !isDocFile
         );
}


