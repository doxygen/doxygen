/*****************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2003 by Dimitri van Heesch.
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
#ifdef _WIN32
#include <windows.h>
#endif

#include "qtbc.h"
#include <qregexp.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qdatetime.h>

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

#ifndef _WIN32
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
extern char **environ;
#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define popen _popen
#define pclose _pclose
#endif

//------------------------------------------------------------------------
// TextGeneratorOLImpl implementation
//------------------------------------------------------------------------

TextGeneratorOLImpl::TextGeneratorOLImpl(OutputDocInterface &od) : m_od(od) 
{
}

void TextGeneratorOLImpl::writeString(const char *s) const
{ 
  m_od.docify(s); 
}

void TextGeneratorOLImpl::writeBreak() const
{ 
  m_od.pushGeneratorState();
  m_od.disableAllBut(OutputGenerator::Html);
  m_od.lineBreak();
  m_od.popGeneratorState();
}

void TextGeneratorOLImpl::writeLink(const char *extRef,const char *file,
                                    const char *anchor,const char *text
                                   ) const
{
  m_od.writeObjectLink(extRef,file,anchor,text);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------

    
/*! Implements an interruptable system call on Unix/Windows */
int iSystem(const char *command,const char *args,bool isBatchFile)
{
  QTime time;
  time.start();
#ifndef _WIN32
  isBatchFile=isBatchFile;
  /*! taken from the system() manpage on my Linux box */
  int pid,status=0;

  if (command==0) return 1;

#ifdef _OS_SOLARIS // for Solaris we use vfork since it is more memory efficient

  // on Solaris fork() duplicates the memory usage
  // so we use vfork instead
  
  char buf[4096];
  strcpy(buf,command);
  strcat(buf," ");
  strcat(buf,args);
  
  // spawn shell
  if ((pid=vfork())<0)
  {
    status=-1;
  }
  else if (pid==0)
  {
     execl("/bin/sh","sh","-c",buf,(char*)0);
     _exit(127);
  }
  else
  {
    while (waitpid(pid,&status,0 )<0)
    {
      if (errno!=EINTR)
      {
        status=-1;
        break;
      }
    }
  }
  Doxygen::sysElapsedTime+=((double)time.elapsed())/1000.0;
  return status;

#else  // Other Unices just use fork

  pid = fork();
  if (pid==-1) return -1;
  if (pid==0)
  {
    char buf[4096];
    strcpy(buf,command);
    strcat(buf," ");
    strcat(buf,args);
    const char * argv[4];
    argv[0] = "sh";
    argv[1] = "-c";
    argv[2] = buf;
    argv[3] = 0;
    execve("/bin/sh",(char * const *)argv,environ);
    exit(127);
  }
  for (;;)
  {
    if (waitpid(pid,&status,0)==-1)
    {
      if (errno!=EINTR) return -1;
    }
    else
    {
      Doxygen::sysElapsedTime+=((double)time.elapsed())/1000.0;
      return status;
    }
  }
#endif // _OS_SOLARIS

#else
  if (isBatchFile)
  {
    QCString fullCmd = command;
    fullCmd += " ";
    fullCmd += args;
    return system(fullCmd);
  }
  else
  {
    SHELLEXECUTEINFO sInfo = {
      sizeof(SHELLEXECUTEINFO),   /* structure size */
      SEE_MASK_NOCLOSEPROCESS,    /* leave the process running */
      NULL,                       /* window handle */
      NULL,                       /* action to perform: open */
      command,                    /* file to execute */
      args,                       /* argument list */ 
      NULL,                       /* use current working dir */
      SW_HIDE,                    /* minimize on start-up */
      0,                          /* application instance handle */
      NULL,                       /* ignored: id list */
      NULL,                       /* ignored: class name */
      NULL,                       /* ignored: key class */
      0,                          /* ignored: hot key */
      NULL,                       /* ignored: icon */
      NULL                        /* resulting application handle */
    };
    if (!ShellExecuteEx(&sInfo))
    {
      return -1;
    }
    else if (sInfo.hProcess)      /* executable was launched, wait for it to finish */
    {
      WaitForSingleObject(sInfo.hProcess,INFINITE); 
      CloseHandle(sInfo.hProcess);
    }
  }
  Doxygen::sysElapsedTime+=((double)time.elapsed())/1000.0;
  return 0;
#endif

}





// an inheritance tree of depth of 100000 should be enough for everyone :-)
const int maxInheritanceDepth = 100000; 

bool isId(char c)
{
  return c=='_' || isalnum(c);
}


/*! 
  Removes all anoymous scopes from string s
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

// replace anonymous scopes with __anonymous__ 
QCString replaceAnonymousScopes(const QCString &s)
{
  QCString result;
  if (s.isEmpty()) return result;
  static QRegExp re("@[0-9]+");
  int i,l,sl=s.length();
  int p=0;
  while ((i=re.match(s,p,&l))!=-1)
  {
    result+=s.mid(p,i-p);
    result+="__anonymous__";
    p=i+l;
  }
  result+=s.right(sl-p);
  //printf("replaceAnonymousScopes(`%s')=`%s'\n",s.data(),result.data());
  return result;
}


// strip annonymous left hand side part of the scope
QCString stripAnonymousNamespaceScope(const QCString &s)
{
#if 0
  int oi=0,i=0,p=0;
  p=s.find('@');
  if (p==-1) return s;
  while (s.at(p)=='@' && (i=s.find("::@",p))!=-1 && 
         Doxygen::namespaceDict[s.left(i)]!=0) { oi=i; p=i+2; }
  if (oi==0) 
  {
    //printf("stripAnonymousNamespaceScope(`%s')=`%s'\n",s.data(),s.data());
    return s;
  }
  else 
  {
    //printf("stripAnonymousNamespaceScope(`%s')=`%s'\n",s.data(),s.right(s.length()-oi-2).data());
    return s.right(s.length()-oi-2);
  }
#endif

  int i,p=0,l;
  QCString newScope;
  while ((i=getScopeFragment(s,p,&l))!=-1)
  {
    //printf("Scope fragment %s\n",s.mid(i,l).data());
    if (Doxygen::namespaceSDict[s.left(i+l)]!=0)
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

/*! strip part of \a path if it matches
 *  one of the paths in the Config_getList("STRIP_FROM_PATH") list
 */
QCString stripFromPath(const QCString &path)
{
  QStrList &l = Config_getList("STRIP_FROM_PATH");
  const char *s=l.first();
  while (s)
  {
    QCString prefix = s;
    if (stricmp(path.left(prefix.length()),prefix)==0) // case insensitive compare
    {
      return path.right(path.length()-prefix.length());
    }
    s = l.next();
  }
  return path;
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
      n.right(3)==".ii"   || // inline
      n.right(4)==".ixx"  ||
      n.right(4)==".ipp"  ||
      n.right(4)==".i++"  ||
      n.right(4)==".inl"
     ) return Entry::SOURCE_SEC;
  if (n.right(2)==".h"   || // header
      n.right(3)==".hh"  ||
      n.right(4)==".hxx" ||
      n.right(4)==".hpp" ||
      n.right(4)==".h++" ||
      n.right(4)==".idl" ||
      n.right(5)==".pidl"
     ) return Entry::HEADER_SEC;
  return 0;
}

