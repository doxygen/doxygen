/*****************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

#include "util.h"
#include "message.h"
#include "classdef.h"
#include "filedef.h"
#include "doxygen.h"
#include "doc.h"
#include "outputlist.h"
#include "defargs.h"
#include "language.h"
#include "config.h"
#include "htmlhelp.h"
#include "example.h"
#include "version.h"
#include "groupdef.h"
#include "reflist.h"
#include "page.h"

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
#ifndef _WIN32
  isBatchFile=isBatchFile;
  /*! taken from the system() manpage on my Linux box */
  int pid,status;

  if (command==0) return 1;
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
      return status;
    }
  }
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
  return 0;
  //return system(command);
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
  result.sprintf("@%d\n",id);
  return result;
}

/*! strip part of \a path if it matches
 *  one of the paths in the Config_getList("STRIP_FROM_PATH") list
 */
QCString stripFromPath(const QCString &path)
{
  const char *s=Config_getList("STRIP_FROM_PATH").first();
  while (s)
  {
    QCString prefix = s;
    if (path.left(prefix.length())==prefix)
    {
      return path.right(path.length()-prefix.length());
    }
    s = Config_getList("STRIP_FROM_PATH").next();
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
      n.right(4)==".idl"
     ) return Entry::HEADER_SEC;
  return 0;
}