// TODO: remove this!
QCString resolveTypeDef(Definition *context,const QCString &qualifiedName,
                        Definition **typedefContext)
{
  //printf("<<resolveTypeDef(%s,%s)\n",
  //          context ? context->name().data() : "<none>",qualifiedName.data());
  QCString result;
  if (qualifiedName.isEmpty()) return result;

  Definition *mContext=context;
  if (typedefContext) *typedefContext=context;

  // see if the qualified name has a scope part
  int scopeIndex = qualifiedName.findRev("::");
  QCString resName=qualifiedName;
  if (scopeIndex!=-1) // strip scope part for the name
  {
    resName=qualifiedName.right(qualifiedName.length()-scopeIndex-2);
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
        mnd=&Doxygen::memberNameSDict;
      }
      else
      {
        mnd=&Doxygen::functionNameSDict;
      }
      MemberName *mn=mnd->find(resName);
      if (mn)
      {
        MemberNameIterator mni(*mn);
        MemberDef *tmd=0;
        for (;(tmd=mni.current());++mni)
        {
          //printf("Found member %s resScope=%s outerScope=%s mContext=%p\n",
          //    tmd->name().data(), resScope->name().data(), 
          //    tmd->getOuterScope()->name().data(), mContext);
          if (tmd->isTypedef() /*&& tmd->getOuterScope()==resScope*/)
          {
            // look if resScope is visible within tmd->getOuterScope()
            Definition *d = tmd->getOuterScope();
            while (d && d!=resScope) d=d->getOuterScope();
            if (d)
            {
              md=tmd;
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
    //printf(">>resolveTypeDef: Found typedef name `%s' in scope `%s' value=`%s'\n",
    //    qualifiedName.data(),context->name().data(),md->typeString()
    //    );
    result=md->typeString();
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
ClassDef *getClass(const char *name)
{
  if (name==0 || name[0]=='\0') return 0;
  return Doxygen::classSDict.find(name);
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
      warn_cont("Warning: possible recursive namespace alias detected for %s!\n",name);
    }
    return Doxygen::namespaceSDict[subst->data()];
  }
  else
  {
    return Doxygen::namespaceSDict[name];
  }
}

static QDict<MemberDef> g_resolvedTypedefs;

// forward declaration
ClassDef *getResolvedClassRec(Definition *scope,
                              FileDef *fileScope,
                              const char *n,
                              MemberDef **pTypeDef,
                              QCString *pTemplSpec
                             );
int isAccessibleFrom(Definition *scope,FileDef *fileScope,Definition *item,
                     const QCString &explicitScopePart);

/*! Returns the class representing the value of the typedef represented by \a md
 *  within file \a fileScope.
 *
 *  Example: typedef A T; will return the class representing A if it is a class.
 * 
 *  Example: typedef int T; will return 0, since "int" is not a class.
 */
ClassDef *newResolveTypedef(FileDef *fileScope,MemberDef *md,QCString *pTemplSpec)
{
  bool isCached = md->isTypedefValCached(); // value already cached
  if (isCached)
  {
    return md->getCachedTypedefVal();
  }
  QCString qname = md->qualifiedName();
  if (g_resolvedTypedefs.find(qname)) return 0; // typedef already done

  g_resolvedTypedefs.insert(qname,md); // put on the trace list
  
  QCString type = md->typeString(); // get the "value" of the typedef
  int ip=type.length()-1; // remove * and & at the end
  while (ip>=0 && (type.at(ip)=='*' || type.at(ip)=='&' || type.at(ip)==' ')) 
  {
    ip--;
  }
  type=type.left(ip+1);
  if (type.left(6)=="const ") // strip leading "const"
  {
    type=type.mid(6);
  }
  if (type.left(7)=="struct ") // strip leading "struct"
  {
  }
  else if (type.left(6)=="union ") // or strip leading "union"
  {
  }
  type=type.stripWhiteSpace(); // strip leading and trailing whitespace
  ClassDef *result = getResolvedClassRec(md->getOuterScope(),fileScope,type,0,0);
  if (result==0)
  {
    // try unspecialized version if type is template
    int i=type.find('<');
    if (i!=-1) // typedef of a template => try the unspecialized version
    {
      *pTemplSpec = type.mid(i);
      result = getResolvedClassRec(md->getOuterScope(),fileScope,type.left(i),0,0);
    }
  }

  // remember computed value for next time
  md->cacheTypedefVal(result);
  
  g_resolvedTypedefs.remove(qname); // remove from the trace list
  
  return result;
}

/*! Substitutes a simple unqualified \a name within \a scope. Returns the
 *  value of the typedef or \a name if no typedef was found.
 */
QCString substTypedef(Definition *scope,FileDef *fileScope,const QCString &name)
{
  QCString result=name;
  if (name.isEmpty()) return result;

  // lookup scope fragment in the symbol map
  DefinitionList *dl = Doxygen::symbolMap->find(name);
  if (dl==0) return result; // no matches

  // search for the best match
  DefinitionListIterator dli(*dl);
  Definition *d;
  int minDistance=10000; // init at "infinite"
  MemberDef *bestMatch=0;
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
        int distance = isAccessibleFrom(scope,fileScope,d,"");
        if (distance!=-1 && distance<minDistance) 
                   // definition is accessible and a better match
        {
          minDistance=distance;
          bestMatch = md; 
        }
      }
    }
  }
  if (bestMatch) result = bestMatch->typeString();
  //printf("substTypedef(%s,%s)=%s\n",scope?scope->name().data():"<global>",
  //                                  name.data(),result.data());
  return result;
}

/*! Starting with scope \a start, the string \a path is interpreted as
 *  a part of a qualified scope name (e.g. A::B::C), and the scope is 
 *  searched. If found the scope definition is returned, otherwise 0 
 *  is returned.
 */
static Definition *followPath(Definition *start,FileDef *fileScope,const QCString &path)
{
  int is,ps=0;
  int l;
  Definition *current=start;
  // for each part of the explicit scope
  while ((is=getScopeFragment(path,ps,&l))!=-1)
  {
    // try to resolve the part if it is a typedef
    QCString qualScopePart = substTypedef(current,fileScope,path.mid(is,l));
    current = current->findInnerCompound(qualScopePart);
    if (current==0) break; // failed to follow the path
    ps=is+l;
  }
  return current; // path could be followed
}

bool accessibleViaUsingClass(const ClassSDict *cl,
                             FileDef *fileScope,
                             Definition *item,
                             const QCString &explicitScopePart="")
{
  if (cl) // see if the class was imported via a using statement 
  {
    ClassSDict::Iterator cli(*cl);
    ClassDef *ucd;
    for (cli.toFirst();(ucd=cli.current());++cli)
    {
      Definition *sc = explicitScopePart.isEmpty() ? ucd : followPath(ucd,fileScope,explicitScopePart);
      if (item->definitionType()==Definition::TypeMember)
      {
        MemberDef *md = (MemberDef *)item;
        if (md->isTypedef()) // d is a typedef
        {
          QCString spec;
          ClassDef *typedefClass = newResolveTypedef(fileScope,md,&spec);
          if (sc && sc==typedefClass) return TRUE;
        }
      }
      else // item is a class
      {
        if (sc && sc==item) return TRUE; 
      }
    }
  }
  return FALSE;
}

bool accessibleViaUsingNamespace(const NamespaceSDict *nl,
                                 FileDef *fileScope,
                                 Definition *item,
                                 const QCString &explicitScopePart="")
{
  if (nl) // check used namespaces for the class
  {
    NamespaceSDict::Iterator nli(*nl);
    NamespaceDef *und;
    for (nli.toFirst();(und=nli.current());++nli)
    {
      Definition *sc = explicitScopePart.isEmpty() ? und : followPath(und,fileScope,explicitScopePart);
      if (sc && item->getOuterScope()==sc) return TRUE; 
    }
  }
  return FALSE;
}

/* Returns the "distance" (=number of levels up) from item to scope, or -1
 * if item in not inside scope. 
 */
int isAccessibleFrom(Definition *scope,FileDef *fileScope,Definition *item)
{
  //printf("<isAccesibleFrom(%s,%s)\n",scope?scope->name().data():"<global>",
  //                                      item?item->name().data():"<none>");
  if (item->getOuterScope()==scope) 
  {
    //printf("> found it\n");
    return 0; // found it
  }
  else if (scope==Doxygen::globalScope)
  {
    if (fileScope)
    {
      ClassSDict *cl = fileScope->getUsedClasses();
      if (accessibleViaUsingClass(cl,fileScope,item)) return 0;
      NamespaceSDict *nl = fileScope->getUsedNamespaces();
      if (accessibleViaUsingNamespace(nl,fileScope,item)) return 0;
    }
    return -1; // not found in path to globalScope
    //printf("> reached global scope\n");
  }
  else // keep searching
  {
    // check if scope is a namespace, which is using other classes and namespaces
    if (scope->definitionType()==Definition::TypeNamespace)
    {
      NamespaceDef *nscope = (NamespaceDef*)scope;
      ClassSDict *cl = nscope->getUsedClasses();
      if (accessibleViaUsingClass(cl,fileScope,item)) return 0;
      NamespaceSDict *nl = nscope->getUsedNamespaces();
      if (accessibleViaUsingNamespace(nl,fileScope,item)) return 0;
    }

    // repeat for the parent scope
    int i=isAccessibleFrom(scope->getOuterScope(),fileScope,item);
    //printf("> continue\n");
    return (i==-1) ? -1 : i+1;
  }
}


/* Returns the "distance" (=number of levels up) from item to scope, or -1
 * if item in not in this scope. The explicitScopePart limits the search
 * to scopes that match \a scope plus the explicit part.
 */
int isAccessibleFrom(Definition *scope,FileDef *fileScope,Definition *item,
                     const QCString &explicitScopePart)
{
  if (explicitScopePart.isEmpty())
  {
    // handle degenerate case where there is no explicit scope.
    return isAccessibleFrom(scope,fileScope,item);
  }
  //printf("<isAccesibleFrom(%s,%s,%s)\n",scope?scope->name().data():"<global>",
  //                                      item?item->name().data():"<none>",
  //                                      explicitScopePart.data());
  Definition *newScope = followPath(scope,fileScope,explicitScopePart);
  if (newScope)  // explicitScope is inside scope => newScope is the result
  {
    //printf("scope traversal successful!\n");
    if (item->getOuterScope()==newScope) 
    {
      //printf("> found it\n");
      return 0; // found it
    }
    else 
    {
      // repeat for the parent scope
      int i=-1;
      if (scope!=Doxygen::globalScope)
      {
        i = isAccessibleFrom(scope->getOuterScope(),fileScope,item,explicitScopePart);
      }
      //printf("> continue\n");
      return (i==-1) ? -1 : i+1;
    }
  }
  else // failed to resolve explicitScope
  {
    //printf("failed to resolve: scope=%s\n",scope->name().data());
    if (scope->definitionType()==Definition::TypeNamespace)
    {
      NamespaceDef *nscope = (NamespaceDef*)scope;
      ClassSDict *cl = nscope->getUsedClasses();
      if (accessibleViaUsingClass(cl,fileScope,item,explicitScopePart)) return 0;
      NamespaceSDict *nl = nscope->getUsedNamespaces();
      if (accessibleViaUsingNamespace(nl,fileScope,item,explicitScopePart)) return 0;
    }
    if (scope==Doxygen::globalScope)
    {
      if (fileScope)
      {
        ClassSDict *cl = fileScope->getUsedClasses();
        if (accessibleViaUsingClass(cl,fileScope,item,explicitScopePart)) return 0;
        NamespaceSDict *nl = fileScope->getUsedNamespaces();
        if (accessibleViaUsingNamespace(nl,fileScope,item,explicitScopePart)) return 0;
      }
      //printf("> reached global scope\n");
      return -1;
    }
    else // continue by looking into the parent scope
    {
      int i=isAccessibleFrom(scope->getOuterScope(),fileScope,item,explicitScopePart);
      //printf("> continue\n");
      return (i==-1) ? -1 : i+1;
    }
  }
}

/* Find the fully qualified class name refered to by the input class
 * or typedef name against the input scope.
 * Loops through scope and each of its parent scopes looking for a
 * match against the input name. Can recursively call itself when 
 * resolving typedefs.
 */
ClassDef *getResolvedClassRec(Definition *scope,
                              FileDef *fileScope,
                              const char *n,
                              MemberDef **pTypeDef,
                              QCString *pTemplSpec
                             )
{
  //printf("[getResolvedClassRec(%s,%s)\n",scope?scope->name().data():"<global>",n);
  QCString name=n;
  QCString explicitScopePart;

  int qualifierIndex = name.findRev("::"); // todo: deal with cases like A<B::C>
  if (qualifierIndex!=-1) // qualified name
  {
    // split off the explicit scope part
    explicitScopePart=name.left(qualifierIndex);
    // todo: improve namespace alias substitution
    replaceNamespaceAliases(explicitScopePart,explicitScopePart.length());
    name=name.mid(qualifierIndex+2);
  }
  if (name.isEmpty()) return 0; // empty name

  DefinitionList *dl = Doxygen::symbolMap->find(name);
  //printf("Looking for symbol %s result=%p\n",name.data(),dl);
  if (dl==0) return 0; // symbol not found

  //printf(" found %d symbol with name %s\n",dl->count(),name.data());
  // now we look int the list of Definitions and determine which one is the "best"
  DefinitionListIterator dli(*dl);
  Definition *d;
  ClassDef *bestMatch=0;
  MemberDef *bestTypedef=0;
  QCString bestTemplSpec;
  int minDistance=10000; // init at "infinite"
  for (dli.toFirst();(d=dli.current());++dli) // foreach definition
  {
    //printf("  found type %x name=%s\n",d->definitionType(),d->name().data());
    // only look at classes and members
    if (d->definitionType()==Definition::TypeClass ||
        d->definitionType()==Definition::TypeMember)
    {
      // test accessibility of definition within scope.
      int distance = isAccessibleFrom(scope,fileScope,d,explicitScopePart);
      if (distance!=-1) // definition is accessible
      {
        // see if we are dealing with a class or a typedef
        if (d->definitionType()==Definition::TypeClass) // d is a class
        {
          if (distance<minDistance) // found a definition that is "closer"
          {
            minDistance=distance;
            bestMatch = (ClassDef *)d; 
            bestTypedef = 0;
            bestTemplSpec.resize(0);
          }
        }
        else if (d->definitionType()==Definition::TypeMember)
        {
          MemberDef *md = (MemberDef *)d;
          if (md->isTypedef()) // d is a typedef
          {
            //printf("found typedef!\n");
            QCString spec;
            ClassDef *typedefClass = newResolveTypedef(fileScope,md,&spec);

            // we found a symbol at this distance, but if it didn't
            // resolve to a class, we still have to make sure that
            // something at a greater distance does not match, since
            // that symbol is hidden by this one.
            if (distance<minDistance)
            {
              minDistance=distance;
              bestMatch = typedefClass; 
              //printf("bestTypeDef=%p\n",md);
              bestTypedef = md;
              bestTemplSpec = spec;
            }

          }
        }
      } // if definition accessible
    } // if definition is a class or member
  } // foreach definition
  if (pTypeDef) 
  {
    *pTypeDef = bestTypedef;
  }
  if (pTemplSpec)
  {
    *pTemplSpec = bestTemplSpec;
  }
  //printf("] bestMatch=%s\n",bestMatch?bestMatch->name().data():"<none>");
  return bestMatch;
}

/* Find the fully qualified class name refered to by the input class
 * or typedef name against the input scope.
 * Loops through scope and each of its parent scopes looking for a
 * match against the input name. 
 */
ClassDef *getResolvedClass(Definition *scope,
                           FileDef *fileScope,
                           const char *n,
                           MemberDef **pTypeDef,
                           QCString *pTemplSpec
                          )
{
  g_resolvedTypedefs.clear();
  if (scope==0 ||
      (scope->definitionType()!=Definition::TypeClass && 
       scope->definitionType()!=Definition::TypeNamespace
      )
     )
  {
    scope=Doxygen::globalScope;
  }
  ClassDef *result = getResolvedClassRec(scope,fileScope,n,pTypeDef,pTemplSpec);
  if (result && !result->isLinkable()) result=0; // don't link to artifical classes
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
  while (b<i) // check if there are only spaces inbetween 
              // the operator and the >
  {
    if (!isspace(s.at(b))) return FALSE;
    b++;
  }
  return TRUE;
}

static const char constScope[] = { 'c', 'o', 'n', 's', 't', ':' };

QCString removeRedundantWhiteSpace(const QCString &s)
{
  if (s.isEmpty()) return s;
  QCString result;
  uint i;
  uint l=s.length();
  uint csp=0;
  for (i=0;i<l;i++)
  {
    char c=s.at(i);
    if (csp<6 && c==constScope[csp]) csp++; else csp=0;
    if (i<l-2 && c=='<' &&  // current char is a <
        (isId(s.at(i+1)) || isspace(s.at(i+1))) && // next char is an id char or space
        (i<8 || !findOperator(s,i)) // string in front is not "operator"
       )
    {
      result+="< "; // insert extra space for layouting (nested) templates
    }
    else if (i>0 && c=='>' && // current char is a >
             (isId(s.at(i-1)) || isspace(s.at(i-1)) || s.at(i-1)=='*' || s.at(i-1)=='&') && // prev char is an id char or space
             (i<8 || !findOperator(s,i)) // string in front is not "operator"
            )
    {
      result+=" >"; // insert extra space for layouting (nested) templates
    }
    else if (i>0 && c==',' && !isspace(s.at(i-1))
             && ((i<l-1 && isId(s.at(i+1)))
                 || (i<l-2 && s.at(i+1)=='$' && isId(s.at(i+2)))  // for PHP
                 || (i<l-3 && s.at(i+1)=='&' && s.at(i+2)=='$' && isId(s.at(i+3)))))  // for PHP
    {
      result+=", ";
    }
    else if (i>0 && 
              ((isId(s.at(i)) && s.at(i-1)==')') || 
               (s.at(i)=='\''  && s.at(i-1)==' ')
              )
            )
    {
      result+=' ';
      result+=s.at(i);
    }
    else if (c=='t' && csp==5 && !(isId(s.at(i+1)) || s.at(i+1)==' ')) // prevent const ::A from being converted to const::A
    {
      result+="t ";
      csp=0;
    }
    else if (c==':' && csp==6) // replace const::A by const ::A
    {
      result+=" :";
      csp=0;
    }
    else if (!isspace(c) ||
	      ( i>0 && i<l-1 && 
                (isId(s.at(i-1)) || s.at(i-1)==')' || s.at(i-1)==',' || s.at(i-1)=='>' || s.at(i-1)==']')
                 && (isId(s.at(i+1)) || (i<l-2 && s.at(i+1)=='$' && isId(s.at(i+2)))
                     || (i<l-3 && s.at(i+1)=='&' && s.at(i+2)=='$' && isId(s.at(i+3))))
              ) 
            )
    {
      if (c=='*' || c=='&' || c=='@' || c=='$')
      {  
        uint rl=result.length();
	if (rl>0 && (isId(result.at(rl-1)) || result.at(rl-1)=='>')) result+=' ';
      }
      result+=c;
    }
  }
  //printf("removeRedundantWhiteSpace(`%s')=`%s'\n",s.data(),result.data());
  return result;
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


void linkifyText(const TextGeneratorIntf &out,Definition *scope,FileDef *fileScope,const char * /*name*/,const char *text,bool autoBreak,bool external)
{
  //printf("`%s'\n",text);
  static QRegExp regExp("[a-z_A-Z][a-z_A-Z0-9:]*");
  QCString txtStr=text;
  QCString scopeName;
  int strLen = txtStr.length();
  //printf("linkifyText scope=%s fileScope=%s strtxt=%s strlen=%d\n",
  //    scope?scope->name().data():"<none>",
  //    fileScope?fileScope->name().data():"<none>",
  //    txtStr.data(),strLen);
  int matchLen;
  int index=0;
  int newIndex;
  int skipIndex=0;
  int floatingIndex=0;
  if (strLen==0) return;
  // read a word from the text string
  while ((newIndex=regExp.match(txtStr,index,&matchLen))!=-1)
  {
    // add non-word part to the result
    floatingIndex+=newIndex-skipIndex;
    bool insideString=FALSE; 
    int i;
    for (i=index;i<newIndex;i++) 
    { 
      if (txtStr.at(i)=='"') insideString=!insideString; 
    }
    
    if (strLen>30 && floatingIndex>25 && autoBreak) // try to insert a split point
    {
      QCString splitText = txtStr.mid(skipIndex,newIndex-skipIndex);
      int splitLength = splitText.length();
      int i=splitText.find('<');
      if (i==-1) i=splitText.find(',');
      if (i==-1) i=splitText.find(' ');
      if (i!=-1) // add a link-break at i in case of Html output
      {
        out.writeString(splitText.left(i+1));
        out.writeBreak();
        out.writeString(splitText.right(splitLength-i-1));
      } 
      else
      {
        out.writeString(splitText); 
      }
      floatingIndex=splitLength-i-1;
    }
    else
    {
      //ol.docify(txtStr.mid(skipIndex,newIndex-skipIndex)); 
      out.writeString(txtStr.mid(skipIndex,newIndex-skipIndex)); 
    }
    // get word from string
    QCString word=txtStr.mid(newIndex,matchLen);
    bool found=FALSE;
    if (!insideString)
    {
      ClassDef     *cd=0;
      FileDef      *fd=0;
      MemberDef    *md=0;
      NamespaceDef *nd=0;
      GroupDef     *gd=0;

      //QCString searchName=name;
      //printf("word=`%s' scope=`%s'\n",
      //        word.data(),scope ? scope->name().data() : "<none>"
      //      );
//      Definition *curScope = scope;
      // check if `word' is a documented class name
      //int scopeOffset=scopeName.length();
//      do // for each scope (starting with full scope and going to empty scope)
//      {
        //printf("Searching %s in %s...\n",word.data(),curScope?curScope->name().data():"<global>");
//        QCString fullName = word;
//        QCString prefix;
//        replaceNamespaceAliases(fullName,fullName.length());
//        //if (scopeOffset>0)
//        if (curScope && curScope!=Doxygen::globalScope)
//        {
//          prefix = curScope->name();
//          replaceNamespaceAliases(prefix,prefix.length());
//          fullName.prepend(prefix+"::");
//        }

        MemberDef *typeDef=0;
        if ((cd=getResolvedClass(scope,fileScope,word,&typeDef))) 
        {
          // add link to the result
          if (external ? cd->isLinkable() : cd->isLinkableInProject())
          {
            out.writeLink(cd->getReference(),cd->getOutputFileBase(),0,word);
            found=TRUE;
          }
        }
        else if (typeDef)
        {
          if (external ? typeDef->isLinkable() : typeDef->isLinkableInProject())
          {
            out.writeLink(typeDef->getReference(),
                          typeDef->getOutputFileBase(),
                          typeDef->anchor(),
                          word);
            found=TRUE;
          }
        }

//        if (curScope) curScope = curScope->getOuterScope();
//      } //while (!found && scopeOffset>=0);
//      while (!found && curScope);

//endloop:      
      if (scope && 
          (scope->definitionType()==Definition::TypeClass || 
           scope->definitionType()==Definition::TypeNamespace
          ) 
         )
      {
        scopeName=scope->name();
      }
      //if (!found) printf("Trying to link %s in %s\n",word.data(),scName); 
      if (!found && 
          getDefs(scopeName,word,0,md,cd,fd,nd,gd) && 
          (md->isTypedef() || md->isEnumerate() || 
           md->isReference() || md->isVariable()
          ) && 
          (external ? md->isLinkable() : md->isLinkableInProject()) 
         )
      {
        //printf("Found ref scope=%s\n",d?d->name().data():"<global>");
        if ((external ? md->isLinkable() : md->isLinkableInProject()))
        {
          //ol.writeObjectLink(d->getReference(),d->getOutputFileBase(),
          //                       md->anchor(),word);
          out.writeLink(md->getReference(),md->getOutputFileBase(),
              md->anchor(),word);
          found=TRUE;
        }
      }
    }

    if (!found) // add word to the result
    {
      //ol.docify(word);
      out.writeString(word);
    }
    // set next start point in the string
    //printf("index=%d/%d\n",index,txtStr.length());
    skipIndex=index=newIndex+matchLen;
    floatingIndex+=matchLen;
  }
  // add last part of the string to the result.
  //ol.docify(txtStr.right(txtStr.length()-skipIndex));
  out.writeString(txtStr.right(txtStr.length()-skipIndex));
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


QCString argListToString(ArgumentList *al)
{
  QCString result;
  if (al==0) return result;
  Argument *a=al->first();
  result+="(";
  while (a)
  {
    if (!a->name.isEmpty() || !a->array.isEmpty())
    {
      result+= a->type+" "+a->name+a->array;
    }
    else
    {
      result+= a->type;
    }
    if (!a->defval.isEmpty())
    {
      result+="="+a->defval;
    }
    a = al->next();
    if (a) result+=", "; 
  }
  result+=")";
  if (al->constSpecifier) result+=" const";
  if (al->volatileSpecifier) result+=" volatile";
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

/*! takes the \a buf of the given lenght \a len and converts CR LF (DOS)
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
    buf[dest++] = c;           // copy the (modified) character to dest
  }
  return dest;                 // length of the valid part of the buf
}

/*! reads a file with name \a name and returns it as a string. If \a filter
 *  is TRUE the file will be filtered by any user specified input filter.
 *  If \a name is "-" the string will be read from standard input. 
 */
QCString fileToString(const char *name,bool filter)
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
      err("Error: file `%s' not found\n",name);
      return "";
    }
    if (Config_getString("INPUT_FILTER").isEmpty() || !filter)
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
        return contents;
      }
    }
    else // filter the input
    {
      QCString cmd=Config_getString("INPUT_FILTER")+" \""+name+"\"";
      FILE *f=popen(cmd,"r");
      if (!f)
      {
        err("Error: could not execute filter %s\n",Config_getString("INPUT_FILTER").data());
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
      pclose(f);
      return contents;
    }
  }
  if (!fileOpened)  
  {
    err("Error: cannot open file `%s' for reading\n",name);
  }
  return "";
}