QCString resolveTypeDef(Definition *d,const QCString &name)
{
  //printf("resolveTypeDef(%s,%s)\n",d ? d->name().data() : "<none>",name.data());
  QCString result;
  if (name.isEmpty()) return result;

  Definition *mContext=d;
  MemberDef *md=0;
  while (mContext && md==0)
  {
    MemberNameSDict *mnd=0;
    if (mContext->definitionType()==Definition::TypeClass)
    {
      mnd=&Doxygen::memberNameSDict;
    }
    else
    {
      mnd=&Doxygen::functionNameSDict;
    }
    MemberName *mn=mnd->find(name);
    if (mn)
    {
      MemberNameIterator mni(*mn);
      MemberDef *tmd=0;
      for (;(tmd=mni.current());++mni)
      {
        //printf("Found member %s scope=%p mContext=%p\n",tmd->name().data(),
        //    tmd->getOuterScope(),mContext);
        if (tmd->isTypedef() && tmd->getOuterScope()==mContext)
        {
          md=tmd;
        }
      }
    }
    mContext=mContext->getOuterScope();
  }
  if (md)
  {
    //printf("Found typedef name `%s' in scope `%s' value=`%s'\n",
    //    name.data(),d->name().data(),md->typeString()
    //    );
    result=md->typeString();
  }
  else
  {
    //printf("Typedef `%s' not found in scope `%s'!\n",
    //    name.data(),d ? d->name().data() : "<global>");
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

ClassDef *getResolvedClass(
                           Definition *scope,
                           const char *n,
                           bool *pIsTypeDef,
                           QCString *pTemplSpec
                          )
{
  //printf("getResolvedClass(%s,%s)\n",scope ? scope->name().data() : "<none>", n);
  QCString name = n;
  if (name.isEmpty()) return 0;
  if (scope==0) scope=Doxygen::globalScope;
  int i = name.findRev("::");
  QCString subst;
  if (i!=-1)
  {
    subst = resolveTypeDef(scope,name.right(name.length()-i-2));
  }
  else
  {
    subst = resolveTypeDef(scope,name);
  }
  //printf("  typedef subst=`%s'\n",subst.data());
  
  if (!subst.isEmpty())
  {
    // strip * and & from n
    int ip=subst.length()-1;
    while (ip>=0 && (subst.at(ip)=='*' || subst.at(ip)=='&' || subst.at(ip)==' ')) ip--;
    subst=subst.left(ip+1);

    if (pIsTypeDef) *pIsTypeDef=TRUE;
    //printf("  getResolvedClass `%s'->`%s'\n",name.data(),subst.data());
    if (subst==name) // avoid resolving typedef struct foo foo; 
    {
      return Doxygen::classSDict.find(name);
    }
    int count=0; // recursion detection guard
    QCString newSubst;
    QCString typeName = subst;
  
    if (i!=-1) typeName.prepend(name.left(i)+"::");
    while (!(newSubst=resolveTypeDef(scope,typeName)).isEmpty() 
           && count<10)
    {
      if (typeName==newSubst) 
      {
        ClassDef *cd = Doxygen::classSDict.find(subst); // for breaking typedef struct A A; 
        //printf("  getClass: exit `%s' %p\n",subst.data(),cd);
        return cd;
      }
      subst=newSubst;
      // strip * and & from n
      int ip=subst.length()-1;
      while (subst.at(ip)=='*' || subst.at(ip)=='&' || subst.at(ip)==' ') ip--;
      subst=subst.left(ip+1);
      //printf("  getResolvedClass `%s'->`%s'\n",name.data(),subst.data());

      typeName=newSubst;
      if (i!=-1) typeName.prepend(name.left(i)+"::");
      count++;
    }
    if (count==10)
    {
      warn_cont("Warning: possible recursive typedef dependency detected for %s!\n",n);
      return Doxygen::classSDict.find(name);
    }
    else
    {
      int i;
      ClassDef *cd = Doxygen::classSDict.find(typeName);
      //printf("  getClass: subst %s->%s cd=%p\n",name.data(),typeName.data(),cd);
      if (cd==0 && (i=typeName.find('<'))>0) // try unspecialized version as well
      {
        if (pTemplSpec) *pTemplSpec = typeName.right(typeName.length()-i);
        return Doxygen::classSDict.find(typeName.left(i));
      }
      else
      {
        return cd;
      }
    }
  }
  else
  {
    if (pIsTypeDef) *pIsTypeDef=FALSE;
    return Doxygen::classSDict.find(name);
  }
}

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
             (isId(s.at(i-1)) || isspace(s.at(i-1))) && // prev char is an id char or space
             (i<8 || !findOperator(s,i)) // string in front is not "operator"
            )
    {
      result+=" >"; // insert extra space for layouting (nested) templates
    }
    else if (i>0 && i<l-1 && c==',' && !isspace(s.at(i-1)) && isId(s.at(i+1)))
    {
      result+=", ";
    }
    else if (i>0 && isId(s.at(i)) && s.at(i-1)==')')
    {
      result+=' ';
      result+=s.at(i);
    }
    else if (c==':' && csp==6)
    {
      result+=" :";
      csp=0;
    }
    else if (!isspace(c) ||
	      ( i!=0 && i!=l-1 && 
                (isId(s.at(i-1)) || s.at(i-1)==')' || s.at(i-1)==',' || s.at(i-1)=='>' || s.at(i-1)==']') && 
                isId(s.at(i+1))
              ) 
            )
    {
      if (c=='*' || c=='&' || c=='@')
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


void linkifyText(const TextGeneratorIntf &out,const char *scName,const char *name,const char *text,bool autoBreak,bool external)
{
  //printf("scope=`%s' name=`%s' Text: `%s'\n",scName,name,text);
  static QRegExp regExp("[a-z_A-Z][a-z_A-Z0-9:]*");
  QCString txtStr=text;
  int strLen = txtStr.length();
  //printf("linkifyText strtxt=%s strlen=%d\n",txtStr.data(),strLen);
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
    ClassDef     *cd=0;
    FileDef      *fd=0;
    MemberDef    *md=0;
    NamespaceDef *nd=0;
    GroupDef     *gd=0;

    QCString scopeName=scName;
    QCString searchName=name;
    //printf("word=`%s' scopeName=`%s' searchName=`%s'\n",
    //        word.data(),scopeName.data(),searchName.data()
    //      );
    // check if `word' is a documented class name
    if (!word.isEmpty() && 
        !rightScopeMatch(word,searchName) && 
        !rightScopeMatch(scopeName,word)
       )
    {
      //printf("Searching...\n");
      int scopeOffset=scopeName.length();
      bool found=FALSE;
      do // for each scope (starting with full scope and going to empty scope)
      {
        QCString fullName = word.copy();
        if (scopeOffset>0)
        {
          fullName.prepend(scopeName.left(scopeOffset)+"::");
        }
        //printf("Trying class %s\n",fullName.data());

        if ((cd=getClass(fullName)))
        {
          // add link to the result
          if (external ? cd->isLinkable() : cd->isLinkableInProject())
          {
            //ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,word);
            out.writeLink(cd->getReference(),cd->getOutputFileBase(),0,word);
            found=TRUE;
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
      } while (!found && scopeOffset>=0);

      //if (!found) printf("Trying to link %s in %s\n",word.data(),scName); 
      if (!found && 
          getDefs(scName,word,0,md,cd,fd,nd,gd) && 
          (md->isTypedef() || md->isEnumerate() || 
           md->isReference() || md->isVariable()
          ) && 
          (external ? md->isLinkable() : md->isLinkableInProject()) 
         )
      {
        //printf("Found ref\n");
        Definition *d=0;
        if (cd) d=cd; else if (nd) d=nd; else if (fd) d=fd; else d=gd;
        if (d && (external ? d->isLinkable() : d->isLinkableInProject()))
        {
          //ol.writeObjectLink(d->getReference(),d->getOutputFileBase(),
          //                       md->anchor(),word);
          out.writeLink(d->getReference(),d->getOutputFileBase(),
                                 md->anchor(),word);
          found=TRUE;
        }
      }

      if (!found) // add word to the result
      {
        //ol.docify(word);
        out.writeString(word);
      }
    }
    else
    {
      //ol.docify(word);
      out.writeString(word);
    }
    // set next start point in the string
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
    parseText(ol,exampleLine.mid(index,newIndex-index));
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
  parseText(ol,exampleLine.right(exampleLine.length()-index));
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
  if (!al || al->count()==0) return result;
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
void setAnchors(char id,MemberList *ml,int groupId)
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
      //printf("Member %s anchor %s\n",md->name(),anchor.data());
      md->setAnchor(anchor);
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
      QCString cmd=Config_getString("INPUT_FILTER")+" "+name;
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

// removes the (one and only) occurrence of name:: from s.
static QCString trimScope(const QCString &name,const QCString &s)
{
  int scopeOffset=name.length();
  QCString result=s;
  do // for each scope
  {
    QCString tmp;
    QCString scope=name.left(scopeOffset)+"::";
    //printf("Trying with scope=`%s'\n",scope.data());
    
    int i,p=0;
    while ((i=result.find(scope,p))!=-1) // for each occurrence
    {
      tmp+=result.mid(p,i-p); // add part before pattern
      p=i+scope.length();
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
  int i;
  if (s=="const")    { s.resize(0); return; }
  if (s=="volatile") { s.resize(0); return; }

  // strip occurrences of const
  i = s.find("const ");
  if (i!=-1) 
  {
    // no & or * after the const
    if (s.find('*',i+6)==-1 && s.find('&',i+6)==-1)
    {
      s=s.left(i)+s.right(s.length()-i-6); 
    }
  }

  // strip occurrences of volatile
  i = s.find("volatile ");
  if (i!=-1) 
  {
    // no & or * after the volatile
    if (s.find('*',i+9)==-1 && s.find('&',i+9)==-1)
    {
      s=s.left(i)+s.right(s.length()-i-9); 
    }
  }
}


// a bit of debug support for matchArguments
#define MATCH
#define NOMATCH
//#define MATCH printf("Match at line %d\n",__LINE__);
//#define NOMATCH printf("Nomatch at line %d\n",__LINE__);

static bool matchArgument(const Argument *srcA,const Argument *dstA,
                   const QCString &className,
                   const QCString &namespaceName,
                   NamespaceList *usingNamespaces,
                   ClassList *usingClasses)
{
  //printf("match argument start %s:%s <-> %s:%s\n",
  //    srcA->type.data(),srcA->name.data(),
  //    dstA->type.data(),dstA->name.data());

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
      NamespaceListIterator nli(*usingNamespaces);
      NamespaceDef *nd;
      for (;(nd=nli.current());++nli)
      {
        srcAType=trimScope(nd->name(),srcAType);
        dstAType=trimScope(nd->name(),dstAType);
      }
    }
    if (usingClasses && usingClasses->count()>0)
    {
      ClassListIterator cli(*usingClasses);
      ClassDef *cd;
      for (;(cd=cli.current());++cli)
      {
        srcAType=trimScope(cd->name(),srcAType);
        dstAType=trimScope(cd->name(),dstAType);
      }
    }

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
    
    //printf("2. srcA=%s:%s dstA=%s:%s\n",srcAType.data(),srcA->name.data(),
    //    dstAType.data(),dstA->name.data());

    uint srcPos=0,dstPos=0; 
    bool equal=TRUE;
    while (srcPos<srcAType.length() && dstPos<dstAType.length() && equal)
    {
      equal=srcAType.at(srcPos)==dstAType.at(dstPos);
      if (equal) srcPos++,dstPos++; 
    }
    if (srcPos<srcAType.length() && dstPos<dstAType.length())
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
        // check if a name if already found -> if no then there is no match
        if (!srcA->name.isEmpty() || !dstA->name.isEmpty()) 
        {
          NOMATCH
          return FALSE;
        }
        while (srcPos<srcAType.length() && isId(srcAType.at(srcPos))) srcPos++;
        while (dstPos<dstAType.length() && isId(dstAType.at(dstPos))) dstPos++;
        if (srcPos<srcAType.length() || dstPos<dstAType.length()) 
        {
          NOMATCH
          return FALSE;
        }
      }
      else
      {
        // otherwise we assume that a name starts at the current position.
        while (srcPos<srcAType.length() && isId(srcAType.at(srcPos))) srcPos++;
        while (dstPos<dstAType.length() && isId(dstAType.at(dstPos))) dstPos++;

        // if nothing more follows for both types then we assume we have
        // found a match. Note that now `signed int' and `signed' match, but
        // seeing that int is not a name can only be done by looking at the
        // semantics.

        if (srcPos!=srcAType.length() || dstPos!=dstAType.length()) 
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
      else // maybe dst has a name while src has not
      {
        dstPos++;
        while (dstPos<dstAType.length() && isId(dstAType.at(dstPos))) dstPos++;
        if (dstPos!=dstAType.length()) 
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
        if (srcPos!=srcAType.length()) 
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

static void mergeArgument(Argument *srcA,Argument *dstA,
                   const QCString &className,
                   const QCString &namespaceName,
                   NamespaceList *usingNamespaces,
                   ClassList *usingClasses)
{
  //printf("merge argument start %s:%s <-> %s:%s\n",
  //    srcA->type.data(),srcA->name.data(),
  //    dstA->type.data(),dstA->name.data());

  if ((srcA->type=="const" || srcA->type=="volatile") && !srcA->name.isEmpty())
  {
    srcA->type+=" ";
    srcA->type+=srcA->name;
    srcA->name.resize(0);
  } 
  if ((dstA->type=="const" || dstA->type=="volatile") && !dstA->name.isEmpty())
  {
    dstA->type+=" ";
    dstA->type+=dstA->name;
    dstA->name.resize(0);
  } 
  if (srcA->name=="const" || srcA->name=="volatile")
  {
    srcA->type+=" ";
    srcA->type+=srcA->name;
    srcA->type=removeRedundantWhiteSpace(srcA->type);
    srcA->name.resize(0);
  }
  if (dstA->name=="const" || dstA->name=="volatile")
  {
    dstA->type+=" ";
    dstA->type+=dstA->name;
    dstA->type=removeRedundantWhiteSpace(dstA->type);
    dstA->name.resize(0);
  }
  
  QCString srcAType=trimTemplateSpecifiers(namespaceName,className,srcA->type);
  QCString dstAType=trimTemplateSpecifiers(namespaceName,className,dstA->type);
  if (srcAType.left(6)=="class ") srcAType=srcAType.right(srcAType.length()-6);
  if (dstAType.left(6)=="class ") dstAType=dstAType.right(dstAType.length()-6);

  stripIrrelevantConstVolatile(srcAType);
  stripIrrelevantConstVolatile(dstAType);

  if (srcAType!=dstAType) // check if the argument only differs on name 
  {
    //printf("scope=`%s': `%s' <=> `%s'\n",className.data(),srcAType.data(),dstAType.data());

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
      NamespaceListIterator nli(*usingNamespaces);
      NamespaceDef *nd;
      for (;(nd=nli.current());++nli)
      {
        srcAType=trimScope(nd->name(),srcAType);
        dstAType=trimScope(nd->name(),dstAType);
      }
    }
    if (usingClasses && usingClasses->count()>0)
    {
      ClassListIterator cli(*usingClasses);
      ClassDef *cd;
      for (;(cd=cli.current());++cli)
      {
        srcAType=trimScope(cd->name(),srcAType);
        dstAType=trimScope(cd->name(),dstAType);
      }
    }

    if (!srcA->name.isEmpty() && !dstA->type.isEmpty() &&
        (srcAType+" "+srcA->name)==dstAType)
    {
      srcA->type=srcAType+" "+srcA->name;
      srcA->name.resize(0);
      goto done;
    }
    else if (!dstA->name.isEmpty() && !srcA->type.isEmpty() &&
        (dstAType+" "+dstA->name)==srcAType)
    {
      dstA->type=dstAType+" "+dstA->name;
      dstA->name.resize(0);
      goto done;
    }
    //printf("srcA=%s::%s dstA=%s::%s\n",srcAType.data(),srcA->name.data(),
    //    dstAType.data(),dstA->name.data());

    uint srcPos=0,dstPos=0; 
    bool equal=TRUE;
    while (srcPos<srcAType.length() && dstPos<dstAType.length() && equal)
    {
      equal=srcAType.at(srcPos)==dstAType.at(dstPos);
      if (equal) srcPos++,dstPos++; 
    }
    if (srcPos<srcAType.length() && dstPos<dstAType.length())
    {
      // if nothing matches or the match ends in the middle or at the
      // end of a string then there is no match
      int srcStart=srcPos;
      int dstStart=dstPos;
      if (isId(srcAType.at(srcPos)) && isId(dstAType.at(dstPos)))
      {
        // find the start of the name
        while (srcStart>=0 && isId(srcAType.at(srcStart))) srcStart--;
        while (dstStart>=0 && isId(dstAType.at(dstStart))) dstStart--;
        if (srcStart>0) // move the name from the type to the name field
        {
          srcA->name=srcAType.right(srcAType.length()-srcStart-1);
          srcA->type=srcAType.left(srcStart+1).stripWhiteSpace(); 
        } 
        if (dstStart>0) // move the name from the type to the name field
        {
          dstA->name=dstAType.right(dstAType.length()-dstStart-1);
          dstA->type=dstAType.left(dstStart+1).stripWhiteSpace(); 
        } 
      }
      else
      {
        dstA->name=dstAType.right(dstAType.length()-dstStart);
        dstA->type=dstAType.left(dstStart).stripWhiteSpace();
        srcA->name=srcAType.right(dstAType.length()-srcStart);
        srcA->type=srcAType.left(srcStart).stripWhiteSpace();
      }
    }
    else if (dstPos<dstAType.length())
    {
      if (!isspace(dstAType.at(dstPos))) // maybe the names differ
      {
        int startPos=dstPos;
        while (startPos>=0 && isId(dstAType.at(startPos))) startPos--;
        if (startPos>0)
        {
          dstA->name=dstAType.right(dstAType.length()-startPos-1);
          dstA->type=dstAType.left(startPos+1).stripWhiteSpace(); 
        } 
      }
      else // maybe dst has a name while src has not
      {
        dstPos++;
        int startPos=dstPos;
        dstA->name=dstAType.right(dstAType.length()-startPos);
        dstA->type=dstAType.left(startPos).stripWhiteSpace();
      }
    }
    else if (srcPos<srcAType.length())
    {
      if (!isspace(srcAType.at(srcPos))) // maybe the names differ
      {
        int startPos=srcPos;
        while (startPos>=0 && isId(srcAType.at(startPos))) startPos--;
        if (startPos>0)
        {
          srcA->name=srcAType.right(srcAType.length()-startPos-1);
          srcA->type=srcAType.left(startPos+1).stripWhiteSpace(); 
        } 
      }
      else // maybe src has a name while dst has not
      {
        srcPos++;
        int startPos=srcPos;
        srcA->name=srcAType.right(srcAType.length()-startPos);
        srcA->type=srcAType.left(startPos).stripWhiteSpace();
      }
    }
    goto done;
  }
  //printf("match exactly\n");
  if (srcA->name.isEmpty() && dstA->name.isEmpty()) 
    // arguments match exactly but no name ->
    // see if we can find the name
  {
    int i=srcAType.length()-1;
    while (i>=0 && isId(srcAType.at(i))) i--;
    if (i>0 && i<(int)srcAType.length()-1 && srcAType.at(i)!=':') 
      // there is (probably) a name
    {
      QCString srcAName=srcAType.right(srcAType.length()-i-1);
      QCString dstAName=dstAType.right(dstAType.length()-i-1);
      if (srcAName!="const" && srcAName!="volatile" &&
          dstAName!="const" && dstAName!="volatile")
      {
        srcA->name=srcAName;
        srcA->type=srcAType.left(i+1).stripWhiteSpace();
        dstA->name=dstAName;
        dstA->type=dstAType.left(i+1).stripWhiteSpace();
      }
    } 
  }
  else if (!dstA->name.isEmpty())
  {
    srcA->name = dstA->name.copy();
  }
  else if (!srcA->name.isEmpty())
  {
    dstA->name = srcA->name.copy(); 
  }
done:
  //printf("merge argument result %s:%s <-> %s:%s\n",
  //    srcA->type.data(),srcA->name.data(),
  //    dstA->type.data(),dstA->name.data());
  return;
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
                    NamespaceList *usingNamespaces,
                    ClassList *usingClasses)
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
  // merge/correct argument type/names
  for (srcAli.toFirst(),dstAli.toFirst();
       (srcA=srcAli.current(),dstA=dstAli.current());
       ++srcAli,++dstAli
      )
  { 
    mergeArgument(srcA,dstA,className,namespaceName,
                  usingNamespaces,usingClasses);
  }
  MATCH
  return TRUE; // all arguments match 
}

// merges the initializer of two argument lists
// pre:  the types of the arguments in the list should match.
void mergeArguments(ArgumentList *srcAl,ArgumentList *dstAl)
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
        srcA->name = dstA->name.copy();
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
             FileDef *currentFile
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
      if ((fcd=getResolvedClass(Doxygen::globalScope,className)) &&  // is it a documented class
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
              matchArguments(mmd->argumentList(),argList,className,0,FALSE); 
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
                  namespaceName,FALSE); 
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
        if (found) return TRUE;
      }
      else // no scope => global function
      {
        QList<MemberDef> members;
        
        //printf("  Function with global scope `%s' args=`%s'\n",namespaceName.data(),args);
        MemberListIterator mli(*mn);
        for (mli.toFirst();(md=mli.current());++mli)
        {
          if (md->isLinkable())
          {
            fd=md->getFileDef();
            gd=md->getGroupDef();
            //printf("md->name()=`%s' md->args=`%s' fd=%p gd=%p\n",
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
                match=matchArguments(md->argumentList(),argList); 
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
        if (!strcmp(args,"()"))
        {
          // no exact match found, but if args="()" an arbitrary 
          // member will do
          md=mn->last();
          while (md)
          {
            //printf("Found member `%s'\n",md->name().data());
            if (1 /* md->isLinkable() */)
            {
              //printf("member is linkable md->name()=`%s'\n",md->name().data());
              fd=md->getFileDef();
              gd=md->getGroupDef();
              if (
                  (gd && gd->isLinkable()) || (fd && fd->isLinkable()) 
                 )
              {
                members.append(md);
              }
            }
            md=mn->prev();
          }
        }
        if (members.count()==1 || currentFile!=0)
        {
          md=members.first();
        }
        else if (members.count()>1)
        {
          // use some C scoping rules to determine the correct link
          // 1. member in current file
          // 2. non-static member in different file
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

  QCString docScopeName=docScope;
  int scopeOffset=docScopeName.length();

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
  
  QCString tsName = name;
  bool memberScopeFirst = tsName.find('#')!=-1;
  QCString tmpName = substitute(tsName,"#","::");
  QCString linkText = rt;
  int scopePos=tmpName.findRev("::");
  int bracePos=tmpName.findRev('('); // reverse is needed for operator()(...)
  if (bracePos==-1) // simple name
  {
    ClassDef *cd=0;
    NamespaceDef *nd=0;
    if (linkText.isEmpty()) linkText=tmpName;
    // check if this is a class or namespace reference
    if (scName!=tmpName && getScopeDefs(scName,name,cd,nd))
    {
      if (cd) // scope matches that of a class
      {
        od.writeObjectLink(cd->getReference(),
            cd->getOutputFileBase(),0,linkText);
        if (!cd->isReference() /*&& !Config_getBool("PDF_HYPERLINKS")*/) 
        {
          writePageRef(od,cd->getOutputFileBase(),0);
        }
      }
      else // scope matches that of a namespace
      {
        od.writeObjectLink(nd->getReference(),
            nd->getOutputFileBase(),0,linkText);
        if (!nd->getReference() /*&& !Config_getBool("PDF_HYPERLINKS")*/) 
        {
          writePageRef(od,nd->getOutputFileBase(),0);
        }
      }
      // link has been written, stop now.
      return TRUE;
    }
    else if (scName==tmpName || (!inSeeBlock && scopePos==-1)) // nothing to link => output plain text
    {
      od.docify(linkText);
      // text has been written, stop now.
      return FALSE;
    }
    // continue search...
    linkText = rt; 
  }
  
  // extract scope
  QCString scopeStr=scName;

  //printf("scopeContext=%s scopeUser=%s\n",scopeContext.data(),scopeUser.data());

  // extract userscope+name
  int endNamePos=bracePos!=-1 ? bracePos : tmpName.length();
  QCString nameStr=tmpName.left(endNamePos);

  // extract arguments
  QCString argsStr;
  if (bracePos!=-1) argsStr=tmpName.right(tmpName.length()-bracePos);
  
  // create a default link text if none was explicitly given
  if (linkText.isEmpty())
  {
    //if (!scopeUser.isEmpty()) linkText=scopeUser+"::";
    linkText=nameStr;
    if (linkText.left(2)=="::") linkText=linkText.right(linkText.length()-2);
  } 
  //printf("scope=`%s' name=`%s' arg=`%s' linkText=`%s'\n",
  //       scopeStr.data(),nameStr.data(),argsStr.data(),linkText.data());
  
  // strip template specifier
  // TODO: match against the correct partial template instantiation 
  int templPos=nameStr.find('<');
  if (templPos!=-1 && nameStr.find("operator")==-1)
  {
    int endTemplPos=nameStr.findRev('>');
    nameStr=nameStr.left(templPos)+nameStr.right(nameStr.length()-endTemplPos-1);
  }

  MemberDef *md    = 0;
  ClassDef *cd     = 0;
  FileDef *fd      = 0;
  NamespaceDef *nd = 0;
  GroupDef *gd     = 0;

  //printf("Try with scName=`%s' nameStr=`%s' argsStr=`%s'\n",
  //        scopeStr.data(),nameStr.data(),argsStr.data());

  // check if nameStr is a member or global.
  if (getDefs(scopeStr,nameStr,argsStr,md,cd,fd,nd,gd,
              scopePos==0 && !memberScopeFirst))
  {
    //printf("after getDefs md=%p cd=%p fd=%p nd=%p gd=%p\n",md,cd,fd,nd,gd);
    QCString anchor;
    if (md->isLinkable()) anchor = md->anchor();
    QCString cName,aName;
    if (cd) // nameStr is a member of cd
    {
      //printf("addObjectLink(%s,%s,%s,%s)\n",cd->getReference(),
      //      cd->getOutputFileBase(),anchor.data(),resultName.stripWhiteSpace().data());
      od.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),
          anchor,linkText.stripWhiteSpace());
      cName=cd->getOutputFileBase();
      aName=md->anchor();
    }
    else if (nd) // nameStr is a member of nd
    {
      //printf("writing namespace link\n");
      od.writeObjectLink(nd->getReference(),nd->getOutputFileBase(),
          anchor,linkText.stripWhiteSpace());
      cName=nd->getOutputFileBase();
      aName=md->anchor();
    }
    else if (fd) // nameStr is a global in file fd
    {
      //printf("addFileLink(%s,%s,%s)\n",fd->getOutputFileBase(),anchor.data(),
      //        resultName.stripWhiteSpace().data());
      od.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),
          anchor,linkText.stripWhiteSpace());
      cName=fd->getOutputFileBase();
      aName=md->anchor();
    }
    else if (gd)
    {
      //printf("addGroupLink(%s,%s,%s)\n",fd->getOutputFileBase().data(),anchor.data(),
      //        gd->name().data());
      od.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),
          anchor,linkText.stripWhiteSpace());
      cName=gd->getOutputFileBase();
      aName=md->anchor();
    }
    else // should not be reached
    {
      //printf("add no link fd=cd=0\n");
      od.docify(linkText);
    }

    // for functions we add the arguments if explicitly specified or else "()"
    if (!rt && (md->isFunction() || md->isPrototype() || md->isSignal() || md->isSlot() || md->isDefine())) 
    {
      if (argsStr.isEmpty() && (!md->isDefine() || md->argsString()!=0))
      //  od.writeString("()")
        ;
      else
        od.docify(argsStr);
    }

    // generate the page reference (for LaTeX)
    if ((!cName.isEmpty() || !aName.isEmpty()) && md->isLinkableInProject())
    {
      writePageRef(od,cName,aName);
    }
    return TRUE;
  }
  else if (inSeeBlock && !nameStr.isEmpty() && (gd=Doxygen::groupSDict[nameStr]))
  { // group link
    od.startTextLink(gd->getOutputFileBase(),0);
    if (rt) // explict link text
    {
      od.docify(rt);
    }
    else // use group title as the default link text
    {
      od.docify(gd->groupTitle());
    }
    od.endTextLink();
    return TRUE;
  }

  // nothing found
  if (rt) 
    od.docify(rt); 
  else 
  {
    od.docify(linkText);
    if (!argsStr.isEmpty()) od.docify(argsStr);
  }
  return FALSE;
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
  //printf("generateLink clName=`%s' lr=`%s' lt=`%s'\n",clName,lr,lt);
  QCString linkRef=lr;
  FileDef *fd;
  GroupDef *gd;
  PageInfo *pi;
  bool ambig;
  if (linkRef.isEmpty()) // no reference name!
  {
    od.docify(lt);
    return FALSE;
  }
  else if ((pi=Doxygen::pageSDict->find(linkRef))) // link to a page
  {
    GroupDef *gd = pi->inGroup;
    if (gd)
    {
      SectionInfo *si=0;
      if (!pi->name.isEmpty()) si=Doxygen::sectionDict[pi->name];
      od.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),si ? si->label.data() : 0,lt);  
      if (gd->isLinkableInProject())
      {
        writePageRef(od,gd->getOutputFileBase(),si ? si->label.data() : 0);
      }
    }
    else
    {
      od.writeObjectLink(pi->getReference(),pi->getOutputFileBase(),0,lt);  
      if (!pi->isReference())
      {
        writePageRef(od,pi->name,0);
      }
    }
    return TRUE;
  }
  else if ((pi=Doxygen::exampleSDict->find(linkRef))) // link to an example
  {
    od.writeObjectLink(0,convertNameToFile(pi->name+"-example"),0,lt);
    if (!pi->isReference())
    {
      writePageRef(od,convertNameToFile(pi->name+"-example"),0);
    }
    return TRUE;
  }
  else if ((gd=Doxygen::groupSDict[linkRef])) // link to a group
  {
    //od.startTextLink(gd->getOutputFileBase(),0);
    //if (lt)
    //  od.docify(lt);
    //else
    //  od.docify(gd->groupTitle());
    //od.endTextLink();
    QCString title;
    if (lt) title=lt; else title=gd->groupTitle();
    od.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),0,title);  
    if (gd->isLinkableInProject())
    {
      writePageRef(od,gd->getOutputFileBase(),0);
    }
    return TRUE;
  }
  else if ((fd=findFileDef(Doxygen::inputNameDict,linkRef,ambig))
       && fd->isLinkable())
  {
        // link to documented input file
    od.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,lt);
    if (fd->isLinkableInProject())
    {
      writePageRef(od,fd->getOutputFileBase(),0);
    }
    return TRUE;
  }
  else // probably a class or member reference
  {
    return generateRef(od,clName,lr,inSeeBlock,lt);
  }
}