QCString dateToString(bool includeTime)
{
  if (includeTime)
  {
    return convertToQCString(QDateTime::currentDateTime().toString());
  }
  else
  {
    const QDate &d=QDate::currentDate();
    QCString result;
    result.sprintf("%d %s %d",
        d.day(),
        convertToQCString(d.monthName(d.month())).data(),
        d.year());
    return result;
  }
  //QDate date=dt.date();
  //QTime time=dt.time();
  //QCString dtString;
  //dtString.sprintf("%02d:%02d, %04d/%02d/%02d",
  //    time.hour(),time.minute(),date.year(),date.month(),date.day());
  //return dtString;
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

int minClassDistance(ClassDef *cd,ClassDef *bcd,int level)
{
  if (cd==bcd) return level; 
  BaseClassListIterator bcli(*cd->baseClasses());
  int m=maxInheritanceDepth; 
  for ( ; bcli.current() ; ++bcli)
  {
    m=QMIN(minClassDistance(bcli.current()->classDef,bcd,level+1),m);
  }
  return m;
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
    
  return result;
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
    if (cd->baseClasses()->count()>0)
      trimBaseClassScope(cd->baseClasses(),s,level+1); 
  }
}

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
  int i;
  if (s=="const")    { s.resize(0); return; }
  if (s=="volatile") { s.resize(0); return; }

  // strip occurrences of const
  int constLen=5+1;
  i = s.find("const ");
  if (i==-1 && s.right(5)=="const") { i=s.length()-5;constLen=5; }
  while (i!=-1) 
  {
    // no & or * after the const
    int i1=s.find('*',i+constLen);
    int i2=s.find('&',i+constLen);
    if (i1==-1 && i2==-1)
    {
      s=s.left(i)+s.right(s.length()-i-constLen); 
      i-=constLen;
    }
    else if ((i1!=-1 && i<i1) || (i2!=-1 && i<i2)) // const before * or &
    {
      // move const to front
      s=(QCString)"const "+s.left(i)+s.right(s.length()-i-constLen);
    }
    i = s.find("const ",i+constLen);
  }

  // strip occurrences of volatile
  int volatileLen=8+1;
  i = s.find("volatile ");
  if (i==-1 && s.right(8)=="volatile") { i=s.length()-8;constLen=8; }
  while (i!=-1) 
  {
    // no & or * after the volatile
    int i1=s.find('*',i+volatileLen);
    int i2=s.find('&',i+volatileLen);
    if (i1==-1 && i2==-1)
    {
      s=s.left(i)+s.right(s.length()-i-volatileLen); 
      i-=volatileLen;
    }
    else if ((i1!=-1 && i<i1) || (i2!=-1 && i<i2)) // volatile before * or &
    {
      // move volatile to front
      s=(QCString)"volatile "+s.left(i)+s.right(s.length()-i-volatileLen);
    }
    i = s.find("volatile ",i+volatileLen);
  }
  //printf("%s\n",s.data());
}


// a bit of debug support for matchArguments
#define MATCH
#define NOMATCH
//#define MATCH printf("Match at line %d\n",__LINE__);
//#define NOMATCH printf("Nomatch at line %d\n",__LINE__);