void generateFileRef(OutputDocInterface &od,const char *name,const char *text)
{
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
      return fn->getFirst();
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
  //printf("getPrefixIndex(%s) ni=%d\n",name.data(),ni);
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
}

/*! Input is a scopeName, output is the scopename split into a
 *  namespace part (as large as possible) and a classname part.
 */
void extractNamespaceName(const QCString &scopeName,
                          QCString &className,QCString &namespaceName)
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
    if (i>0 && (nd=getResolvedNamespace(clName.left(i))) && getClass(clName.left(i))==0)
    {
      namespaceName=nd->name().copy();
      className=clName.right(clName.length()-i-2);
      goto done;
    } 
    p=i-2; // try a smaller piece of the scope
  }

  // not found, so we just have to guess.
  className=scopeName.copy();
  namespaceName.resize(0);

done:
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
      default:   result+=c;        break;         
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
      
void addMembersToMemberGroup(MemberList *ml,MemberGroupSDict *memberGroupSDict)
{
  MemberListIterator mli(*ml);
  MemberDef *md;
  uint index;
  for (index=0;(md=mli.current());)
  {
    int groupId=md->getMemberGroupId();
    if (groupId!=-1)
    {
      QCString *pGrpHeader = Doxygen::memberHeaderDict[groupId];
      QCString *pDocs      = Doxygen::memberDocDict[groupId];
      if (pGrpHeader)
      {
        MemberGroup *mg = memberGroupSDict->find(groupId);
        if (mg==0)
        {
          mg = new MemberGroup(groupId,*pGrpHeader,pDocs ? pDocs->data() : 0);
          memberGroupSDict->append(groupId,mg);
        }
        md = ml->take(index);
        mg->insertMember(md);
        md->setMemberGroup(mg);
      }
      else
      {
        ++mli;++index;
      }
    }
    else
    {
      ++mli;++index;
    }
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
 *  is returned as a string. The argument \a className is used to
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
          result += actArg->type; 
          found=TRUE;
        }
        else if (formArg->name==n && actArg==0 && !formArg->defval.isEmpty())
        {
          result += substituteTemplateArgumentsInString(formArg->defval,formalArgs,actualArgs);
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
 *  try to strip <T> and not <S>, while \a parentOnly is \c FALSE will 
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

void addRelatedPage(const char *name,const QCString &ptitle,
                           const QCString &doc,QList<QCString> *anchors,
                           const char *fileName,int startLine,
                           int todoId,int testId,int bugId,GroupDef *gd,
                           TagInfo *tagInfo
                          )
{
  PageInfo *pi=0;
  if ((pi=Doxygen::pageSDict->find(name)) && !tagInfo)
  {
    // append documentation block to the page.
    pi->doc+="\n\n"+doc;
  }
  else // new page
  {
    QCString baseName=name;
    if (baseName.right(4)==".tex") 
      baseName=baseName.left(baseName.length()-4);
    else if (baseName.right(5)==".html")
      baseName=baseName.left(baseName.length()-5);
    
    QCString title=ptitle.stripWhiteSpace();
    pi=new PageInfo(fileName,startLine,baseName,doc,title);
    pi->todoId=todoId;
    pi->testId=testId;
    pi->bugId=bugId;
    if (tagInfo)
    {
      pi->reference = tagInfo->tagName;
    }

    QCString pageName;
    if (Config_getBool("CASE_SENSE_NAMES"))
      pageName=pi->name.copy();
    else
      pageName=pi->name.lower();
    //setFileNameForSections(anchors,pageName,pi);
    pi->fileName = pageName;
    pi->addSections(anchors);

    Doxygen::pageSDict->append(baseName,pi);

    if (gd) gd->addPage(pi);
    
    if (!pi->title.isEmpty())
    {
      //outputList->writeTitle(pi->name,pi->title);

      // a page name is a label as well!
      SectionInfo *si=new SectionInfo(
          pi->name,pi->title,SectionInfo::Section,pi->reference);
      if (gd)
      {
        si->fileName=gd->getOutputFileBase();
      }
      else if (pi->inGroup)
      {
        si->fileName=pi->inGroup->getOutputFileBase().copy();
      }
      else
      {
        si->fileName=pageName;
      }
      //printf("si->label=`%s' si->definition=%s si->fileName=`%s'\n",
      //      si->label.data(),si->definition?si->definition->name().data():"<none>",
      //      si->fileName.data());
      //printf("  SectionInfo: sec=%p sec->fileName=%s\n",si,si->fileName.data());
      //printf("Adding section info %s\n",pi->name.data());
      Doxygen::sectionDict.insert(pageName,si);
    }
  }
}

//----------------------------------------------------------------------------

void addRefItem(int todoId,int testId,int bugId,const char *prefix,
                        const char *name,const char *title,const char *args)
{

  //printf("addRefItem(%s) todoId=%d testId=%d bugId=%d\n",name,todoId,testId,bugId);

  ////////////////////////////////////////////////////////////
  // add item to the todo list
  ////////////////////////////////////////////////////////////

  if (todoId>0 && Config_getBool("GENERATE_TODOLIST"))
  {
    RefItem *item = todoList.getRefItem(todoId);
    ASSERT(item!=0);
    if (item->written) return;

    QCString doc;
    doc += "<dl><dt>\\anchor ";
    doc += item->listAnchor;
    doc += "\n";
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
    //printf("Todo page: %s\n",doc.data());
    addRelatedPage("todo",theTranslator->trTodoList(),doc,0,"generated",1,0,0,0);

    item->written=TRUE;
  }

  ////////////////////////////////////////////////////////////
  // add item to the test list
  ////////////////////////////////////////////////////////////

  if (testId>0 && Config_getBool("GENERATE_TESTLIST"))
  {
    RefItem *item = testList.getRefItem(testId);
    ASSERT(item!=0);
    if (item->written) return;

    QCString doc;
    doc += "<dl><dt>\\anchor ";
    doc += item->listAnchor;
    doc += "\n";
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
    //printf("Test page: %s\n",doc.data());
    addRelatedPage("test",theTranslator->trTestList(),doc,0,"generated",1,0,0,0);

    item->written=TRUE;
  }
  
  ////////////////////////////////////////////////////////////
  // add item to the bug list
  ////////////////////////////////////////////////////////////

  if (bugId>0 && Config_getBool("GENERATE_BUGLIST"))
  {
    RefItem *item = bugList.getRefItem(bugId);
    ASSERT(item!=0);
    if (item->written) return;

    QCString doc;
    doc += "<dl><dt>\\anchor ";
    doc += item->listAnchor;
    doc += "\n";
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
    //printf("Bug page: %s\n",doc.data());
    addRelatedPage("bug",theTranslator->trBugList(),doc,0,"generated",1,0,0,0);

    item->written=TRUE;
  }
}