static bool matchArgument(const Argument *srcA,const Argument *dstA,
                   const QCString &className,
                   const QCString &namespaceName,
                   NamespaceSDict *usingNamespaces,
                   ClassSDict *usingClasses)
{
  //printf("match argument start %s:%s <-> %s:%s using nsp=%p class=%p\n",
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
  if (srcAType.left(6)=="class ") srcAType=srcAType.right(srcAType.length()-6);
  if (dstAType.left(6)=="class ") dstAType=dstAType.right(dstAType.length()-6);
  
  // allow distingishing "const A" from "const B" even though 
  // from a syntactic point of view they would be two names of the same 
  // type "const". This is not fool prove ofcourse, but should at least 
  // catch the most common cases.
  if ((srcAType=="const" || srcAType=="volatile") && !srcA->name.isEmpty())
  {
    srcAType+=" ";
    srcAType+=srcA->name;
  } 
  if ((dstAType=="const" || dstAType=="volatile") && !dstA->name.isEmpty())
  {
    dstAType+=" ";
    dstAType+=dstA->name;
  }
  if (srcA->name=="const" || srcA->name=="volatile")
  {
    srcAType+=srcA->name;
  }
  if (dstA->name=="const" || dstA->name=="volatile")
  {
    dstAType+=dstA->name;
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

  //printf("srcA=%s:%s dstA=%s:%s\n",srcAType.data(),srcA->name.data(),
  //      dstAType.data(),dstA->name.data());

  if (srcA->array!=dstA->array) // nomatch for char[] against char
  {
    NOMATCH
    return FALSE;
  }
  if (srcAType!=dstAType) // check if the argument only differs on name 
  {

    // remove a namespace scope that is only in one type 
    // (assuming a using statement was used)
    trimNamespaceScope(srcAType,dstAType,namespaceName);
  
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
      if (cd && cd->baseClasses()->count()>0)
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
    if (usingClasses && usingClasses->count()>0)
    {
      ClassSDict::Iterator cli(*usingClasses);
      ClassDef *cd;
      for (;(cd=cli.current());++cli)
      {
        srcAType=trimScope(cd->name(),srcAType);
        dstAType=trimScope(cd->name(),dstAType);
      }
    }

    //printf("2. srcA=%s:%s dstA=%s:%s\n",srcAType.data(),srcA->name.data(),
    //    dstAType.data(),dstA->name.data());
    
    if (!srcA->name.isEmpty() && !dstA->type.isEmpty() &&
        (srcAType+" "+srcA->name)==dstAType)
    {
      MATCH
      return TRUE;
    }
    else if (!dstA->name.isEmpty() && !srcA->type.isEmpty() &&
        (dstAType+" "+dstA->name)==srcAType)
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
        if (!srcA->name.isEmpty() || !dstA->name.isEmpty()) 
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
      if (!isspace(dstAType.at(dstPos))) // maybe the names differ
      {
        if (!dstA->name.isEmpty()) // dst has its name separated from its type
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
        if (dstPos!=dstAType.length() || !srcA->name.isEmpty()) 
        {
          NOMATCH
          return FALSE; // nope not a name -> no match
        }
      }
    }
    else if (srcPos<srcAType.length())
    {
      if (!isspace(srcAType.at(srcPos))) // maybe the names differ
      {
        if (!srcA->name.isEmpty()) // src has its name separated from its type
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
        if (srcPos!=srcAType.length() || !dstA->name.isEmpty()) 
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
                    ClassSDict *usingClasses)
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
    if (srcA->type==dstA->type)
    {
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
        if (forceNameOverwrite)
        {
          srcA->name = dstA->name.copy();
        }
        else
        {
          if (srcA->docs.isEmpty() && !dstA->docs.isEmpty())
          {
            srcA->name = dstA->name.copy();
          }
          else if (!srcA->docs.isEmpty() && dstA->docs.isEmpty())
          {
            dstA->name = srcA->name.copy();
          }
        }
      }
    }
    else
    {
      //printf("merging %s:%s <-> %s:%s\n",srcA->type.data(),srcA->name.data(),dstA->type.data(),dstA->name.data());
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
  }
  //printf("result mergeArguments `%s', `%s'\n",
  //    argListToString(srcAl).data(),argListToString(dstAl).data());
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
bool getDefs(const QCString &scName,const QCString &memberName, 
             const char *args,
             MemberDef *&md, 
             ClassDef *&cd, FileDef *&fd, NamespaceDef *&nd, GroupDef *&gd,
             bool forceEmptyScope,
             FileDef *currentFile,
             bool checkCV
            )
{
  fd=0, md=0, cd=0, nd=0, gd=0;
  if (memberName.isEmpty()) return FALSE; /* empty name => nothing to link */

  QCString scopeName=scName.copy();
  //printf("Search for name=%s args=%s in scope=%s\n",
  //          memberName.data(),args,scopeName.data());
  
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
      (im=memberName.findRev("::"))!=-1
     )
  {
    mScope=memberName.left(im); 
    mName=memberName.right(memberName.length()-im-2);
  }
  
  // handle special the case where both scope name and member scope are equal
  if (mScope==scopeName) scopeName.resize(0);

  //printf("mScope=`%s' mName=`%s'\n",mScope.data(),mName.data());
  
  MemberName *mn = Doxygen::memberNameSDict[mName];
  if (!forceEmptyScope && mn && !(scopeName.isEmpty() && mScope.isEmpty()))
  {
    //printf("  >member name found\n");
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
        className=mScope.copy();
      }
      //printf("Trying class scope %s\n",className.data());

      ClassDef *fcd=0;
      // todo: fill in correct fileScope!
      if ((fcd=getResolvedClass(Doxygen::globalScope,0,className)) &&  // is it a documented class
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
          if (mmd->isLinkable())
          {
            bool match=args==0 || 
              matchArguments(mmd->argumentList(),argList,className,0,checkCV); 
            //printf("match=%d\n",match);
            if (match)
            {
              ClassDef *mcd=mmd->getClassDef();
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
        if (argList)
        {
          delete argList; argList=0;
        }
        if (mdist==maxInheritanceDepth && !strcmp(args,"()"))
          // no exact match found, but if args="()" an arbitrary member will do
        {
          //printf("  >Searching for arbitrary member\n");
          for (mmli.toFirst();(mmd=mmli.current());++mmli)
          {
            if (//(mmd->protection()!=Private || Config_getBool("EXTRACT_PRIVATE")) &&
                //(
                //mmd->hasDocumentation() 
                /*mmd->detailsAreVisible()*/
                //|| mmd->isReference()
                //)
                mmd->isLinkable()
               )
            {
              ClassDef *mcd=mmd->getClassDef();
              //printf("  >Class %s found\n",mcd->name().data());
              int m=minClassDistance(fcd,mcd);
              if (m<mdist && mcd->isLinkable())
              {
                //printf("Class distance %d\n",m);
                mdist=m;
                cd=mcd;
                md=mmd;
              }
            }
          }
        }
        //printf("  >Succes=%d\n",mdist<maxInheritanceDepth);
        if (mdist<maxInheritanceDepth) 
        {
          gd=md->getGroupDef();
          if (gd) cd=0;
          return TRUE; /* found match */
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
    
    // unknown or undocumented scope 
  }

  // maybe an namespace, file or group member ?
  //printf("Testing for global function scopeName=`%s' mScope=`%s' :: mName=`%s'\n",
  //              scopeName.data(),mScope.data(),mName.data());
  if ((mn=Doxygen::functionNameSDict[mName])) // name is known
  {
    //printf("  >function name found\n");
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
          (fnd=Doxygen::namespaceSDict[namespaceName]) &&
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
          if (mmd->getNamespaceDef()==fnd && mmd->isLinkable())
          { // namespace is found
            bool match=TRUE;
            ArgumentList *argList=0;
            if (args)
            {
              argList=new ArgumentList;
              stringToArgumentList(args,argList);
              match=matchArguments(mmd->argumentList(),argList,0,
                  namespaceName,checkCV); 
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
        if (!found && !strcmp(args,"()")) 
          // no exact match found, but if args="()" an arbitrary 
          // member will do
        {
          for (mmli.toFirst();((mmd=mmli.current()) && !found);++mmli)
          {
            if (mmd->getNamespaceDef()==fnd && mmd->isLinkable())
            {
              nd=fnd;
              md=mmd;
              found=TRUE;
            }
          }
        }
        if (found) 
        {
          gd=md->getGroupDef();
          if (gd && gd->isLinkable()) nd=0; else gd=0;
          return TRUE;
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
        
        //printf("  Function with global scope name `%s' args=`%s'\n",memberName.data(),args);
        MemberListIterator mli(*mn);
        for (mli.toFirst();(md=mli.current());++mli)
        {
          if (md->isLinkable())
          {
            fd=md->getFileDef();
            gd=md->getGroupDef();
            //printf("  md->name()=`%s' md->args=`%s' fd=%p gd=%p\n",
            //    md->name().data(),args,fd,gd);
            if (
                (gd && gd->isLinkable()) || (fd && fd->isLinkable()) 
               )
            {
              //printf("fd=%p gd=%p inGroup=`%d' args=`%s'\n",fd,gd,inGroup,args);
              bool match=TRUE;
              ArgumentList *argList=0;
              if (args && !md->isDefine())
              {
                argList=new ArgumentList;
                stringToArgumentList(args,argList);
                match=matchArguments(md->argumentList(),argList,0,0,checkCV); 
                delete argList; argList=0;
              }
              if (match) 
              {
                //printf("Found match!\n");
                members.append(md);
              }
            }
          }
        }
        if (members.count()!=1 && !strcmp(args,"()"))
        {
          // no exact match found, but if args="()" an arbitrary 
          // member will do
          md=mn->last();
          while (md && md->isLinkable())
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
        if (members.count()==1 || currentFile!=0)
        {
          md=members.first();
        }
        else if (members.count()>1)
        {
          //printf("Found more than one matching member!\n");
          // use some C scoping rules to determine the correct link
          // 1. member in current file
          // 2. non-static member in different file
          if (currentFile==0)
          {
            bool ambig;
            currentFile = findFileDef(Doxygen::inputNameDict,0/*namespaceName*/,ambig);
          }
          MemberDef *bmd = 0;
          for (md=members.first(); md; md=members.next())
          {
            if (md->getFileDef() == currentFile)
            {
              bmd = 0;
              break;
            }
            if (!(md->isStatic()) || Config_getBool("EXTRACT_STATIC")) bmd = md;     
          }
          if (bmd) md=bmd;
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
 * `scope'. 
 *
 * The parameter `docScope' is a string representing the name of the scope in 
 * which the `scope' string was found.
 *
 * The function returns TRUE if the scope is known and documented or
 * FALSE if it is not.
 * If TRUE is returned exactly one of the parameter `cd', `nd' 
 * will be non-zero:
 *   - if `cd' is non zero, the scope was a class pointed to by cd.
 *   - if `nd' is non zero, the scope was a namespace pointed to by nd.
 */
bool getScopeDefs(const char *docScope,const char *scope,
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
    
    if ((cd=getClass(fullName)) && cd->isLinkable())
    {
      return TRUE; // class link written => quit 
    }
    else if ((nd=Doxygen::namespaceSDict[fullName]) && nd->isLinkable())
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
  char *p=s.data();
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
                /* out */ MemberDef  **resMember
               )
{
  //printf("resolveRef(scName=%s,name=%s,inSeeBlock=%d)\n",scName,name,inSeeBlock);
  
  QCString tsName = name;
  bool memberScopeFirst = tsName.find('#')!=-1;
  QCString fullName = substitute(tsName,"#","::");
  fullName = removeRedundantWhiteSpace(substitute(fullName,".","::"));
  
  int scopePos=fullName.findRev("::");
  int bracePos=fullName.findRev('('); // reverse is needed for operator()(...)

  // default result values
  *resContext=0;
  *resMember=0;

  if (bracePos==-1) // simple name
  {
    ClassDef *cd=0;
    NamespaceDef *nd=0;

    if (!inSeeBlock && scopePos==-1 && isLowerCase(tsName))
    { // link to lower case only name => do not try to autolink 
      return FALSE;
    }

    //printf("scName=%s name=%s\n",scName,fullName.data());
    
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
    else if (scName==fullName || (!inSeeBlock && scopePos==-1)) // nothing to link => output plain text
    {
      return FALSE;
    }
    // continue search...
  }
  
  // extract scope
  QCString scopeStr=scName;

  //printf("scopeContext=%s scopeUser=%s\n",scopeContext.data(),scopeUser.data());

  // extract userscope+name
  int endNamePos=bracePos!=-1 ? bracePos : fullName.length();
  QCString nameStr=fullName.left(endNamePos);

  // extract arguments
  QCString argsStr;
  if (bracePos!=-1) argsStr=fullName.right(fullName.length()-bracePos);
  
  //printf("scope=`%s' name=`%s' arg=`%s'\n",
  //       scopeStr.data(),nameStr.data(),argsStr.data());
  
  // strip template specifier
  // TODO: match against the correct partial template instantiation 
  int templPos=nameStr.find('<');
  if (templPos!=-1 && nameStr.find("operator")==-1)
  {
    int endTemplPos=nameStr.findRev('>');
    nameStr=nameStr.left(templPos)+nameStr.right(nameStr.length()-endTemplPos-1);
  }

  MemberDef    *md = 0;
  ClassDef     *cd = 0;
  FileDef      *fd = 0;
  NamespaceDef *nd = 0;
  GroupDef     *gd = 0;

  // check if nameStr is a member or global.
  //printf("getDefs(scope=%s,name=%s,args=%s)\n",scopeStr.data(),nameStr.data(),argsStr.data());
  if (getDefs(scopeStr,nameStr,argsStr,
              md,cd,fd,nd,gd,
              scopePos==0 && !memberScopeFirst,
              0,
              TRUE
             )
     )
  {
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
  else if (inSeeBlock && !nameStr.isEmpty() && (gd=Doxygen::groupSDict[nameStr]))
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

  return FALSE;
}

QCString linkToText(const char *link,bool isFileName)
{
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
    if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
    {
      result=substitute(result,"::",".");
    }
  }
  return result;
}

/*!
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
 *    4) "#memberName      member variable, global variable or define
 *    5) ("ScopeName::")+"memberName()" 
 *    6) ("ScopeName::")+"memberName(...)" 
 *    7) ("ScopeName::")+"memberName" 
 * instead of :: the # symbol may also be used.
 */

bool generateRef(OutputDocInterface &od,const char *scName,
                 const char *name,bool inSeeBlock,const char *rt)
{
  //printf("generateRef(scName=%s,name=%s,rt=%s)\n",scName,name,rt);
  
  Definition *compound;
  MemberDef *md;

  // create default link text
  QCString linkText = linkToText(rt,FALSE);

  if (resolveRef(scName,name,inSeeBlock,&compound,&md))
  {
    if (md) // link to member
    {
      od.writeObjectLink(md->getReference(),
                         md->getOutputFileBase(),
                         md->anchor(),linkText);
      // generate the page reference (for LaTeX)
      if (md->isLinkableInProject())
      {
        writePageRef(od,md->getOutputFileBase(),md->anchor());
      }
    }
    else // link to compound
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
    }
    return TRUE;
  }
  else // no link possible
  {
    od.docify(linkText);
    return FALSE;
  }
}

bool resolveLink(/* in */ const char *scName,
                 /* in */ const char *lr,
                 /* in */ bool inSeeBlock,
                 /* out */ Definition **resContext,
                 /* out */ QCString &resAnchor
                )
{
  *resContext=0;
  
  QCString linkRef=lr;
  FileDef  *fd;
  GroupDef *gd;
  PageDef  *pd;
  ClassDef *cd;
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
  else if ((gd=Doxygen::groupSDict[linkRef])) // link to a group
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
  else if ((cd=getClass(linkRef)))
  {
    *resContext=cd;
    return TRUE;
  }
  else if ((nd=Doxygen::namespaceSDict.find(linkRef)))
  {
    *resContext=nd;
    return TRUE;
  }
  else // probably a member reference
  {
    MemberDef *md;
    bool res = resolveRef(scName,lr,inSeeBlock,resContext,&md);
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
  QCString anchor,linkText=linkToText(lt,FALSE);
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
        linkText=linkToText(lt,TRUE); 
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
      err("%s:%d: Internal error: resolveLink successful but no compound found!\n",__FILE__,__LINE__);
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
// substitute all occurences of `src' in `s' by `dst'

QCString substitute(const char *s,const char *src,const char *dst)
{
  // TODO: optimize by using strstr() instead of find
  QCString input=s;
  QCString output;
  int i=0,p;
  while ((p=input.find(src,i))!=-1)
  {
    output+=input.mid(i,p-i);
    output+=dst;
    i=p+strlen(src);
  }
  output+=input.mid(i,input.length()-i);
  return output;
}

//----------------------------------------------------------------------

FileDef *findFileDef(const FileNameDict *fnDict,const char *n,bool &ambig)
{
  ambig=FALSE;
  QCString name=convertToQCString(QDir::cleanDirPath(n));
  QCString path;
  if (name.isEmpty()) return 0;
  int slashPos=QMAX(name.findRev('/'),name.findRev('\\'));
  if (slashPos!=-1)
  {
    path=name.left(slashPos+1);
    name=name.right(name.length()-slashPos-1); 
  }
  //printf("findFileDef path=`%s' name=`%s'\n",path.data(),name.data());
  if (name.isEmpty()) return 0;
  FileName *fn;
  if ((fn=(*fnDict)[name]))
  {
    if (fn->count()==1)
    {
      FileDef *fd = fn->getFirst();
      if (path.isEmpty() || fd->getPath().right(path.length())==path)
      {
        return fd;
      }
    }
    else // file name alone is ambigious
    {
      int count=0;
      FileNameIterator fni(*fn);
      FileDef *fd;
      FileDef *lastMatch=0;
      for (fni.toFirst();(fd=fni.current());++fni)
      {
        if (path.isEmpty() || fd->getPath().right(path.length())==path) 
        { 
          count++; 
          lastMatch=fd; 
        }
      }
      ambig=(count>1);
      return lastMatch;
    }
  }
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

QCString substituteKeywords(const QCString &s,const char *title)
{
  QCString result = s.copy();
  if (title) result = substitute(result,"$title",title);
  result = substitute(result,"$datetime",dateToString(TRUE));
  result = substitute(result,"$date",dateToString(FALSE));
  result = substitute(result,"$year",yearToString());
  result = substitute(result,"$doxygenversion",versionString);
  result = substitute(result,"$projectname",Config_getString("PROJECT_NAME"));
  result = substitute(result,"$projectnumber",Config_getString("PROJECT_NUMBER"));
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
  QStrList &sl = Config_getList("IGNORE_PREFIX");
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
  BaseClassListIterator bcli(*bcl);
  for ( ; bcli.current(); ++bcli)
  {
    ClassDef *cd=bcli.current()->classDef;
    if (cd->baseClasses()->count()==0) // no base classes => new root
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
  BaseClassListIterator bcli(*bcl);
  for ( ; bcli.current(); ++bcli)
  {
    ClassDef *cd=bcli.current()->classDef;
    if (cd->isVisibleInHierarchy()) return TRUE;
    hasVisibleRoot(cd->baseClasses());
  }
  return FALSE;
}

//----------------------------------------------------------------------

QCString escapeCharsInString(const char *name,bool allowDots)
{
  QCString result;
  char c;
  const char *p=name;
  while ((c=*p++)!=0)
  {
    switch(c)
    {
      case '_': result+="__"; break;
      case '-': result+="-";  break;
      case ':': result+="_1"; break;
      case '/': result+="_2"; break;
      case '<': result+="_3"; break;
      case '>': result+="_4"; break;
      case '*': result+="_5"; break;
      case '&': result+="_6"; break;
      case '|': result+="_7"; break;
      case '.': if (allowDots) result+="."; else result+="_8"; break;
      case '!': result+="_9"; break;
      case ',': result+="_00"; break;
      case ' ': result+="_01"; break;
      default: 
        if (Config_getBool("CASE_SENSE_NAMES") || !isupper(c))
        {
          result+=c;
        }
        else
        {
          result+="_";
          result+=tolower(c); 
        }
        break;
    }
  }
  return result;
}

/*! This function determines the file name on disk of an item
 *  given its name, which could be a class name with templete 
 *  arguments, so special characters need to be escaped.
 */
QCString convertNameToFile(const char *name,bool allowDots)
{
  if (Config_getBool("SHORT_NAMES"))
  {
    static QDict<void> usedNames(10007);
    static int count=1;

    void *value=usedNames.find(name);
    int num;
    if (value==0)
    {
      usedNames.insert(name,(void *)count);
      num = count++;
    }
    else
    {
      num = *(int*)&value;
    }
    QCString result;
    result.sprintf("a%05d",num); 
    return result;
  }
  else // long names
  {
    return escapeCharsInString(name,allowDots);
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

/*! Strips the scope from a name. Examples: A::B will return A
 *  and A<T>::B<N::C<D> > will return A<T>.
 *  \todo deal with cases like A< s<<2 >::B
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

/*! Convert nibble (range 0..15) to hex char */
static char nibbleToHex(int n)
{
  return (n < 10) ? ('0'+n) : ('a'+n-10);
}

/*! Converts a string to an XML-encoded string */
QCString convertToXML(const char *s)
{
  QCString result;
  if (s==0) return result;
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '<':  result+="&lt;";   break;
      case '>':  result+="&gt;";   break;
      case '&':  result+="&amp;";  break;
      case '\'': result+="&apos;"; break; 
      case '"':  result+="&quot;"; break;
      default:   
        if (c<0) 
        {
          result+=(QCString)"&#x" + 
                  nibbleToHex((((uchar)c)>>4)&0xf)+
                  nibbleToHex(c&0xf)+";";
        }
        else 
        {
          result+=c;        
        }
        break;
    }
  }
  return result;
}

/*! Converts a string to a HTML-encoded string */
QCString convertToHtml(const char *s)
{
  return convertToXML(s);
}

/*! Returns the standard string that is generated when the \\overload
 * command is used.
 */
const char *getOverloadDocs()
{
  return "This is an overloaded member function, "
         "provided for convenience. It differs from the above "
         "function only in what argument(s) it accepts.";
}
      
void addMembersToMemberGroup(MemberList *ml,
                             MemberGroupSDict *memberGroupSDict,
                             Definition *context)
{
  ASSERT(context!=0);
  //printf("addMemberToMemberGroup()\n");
  MemberListIterator mli(*ml);
  MemberDef *md;
  uint index;
  for (index=0;(md=mli.current());)
  {
    if (md->isEnumerate()) // insert enum value of this enum into groups
    {
      QList<MemberDef> *fmdl=md->enumFieldList();
      if (fmdl)
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
              MemberGroup *mg = memberGroupSDict->find(groupId);
              if (mg==0)
              {
                mg = new MemberGroup(
                       context,
                       groupId,
                       info->header,
                       info->doc,
                       info->docFile
                      );
                memberGroupSDict->append(groupId,mg);
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
        MemberGroup *mg = memberGroupSDict->find(groupId);
        if (mg==0)
        {
          mg = new MemberGroup(
                  context,
                  groupId,
                  info->header,
                  info->doc,
                  info->docFile
                 );
          memberGroupSDict->append(groupId,mg);
        }
        md = ml->take(index); // remove from member list
        mg->insertMember(md); // insert in member group
        md->setMemberGroup(mg);
        continue;
      }
    }
    ++mli;++index;
  }
}

/*! Extracts a (sub-)string from \a type starting at \a pos that
 *  could form a class. When TRUE is returned the result is the 
 *  class \a name and a template argument list \a templSpec.
 */
bool extractClassNameFromType(const QCString &type,int &pos,QCString &name,QCString &templSpec)
{
  static const QRegExp re("[a-z_A-Z][a-z_A-Z0-9:]*");
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
      if (te>ts) templSpec = type.mid(ts,te-ts),tl+=te-ts;
      name = type.mid(i,l);
      pos=i+l+tl;
      //printf("extractClassNameFromType([in] type=%s,[out] pos=%d,[out] name=%s,[out] templ=%s)=TRUE\n",
      //    type.data(),pos,name.data(),templSpec.data());
      return TRUE;
    }
  }
  //printf("extractClassNameFromType([in] type=%s,[out] pos=%d,[out] name=%s,[out] templ=%s)=FALSE\n",
  //       type.data(),pos,name.data(),templSpec.data());
  return FALSE;
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
  static QRegExp re("[a-z_A-Z][a-z_A-Z0-9]*");
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
      //printf("n=%s formArg->type=%s formArg->name=%s\n",
      //    n.data(),formArg->type.data(),formArg->name.data());
      if (formArg->type=="class" || formArg->type=="typename")
      {
        if (formArg->name==n && actArg && !actArg->type.isEmpty()) // base class is a template argument
        {
          // replace formal argument with the actual argument of the instance
          result += actArg->type+" "; 
          found=TRUE;
        }
        else if (formArg->name==n && actArg==0 && !formArg->defval.isEmpty() &&
                 formArg->defval!=name /* to prevent recursion */
                )
        {
          result += substituteTemplateArgumentsInString(formArg->defval,formalArgs,actualArgs)+" ";
          found=TRUE;
        }
      }
    }
    if (!found) result += n;
    p=i+l;
  }
  result+=name.right(name.length()-p);
  //printf("      Inheritance relation %s -> %s\n",
  //    name.data(),result.data());
  return result;
}


/*! Makes a deep copy of argument list \a src. Will allocate memory, that
 *  is owned by the caller. 
 */
ArgumentList *copyArgumentList(const ArgumentList *src)
{
  ASSERT(src!=0);
  ArgumentList *dst = new ArgumentList;
  dst->setAutoDelete(TRUE);
  ArgumentListIterator tali(*src);
  Argument *a;
  for (;(a=tali.current());++tali)
  {
    dst->append(new Argument(*a));
  }
  dst->constSpecifier    = src->constSpecifier;
  dst->volatileSpecifier = src->volatileSpecifier;
  dst->pureSpecifier     = src->pureSpecifier;
  return dst;
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
    dstLists->append(copyArgumentList(sl));
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
                                          bool parentOnly)
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
      //printf("2:result+=%s\n",fullName.mid(i,e-i-1).data());
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
                           TagInfo *tagInfo
                          )
{
  PageDef *pd=0;
  //printf("addRelatedPage(name=%s gd=%p)\n",name,gd);
  if ((pd=Doxygen::pageSDict->find(name)) && !tagInfo)
  {
    // append documentation block to the page.
    pd->setDocumentation(pd->documentation()+"\n\n"+doc,fileName,startLine);
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

    if (tagInfo)
    {
      pd->setReference(tagInfo->tagName);
    }

    QCString pageName;
    if (Config_getBool("CASE_SENSE_NAMES"))
      pageName=pd->name();
    else
      pageName=pd->name().lower();
    pd->setFileName(pageName);

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
      else if (pd->getGroupDef())
      {
        file=pd->getGroupDef()->getOutputFileBase().copy();
      }
      else
      {
        file=pageName;
      }
      SectionInfo *si=new SectionInfo(
          file,pd->name(),pd->title(),SectionInfo::Page,pd->getReference());
      //printf("si->label=`%s' si->definition=%s si->fileName=`%s'\n",
      //      si->label.data(),si->definition?si->definition->name().data():"<none>",
      //      si->fileName.data());
      //printf("  SectionInfo: sec=%p sec->fileName=%s\n",si,si->fileName.data());
      //printf("Adding section key=%s si->fileName=%s\n",pageName.data(),si->fileName.data());
      Doxygen::sectionDict.insert(pageName,si);
    }
  }
  return pd;
}

//----------------------------------------------------------------------------

void addRefItem(const QList<ListItemInfo> *sli,
                const char *prefix,
                const char *name,const char *title,const char *args)
{
  //printf("addRefItem(%s,%s,%s,%s)\n",prefix,name,title,args);
  if (sli)
  {
    QListIterator<ListItemInfo> slii(*sli);
    ListItemInfo *lii;
    for (slii.toFirst();(lii=slii.current());++slii)
    {
      RefList *refList = Doxygen::xrefLists->find(lii->type);
      if (refList && 
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
        if (item->written) return;
        //printf("anchor=%s\n",item->listAnchor.data());

        QCString doc(1000);
        doc =  "\\anchor ";
        doc += item->listAnchor;
        doc += " <dl><dt>";
        doc += prefix;
        doc += " \\_internalref ";
        doc += name;
        doc += " \"";
        doc += title;
        doc += "\"";
        if (args) doc += args;
        doc += "</dt>\n<dd>";
        doc += item->text;
        doc += "</dd></dl>\n";
        addRelatedPage(refList->listName(),refList->pageTitle(),doc,0,refList->listName(),1,0,0,0);
        item->written=TRUE;
      }
    }
  }
}

void addGroupListToTitle(OutputList &ol,Definition *d)
{
  if (d->partOfGroups()) // write list of group to which this definition belongs
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.lineBreak();
    ol.startSmall();
    ol.docify("[");
    GroupListIterator gli(*d->partOfGroups());
    GroupDef *gd;
    bool first=TRUE;
    for (gli.toFirst();(gd=gli.current());++gli)
    {
      if (!first) { ol.docify(","); ol.writeNonBreakableSpace(1); } else first=FALSE; 
      ol.writeObjectLink(gd->getReference(),
                         gd->getOutputFileBase(),0,gd->groupTitle());
    }
    ol.docify("]");
    ol.endSmall();
    ol.popGeneratorState();
  }
}

/*!
 * Function converts Latin1 character to latex string representin the same
 * character.
 */
static void latin1ToLatex(QTextStream &t,unsigned char c)
{
  switch (c)
  {
    // the Latin-1 characters
    case 161: t << "!`";            break;
    case 181: t << "$\\mu$";        break;
    case 191: t << "?`";            break;
    case 192: t << "\\`{A}";        break;
    case 193: t << "\\'{A}";        break;
    case 194: t << "\\^{A}";        break;
    case 195: t << "\\~{A}";        break;
    case 196: t << "\\\"{A}";       break;
    case 197: t << "\\AA{}";        break;
    case 198: t << "\\AE{}";        break;
    case 199: t << "\\c{C}";        break;
    case 200: t << "\\`{E}";        break;
    case 201: t << "\\'{E}";        break;
    case 202: t << "\\^{E}";        break;
    case 203: t << "\\\"{E}";       break;
    case 204: t << "\\`{I}";        break;
    case 205: t << "\\'{I}";        break;
    case 206: t << "\\^{I}";        break;
    case 207: t << "\\\"{I}";       break;
    case 208: t << "D ";            break; // anyone know the real code?
    case 209: t << "\\~{N}";        break;
    case 210: t << "\\`{O}";        break;
    case 211: t << "\\'{O}";        break;
    case 212: t << "\\^{O}";        break;
    case 213: t << "\\~{O}";        break;
    case 214: t << "\\\"{O}";       break;
    case 215: t << "$\\times$";     break;
    case 216: t << "\\O";           break;
    case 217: t << "\\`{U}";        break;
    case 218: t << "\\'{U}";        break;
    case 219: t << "\\^{U}";        break;
    case 220: t << "\\\"{U}";       break;
    case 221: t << "\\'{Y}";        break;
    case 223: t << "\\ss{}";        break; 
    case 224: t << "\\`{a}";        break;
    case 225: t << "\\'{a}";        break;
    case 226: t << "\\^{a}";        break;
    case 227: t << "\\~{a}";        break;
    case 228: t << "\\\"{a}";       break;
    case 229: t << "\\aa{}";        break;
    case 230: t << "\\ae{}";        break;
    case 231: t << "\\c{c}";        break;
    case 232: t << "\\`{e}";        break;
    case 233: t << "\\'{e}";        break;
    case 234: t << "\\^{e}";        break;
    case 235: t << "\\\"{e}";       break;
    case 236: t << "\\`{\\i}";      break;
    case 237: t << "\\'{\\i}";      break;
    case 238: t << "\\^{\\i}";      break;
    case 239: t << "\\\"{\\i}";     break;
    case 241: t << "\\~{n}";        break;
    case 242: t << "\\`{o}";        break;
    case 243: t << "\\'{o}";        break;
    case 244: t << "\\^{o}";        break;
    case 245: t << "\\~{o}";        break;
    case 246: t << "\\\"{o}";       break;
    case 248: t << "\\o{}";         break;
    case 249: t << "\\`{u}";        break;
    case 250: t << "\\'{u}";        break;
    case 251: t << "\\^{u}";        break;
    case 252: t << "\\\"{u}";       break;
    case 253: t << "\\'{y}";        break;
    case 255: t << "\\\"{y}";       break;           
    default: t << c;
  }
}

/*!
 * Function converts Latin2 character to latex string representin the same
 * character.
 */
static void latin2ToLatex(QTextStream &t,unsigned char c)
{
  switch (c)
  {
    case 0xA1: t << "\\k{A}";   break;
    case 0xA2: t << c;          break;
    case 0xA3: t << "\\L{}";    break;
    case 0xA4: t << c;          break;
    case 0xA5: t << c;          break;
    case 0xA6: t << "\\'{S}";   break;
    case 0xA7: t << c;          break;
    case 0xA8: t << c;          break;
    case 0xA9: t << "\\v{S}";   break;
    case 0xAA: t << "\\c{S}";   break;
    case 0xAB: t << "\\v{T}";   break;
    case 0xAC: t << "\\'{Z}";   break;
    case 0xAD: t << c;          break;
    case 0xAE: t << "\\v{Z}";   break;
    case 0xAF: t << "\\.{Z}";   break;

    case 0xB0: t << c;          break;
    case 0xB1: t << "\\k{a}";   break;
    case 0xB2: t << c;          break;
    case 0xB3: t << "\\l{}";    break;
    case 0xB4: t << c;          break;
    case 0xB5: t << c;          break;
    case 0xB6: t << "\\'{s}";   break;
    case 0xB7: t << c;          break;
    case 0xB8: t << c;          break;
    case 0xB9: t << "\\v{s}";   break;
    case 0xBA: t << "\\c{s}";   break;
    case 0xBB: t << "\\v{t}";   break;
    case 0xBC: t << "\\'{z}";   break;
    case 0xBD: t << c;          break;
    case 0xBE: t << "\\v{z}";   break;
    case 0xBF: t << "\\.{z}";   break;

    case 0xC0: t << "\\'{R}";   break;
    case 0xC1: t << "\\'{A}";   break;
    case 0xC2: t << "\\^{A}";   break;
    case 0xC3: t << "\\u{A}";   break;
    case 0xC4: t << "\\\"{A}";  break;
    case 0xC5: t << "\\'{L}";   break;
    case 0xC6: t << "\\'{C}";   break;
    case 0xC7: t << "\\c{C}";   break;
    case 0xC8: t << "\\v{C}";   break;
    case 0xC9: t << "\\'{E}";   break;
    case 0xCA: t << "\\k{E}";   break;
    case 0xCB: t << "\\\"{E}";  break;
    case 0xCC: t << "\\v{E}";   break;
    case 0xCD: t << "\\'{I}";   break;
    case 0xCE: t << "\\^{I}";   break;
    case 0xCF: t << "\\v{D}";   break;

    case 0xD0: t << "\\bar{D}"; break;
    case 0xD1: t << "\\'{N}";   break;
    case 0xD2: t << "\\v{N}";   break;
    case 0xD3: t << "\\'{O}";   break;
    case 0xD4: t << "\\^{O}";   break;
    case 0xD5: t << "\\H{O}";   break;
    case 0xD6: t << "\\\"{O}";  break;
    case 0xD7: t << c;          break;
    case 0xD8: t << "\\v{R}";   break;
    case 0xD9: t << c;          break;
    case 0xDA: t << "\\'{U}";   break;
    case 0xDB: t << "\\H{U}";   break;
    case 0xDC: t << "\\\"{U}";  break;
    case 0xDD: t << "\\'{Y}";   break;
    case 0xDE: t << "\\c{T}";   break;
    case 0xDF: t << "\\ss";     break;

    case 0xE0: t << "\\'{r}";   break;
    case 0xE1: t << "\\'{a}";   break;
    case 0xE2: t << "\\^{a}";   break;
    case 0xE3: t << c;          break;
    case 0xE4: t << "\\\"{a}";  break;
    case 0xE5: t << "\\'{l}";   break;
    case 0xE6: t << "\\'{c}";   break;
    case 0xE7: t << "\\c{c}";   break;
    case 0xE8: t << "\\v{c}";   break;
    case 0xE9: t << "\\'{e}";   break;
    case 0xEA: t << "\\k{e}";   break;
    case 0xEB: t << "\\\"{e}";  break;
    case 0xEC: t << "\\v{e}";   break;
    case 0xED: t << "\\'{\\i}"; break;
    case 0xEE: t << "\\^{\\i}"; break;
    case 0xEF: t << "\\v{d}";   break;

    case 0xF0: t << "\\bar{d}"; break;
    case 0xF1: t << "\\'{n}";   break;
    case 0xF2: t << "\\v{n}";   break;
    case 0xF3: t << "\\'{o}";   break;
    case 0xF4: t << "\\^{o}";   break;
    case 0xF5: t << "\\H{o}";   break;
    case 0xF6: t << "\\\"{o}";  break;
    case 0xF7: t << c;          break;
    case 0xF8: t << "\\v{r}";   break;
    case 0xF9: t << c;          break;
    case 0xFA: t << "\\'{u}";   break;
    case 0xFB: t << "\\H{u}";   break;
    case 0xFC: t << "\\\"{u}";  break;
    case 0xFD: t << "\\'{y}";   break;
    case 0xFE: t << c;          break;
    case 0xFF: t << c;          break;

    default: t << c;
  }
}

void filterLatexString(QTextStream &t,const char *str,
                       bool insideTabbing,bool insidePre,bool insideItem)
{
  static bool isCzech         = theTranslator->idLanguage()=="czech";
  static bool isJapanese      = theTranslator->idLanguage()=="japanese" ||
                                theTranslator->idLanguage()=="japanese-en";
  static bool isKorean        = theTranslator->idLanguage()=="korean";
  static bool isRussian       = theTranslator->idLanguage()=="russian";
  static bool isUkrainian     = theTranslator->idLanguage()=="ukrainian";
  static bool isChinese       = theTranslator->idLanguage()=="chinese" || 
                                theTranslator->idLanguage()=="chinese-traditional";
  static bool isLatin2        = theTranslator->idLanguageCharset()=="iso-8859-2";
  static bool isGreek         = theTranslator->idLanguage()=="greek";
  //printf("filterLatexString(%s)\n",str);
  if (str)
  {
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
            {
              // Some languages use wide characters
              if (c>=128 && (isJapanese || isKorean || isChinese))
              { 
                t << (char)c;
                if (*p)  
                {
                  c = *p++;
                  t << (char)c;
                }
              }
              else
              {
                t << (char)c; 
              }
              break;
            }
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
          case '_':  t << "\\_"; 
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
          case '-':  if (*p=='>') 
                       { t << " $\\rightarrow$ "; p++; }
                     else
                     { t << (char)c; }
                     break;
          case '\\': if (*p=='<') 
                     { t << "$<$"; p++; }
                     else if (*p=='>')
                     { t << "$>$"; p++; } 
                     else  
                     { t << "$\\backslash$"; }
                     break;           
          case '"':  { t << "\\char`\\\"{}"; }
                     break;

          default:   
            {
               // Some languages use wide characters
               if (isJapanese || isKorean || isChinese)
               { 
                 if (c>=128) 
                 {
                   t << (char)c;
                   if (*p)  
                   {
                     c = *p++;
                     t << (char)c;
                   }
                 }
                 else // ascii char => see if we can insert a hypenation hint
                 {
                   if (isupper(c) && islower(pc) && !insideTabbing) t << "\\-";
                   t << (char)c;    
                 } 
               }
               else if (isCzech || isRussian || isUkrainian)
               {
                 if (c>=128)
                 {
                   t << (char)c;
                 }
                 else // ascii char => see if we can insert a hypenation hint
                 {
                   if (isupper(c) && islower(pc) && !insideTabbing) t << "\\-";
                   t << (char)c;
                 }
               }
               else if (isGreek)
               {
                 if (c<128)
                 {
                   t << "\\textlatin{" << (char)c << "}";
                 }
                 else
                 {
                   t << (char)c;
                 }
               }
               else if (isLatin2)
               {
                 if (c>=128)
                 {
                   latin2ToLatex(t,c);
                 }
                 else
                 { 
                   // see if we can insert an hyphenation hint
                   if (isupper(c) && islower(pc) && !insideTabbing) t << "\\-";
                   t << (char)c;
                 }
               }
               else // another language => assume latin1 charset
               {
                 if (c>=128)
                 {
                   latin1ToLatex(t,c);
                 }
                 else
                 { 
                   // see if we can insert an hyphenation hint
                   if (isupper(c) && islower(pc) && !insideTabbing) t << "\\-";
                   t << (char)c;
                 }
               }
            }
        }
      }
      pc = c;
    }
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
  //printf("replaceNamespaceAliases(%s,%d)\n",scope.data(),i);
  while (i>0)
  {
    QCString *s = Doxygen::namespaceAliasDict[scope.left(i)];
    if (s)
    {
      scope=*s+scope.right(scope.length()-i);
      i=s->length();
    }
    i=scope.findRev("::",i-1);
  }
  //printf("replaceNamespaceAliases() result=%s\n",scope.data());
}

