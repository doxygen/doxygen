/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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
#include "xml.h"
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

TextGeneratorOLImpl::TextGeneratorOLImpl(OutputList &ol) : m_ol(ol) {}

void TextGeneratorOLImpl::writeString(const char *s) const
{ 
  m_ol.docify(s); 
}

void TextGeneratorOLImpl::writeBreak() const
{ 
  m_ol.pushGeneratorState();
  m_ol.disableAllBut(OutputGenerator::Html);
  m_ol.lineBreak();
  m_ol.popGeneratorState();
}

void TextGeneratorOLImpl::writeLink(const char *extRef,const char *file,
                                    const char *anchor,const char *text
                                   ) const
{
  m_ol.writeObjectLink(extRef,file,anchor,text);
}

//------------------------------------------------------------------------
// TextGeneratorXMLImpl implementation
//------------------------------------------------------------------------

TextGeneratorXMLImpl::TextGeneratorXMLImpl(QTextStream &t) : m_t(t) 
{
}

void TextGeneratorXMLImpl::writeString(const char *s) const
{ 
  writeXMLString(m_t,s); 
}

void TextGeneratorXMLImpl::writeBreak() const 
{
}

void TextGeneratorXMLImpl::writeLink(const char *extRef,const char *file,
                                     const char *anchor,const char *text
                                    ) const
{
  if (extRef==0) 
  { writeXMLLink(m_t,file,anchor,text); } 
  else // external references are not supported for XML
  { writeXMLString(m_t,text); }
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

// strip annonymous left hand side part of the scope
//QCString stripAnnonymousScope(const QCString &s)
//{
//  QCString result=s;
//  int i=0;
//  while (!result.isEmpty() && result.at(0)=='@' && (i=result.find("::"))!=-1)
//  { 
//    result=result.right(result.length()-i-2);
//  }
//  //if (result.at(0)=='@')
//  //{
//  //  result.resize(0);
//  //}
//  return result;
//}

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
QCString stripAnnonymousNamespaceScope(const QCString &s)
{
  int oi=0,i=0,p=0;
  if (s.isEmpty()) return s;
  while (s.at(p)=='@' && (i=s.find("::",p))!=-1 && 
         namespaceDict[s.left(i)]!=0) { oi=i; p=i+2; }
  if (oi==0) 
  {
    //printf("stripAnnonymousNamespaceScope(`%s')=`%s'\n",s.data(),s.data());
    return s;
  }
  else 
  {
    //printf("stripAnnonymousNamespaceScope(`%s')=`%s'\n",s.data(),s.right(s.length()-oi-2).data());
    return s.right(s.length()-oi-2);
  }
}

void writePageRef(OutputList &ol,const char *cn,const char *mn)
{
  ol.pushGeneratorState();
  
  ol.disable(OutputGenerator::Html);
  ol.disable(OutputGenerator::Man);
  if (Config::pdfHyperFlag) ol.disable(OutputGenerator::Latex);
  if (Config::rtfHyperFlag) ol.disable(OutputGenerator::RTF);
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
  QCString result;
  result.sprintf("@%d\n",id);
  return result;
}

/*! strip part of \a path if it matches
 *  one of the paths in the Config::stripFromPath list
 */
QCString stripFromPath(const QCString &path)
{
  const char *s=Config::stripFromPath.first();
  while (s)
  {
    QCString prefix = s;
    if (path.left(prefix.length())==prefix)
    {
      return path.right(path.length()-prefix.length());
    }
    s = Config::stripFromPath.next();
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
  if (n.right(2)==".c"   || // source
      n.right(3)==".cc"  ||
      n.right(4)==".cxx" ||
      n.right(4)==".cpp" ||
      n.right(4)==".c++" ||
      n.right(3)==".ii"  || // inline
      n.right(4)==".ixx" ||
      n.right(4)==".ipp" ||
      n.right(4)==".i++"
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

/*! Get a class definition given its name. 
 *  Returns 0 if the class is not found.
 */
ClassDef *getClass(const char *name)
{
  if (name==0 || name[0]=='\0') return 0;
  return classDict[name];
}

NamespaceDef *getResolvedNamespace(const char *name)
{
  if (name==0 || name[0]=='\0') return 0;
  QCString *subst = namespaceAliasDict[name];
  if (subst)
  {
    int count=0; // recursion detection guard
    QCString *newSubst;
    while ((newSubst=namespaceAliasDict[*subst]) && count<10)
    {
      subst=newSubst;
      count++;
    }
    if (count==10)
    {
      warn_cont("Warning: possible recursive namespace alias detected for %s!\n",name);
    }
    return namespaceDict[subst->data()];
  }
  else
  {
    return namespaceDict[name];
  }
}

ClassDef *getResolvedClass(const char *name)
{
  if (name==0 || name[0]=='\0') return 0;
  QCString *subst = typedefDict[name];
  if (subst) // there is a typedef with this name
  {
    //printf("getResolvedClass `%s'->`%s'\n",name,subst->data());
    if (*subst==name) // avoid resolving typedef struct foo foo; 
    {
      return classDict[name];
    }
    int count=0; // recursion detection guard
    QCString *newSubst;
    while ((newSubst=typedefDict[*subst]) && count<10)
    {
      if (*subst==*newSubst) return classDict[subst->data()]; // for breaking typedef struct A A; 
      subst=newSubst;
      count++;
    }
    if (count==10)
    {
      warn_cont("Warning: possible recursive typedef dependency detected for %s!\n",name);
      return classDict[name];
    }
    else
    {
      //printf("getClass: subst %s->%s\n",name,subst->data());
      int i;
      ClassDef *cd = classDict[subst->data()];
      if (cd==0 && (i=subst->find('<'))>0) // try unspecialized version as well
      {
        return classDict[subst->left(i)];
      }
      else
      {
        return cd;
      }
    }
  }
  else
  {
    return classDict[name];
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

QCString removeRedundantWhiteSpace(const QCString &s)
{
  if (s.isEmpty()) return s;
  QCString result;
  uint i;
  uint l=s.length();
  for (i=0;i<l;i++)
  {
    char c=s.at(i);
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
    else if (c!=' ' ||
	(i!=0 && i!=l-1 && isId(s.at(i-1)) && isId(s.at(i+1)))
       )
    {
      if ((c=='*' || c=='&' || c=='@'))
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


void writeExample(OutputList &ol,ExampleList *el)
{
  QCString exampleLine=theTranslator->trWriteList(el->count());
 
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
    Example *e=el->at(entryIndex);
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
    if (a) result+=","; 
  }
  result+=")";
  if (al->constSpecifier) result+=" const";
  if (al->volatileSpecifier) result+=" volatile";
  return result;
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
  return result;
}

static bool manIsEnabled;

void startTitle(OutputList &ol,const char *fileName)
{
  ol.startTitleHead(fileName);
  manIsEnabled=ol.isEnabled(OutputGenerator::Man);
  if (manIsEnabled) ol.disable(OutputGenerator::Man);
}

void endTitle(OutputList &ol,const char *fileName,const char *name)
{
  if (manIsEnabled) ol.enable(OutputGenerator::Man); 
  ol.endTitleHead(fileName,name);
}

void writeQuickLinks(OutputList &ol,bool compact,bool ext)
{
  ol.pushGeneratorState();
  //bool manEnabled = ol.isEnabled(OutputGenerator::Man);
  //bool texEnabled = ol.isEnabled(OutputGenerator::Latex);
  ol.disableAllBut(OutputGenerator::Html);
  QCString extLink;
  if (ext) { extLink="_doc"; }
  //if (manEnabled) ol.disable(OutputGenerator::Man);
  //if (texEnabled) ol.disable(OutputGenerator::Latex);
  if (compact) ol.startCenter(); else ol.startItemList();

  if (!compact) ol.writeListItem();
  if (Config::ftvHelpFlag)
  {
    ol.startQuickIndexItem(extLink,"main.html");
  }
  else
  {
    ol.startQuickIndexItem(extLink,"index.html");
  }
  parseText(ol,theTranslator->trMainPage());
  ol.endQuickIndexItem();

  if (documentedGroups>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"modules.html");
    parseText(ol,theTranslator->trModules());
    ol.endQuickIndexItem();
  } 
  if (documentedNamespaces>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"namespaces.html");
    parseText(ol,theTranslator->trNamespaceList());
    ol.endQuickIndexItem();
  }
  if (hierarchyClasses>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"hierarchy.html");
    parseText(ol,theTranslator->trClassHierarchy());
    ol.endQuickIndexItem();
  } 
  if (annotatedClasses>0)
  {
    if (Config::alphaIndexFlag)
    {
      if (!compact) ol.writeListItem();
      ol.startQuickIndexItem(extLink,"classes.html");
      parseText(ol,theTranslator->trAlphabeticalList());
      ol.endQuickIndexItem();
    }
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"annotated.html");
    parseText(ol,theTranslator->trCompoundList());
    ol.endQuickIndexItem();
  } 
  if (documentedHtmlFiles>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"files.html");
    parseText(ol,theTranslator->trFileList());
    ol.endQuickIndexItem();
  } 
  //if (documentedIncludeFiles>0 && Config::verbatimHeaderFlag)
  //{
  //  if (!compact) ol.writeListItem();
  //  ol.startQuickIndexItem(extLink,"headers.html");
  //  parseText(ol,theTranslator->trHeaderFiles());
  //  ol.endQuickIndexItem();
  //}
  //if (Config::sourceBrowseFlag) 
  //{
  //  if (!compact) ol.writeListItem();
  //  ol.startQuickIndexItem(extLink,"sources.html");
  //  parseText(ol,theTranslator->trSources());
  //  ol.endQuickIndexItem();
  //}
  if (documentedNamespaceMembers>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"namespacemembers.html");
    parseText(ol,theTranslator->trNamespaceMembers());
    ol.endQuickIndexItem();
  }
  if (documentedMembers>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"functions.html");
    parseText(ol,theTranslator->trCompoundMembers());
    ol.endQuickIndexItem();
  } 
  if (documentedFunctions>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"globals.html");
    parseText(ol,theTranslator->trFileMembers());
    ol.endQuickIndexItem();
  } 
  if (documentedPages>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"pages.html");
    parseText(ol,theTranslator->trRelatedPages());
    ol.endQuickIndexItem();
  } 
  if (exampleSDict->count()>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"examples.html");
    parseText(ol,theTranslator->trExamples());
    ol.endQuickIndexItem();
  } 
  if (Config::searchEngineFlag)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem("_cgi","");
    parseText(ol,theTranslator->trSearch());
    ol.endQuickIndexItem();
  } 
  if (compact) 
  {
    ol.endCenter(); 
    ol.writeRuler();
  }
  else 
  {
    ol.endItemList();
  }
  //if (manEnabled) ol.enable(OutputGenerator::Man);
  //if (texEnabled) ol.enable(OutputGenerator::Latex);
  ol.popGeneratorState();
}

void startFile(OutputList &ol,const char *name,const char *title,bool external)
{
  ol.startFile(name,title,external);
  if (!Config::noIndexFlag) writeQuickLinks(ol,TRUE,external);
}

void endFile(OutputList &ol,bool external)
{
  //bool latexEnabled = ol.isEnabled(OutputGenerator::Latex);
  //bool manEnabled   = ol.isEnabled(OutputGenerator::Man);
  //if (latexEnabled) ol.disable(OutputGenerator::Latex);
  //if (manEnabled)   ol.disable(OutputGenerator::Man);
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  ol.writeFooter(0,external); // write the footer
  if (Config::footerFile.isEmpty())
  {
    parseText(ol,theTranslator->trGeneratedAt(
              dateToString(TRUE),
              Config::projectName
             ));
  }
  ol.writeFooter(1,external); // write the link to the picture
  if (Config::footerFile.isEmpty())
  {
    parseText(ol,theTranslator->trWrittenBy());
  }
  ol.writeFooter(2,external); // end the footer
  //if (latexEnabled) ol.enable(OutputGenerator::Latex);
  //if (manEnabled)   ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
  ol.endFile();
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
// read a file with `name' to a string.

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
      totalSize+=size+2;
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
    if (Config::inputFilter.isEmpty() || !filter)
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
        return contents;
      }
    }
    else // filter the input
    {
      QCString cmd=Config::inputFilter+" "+name;
      FILE *f=popen(cmd,"r");
      if (!f)
      {
        err("Error: could not execute filter %s\n",Config::inputFilter.data());
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
      totalSize+=size+2;
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
static QCString trimTemplateSpecifiers(const QCString &className,const QCString &s)
{
  // first we resolve any defines
  //int i=0,p,l;
  //QCString result;
  //QRegExp r("[A-Z_a-z][A-Z_a-z0-9]*");
  //while ((p=r.match(s,i,&l))!=-1)
  //{
  //  if (p>i) result+=s.mid(i,p-i);
  //  result+=resolveDefines(s.mid(p,l));
  //  i=p+l;
  //}
  //if (i<(int)s.length()) result+=s.mid(i,s.length()-i);
  
  // We strip the template arguments following className (if any)
  QCString result=s.copy();
  int l=className.length();
  if (l>0) // there is a class name
  {
    int i,p=0;
    while ((i=result.find(className,p))!=-1) // class name is in the argument type
    {
      uint s=i+l;
      if (s<result.length() && result.at(s)=='<') // class has template args
      {
        int b=1;
        uint e=s+1;
        while (b>0 && e<result.length()) // find matching >
        {
          if (result.at(e)=='<') b++;
          else if (result.at(e)=='>') b--;
          e++;
        }
        // remove template argument
        result=result.left(s)+result.right(result.length()-e);
        if (result.length()>s && (result.at(s)=='*' || result.at(s)=='&'))
        {
          // insert a space to keep the argument in the canonical form
          result=result.left(s)+" "+result.right(result.length()-s);
        }
      }
      p=i+l;
    }
  }
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
static void trimNamespaceScope(QCString &t1,QCString &t2)
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
      if (!scope.isEmpty() && namespaceDict[scope]!=0) // scope is a namespace
      {
        t1 = t1.right(t1.length()-i1-2);
        return;
      }
    }
    else if (i1==-1 && i2!=-1) // only t2 has a scope
    {
      QCString scope=t2.left(i2);
      if (!scope.isEmpty() && namespaceDict[scope]!=0) // scope is a namespace
      {
        t2 = t2.right(t2.length()-i2-2);
        return;
      }
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
  i = s.find("const ");
  if (i!=-1) 
  {
    // no & or * after the const
    if (s.find('*',i+6)==-1 && s.find('&',i+6)==-1)
    {
      s=s.left(i)+s.right(s.length()-i-6); 
    }
  }
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
                   NamespaceList *usingList)
{
  QCString srcAType=trimTemplateSpecifiers(className,srcA->type);
  QCString dstAType=trimTemplateSpecifiers(className,dstA->type);
  if (srcAType.left(6)=="class ") srcAType=srcAType.right(srcAType.length()-6);
  if (dstAType.left(6)=="class ") dstAType=dstAType.right(dstAType.length()-6);
  stripIrrelevantConstVolatile(srcAType);
  stripIrrelevantConstVolatile(dstAType);

  if (srcA->array!=dstA->array) // nomatch for char[] against char
  {
    NOMATCH
    return FALSE;
  }
  if (srcAType!=dstAType) // check if the argument only differs on name 
  {
    //printf("scope=`%s': `%s' <=> `%s'\n",className.data(),srcAType.data(),dstAType.data());

    // remove a namespace scope that is only in one type 
    // (assuming a using statement was used)
    trimNamespaceScope(srcAType,dstAType);

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
    if (usingList && usingList->count()>0)
    {
      NamespaceListIterator nli(*usingList);
      NamespaceDef *nd;
      for (;(nd=nli.current());++nli)
      {
        srcAType=trimScope(nd->name(),srcAType);
        dstAType=trimScope(nd->name(),dstAType);
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
                   NamespaceList *usingList)
{
  QCString srcAType=trimTemplateSpecifiers(className,srcA->type);
  QCString dstAType=trimTemplateSpecifiers(className,dstA->type);
  if (srcAType.left(6)=="class ") srcAType=srcAType.right(srcAType.length()-6);
  if (dstAType.left(6)=="class ") dstAType=dstAType.right(dstAType.length()-6);
  stripIrrelevantConstVolatile(srcAType);
  stripIrrelevantConstVolatile(dstAType);

  if (srcAType!=dstAType) // check if the argument only differs on name 
  {
    //printf("scope=`%s': `%s' <=> `%s'\n",className.data(),srcAType.data(),dstAType.data());

    // remove a namespace scope that is only in one type 
    // (assuming a using statement was used)
    trimNamespaceScope(srcAType,dstAType);

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
    if (usingList && usingList->count()>0)
    {
      NamespaceListIterator nli(*usingList);
      NamespaceDef *nd;
      for (;(nd=nli.current());++nli)
      {
        srcAType=trimScope(nd->name(),srcAType);
        dstAType=trimScope(nd->name(),dstAType);
      }
    }

    if (!srcA->name.isEmpty() && !dstA->type.isEmpty() &&
        (srcAType+" "+srcA->name)==dstAType)
    {
      srcA->type=srcAType+" "+srcA->name;
      srcA->name.resize(0);
      return;
    }
    else if (!dstA->name.isEmpty() && !srcA->type.isEmpty() &&
        (dstAType+" "+dstA->name)==srcAType)
    {
      dstA->type=dstAType+" "+dstA->name;
      dstA->name.resize(0);
      return;
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
    return;
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
      srcA->name=srcAType.right(srcAType.length()-i-1);
      srcA->type=srcAType.left(i+1).stripWhiteSpace();
      dstA->name=dstAType.right(dstAType.length()-i-1);
      dstA->type=dstAType.left(i+1).stripWhiteSpace();
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
                    NamespaceList *usingList)
{
  QCString className=cl;
  QCString namespaceName=ns;

  // strip template specialization from class name if present
  int til=className.find('<'),tir=className.find('>');
  if (til!=-1 && tir!=-1 && tir>til) 
  {
    className=className.left(til)+className.right(className.length()-tir-1);
  }

  //printf("matchArguments(%s,%s) className=%s namespaceName=%s checkCV=%d\n",
  //    srcAl ? argListToString(srcAl).data() : "",
  //    dstAl ? argListToString(dstAl).data() : "",
  //    cl,ns,checkCV);

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
    if (!matchArgument(srcA,dstA,className,namespaceName,usingList))
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
    mergeArgument(srcA,dstA,className,namespaceName,usingList);
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
  
  MemberName *mn = memberNameDict[mName];
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
      if ((fcd=getResolvedClass(className)) &&  // is it a documented class
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
            bool match=args==0 || matchArguments(mmd->argumentList(),argList,className,0,FALSE); 
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
            if (//(mmd->protection()!=Private || Config::extractPrivateFlag) &&
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
        if (mdist<maxInheritanceDepth) return TRUE; /* found match */
      } 
      /* goto the parent scope */
      
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
  if ((mn=functionNameDict[mName])) // name is known
  {
    //printf("  >member name found\n");
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
          (fnd=namespaceDict[namespaceName]) &&
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
              match=matchArguments(mmd->argumentList(),argList,0,namespaceName,FALSE); 
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
            if ((fd && fd->isLinkable()) || 
                (gd && gd->isLinkable())
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
              if ((fd && fd->isLinkable()) ||
                  (gd && gd->isLinkable())
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
            if (!(md->isStatic())) bmd = md;     
          }
          if (bmd) md=bmd;
        }
        if (md) // found a matching global member
        {
          fd=md->getFileDef();
          gd=md->getGroupDef();
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
    else if ((nd=namespaceDict[fullName]) && nd->isLinkable())
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

bool generateRef(OutputList &ol,const char *scName,
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
        ol.writeObjectLink(cd->getReference(),
            cd->getOutputFileBase(),0,linkText);
        if (!cd->isReference() /*&& !Config::pdfHyperFlag*/) 
        {
          writePageRef(ol,cd->getOutputFileBase(),0);
        }
      }
      else // scope matches that of a namespace
      {
        ol.writeObjectLink(nd->getReference(),
            nd->getOutputFileBase(),0,linkText);
        if (!nd->getReference() /*&& !Config::pdfHyperFlag*/) 
        {
          writePageRef(ol,nd->getOutputFileBase(),0);
        }
      }
      // link has been written, stop now.
      return TRUE;
    }
    else if (scName==tmpName || (!inSeeBlock && scopePos==-1)) // nothing to link => output plain text
    {
      ol.docify(linkText);
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
    //printf("after getDefs nd=%p\n",nd);
    QCString anchor;
    if (md->isLinkable()) anchor = md->anchor();
    QCString cName,aName;
    if (cd) // nameStr is a member of cd
    {
      //printf("addObjectLink(%s,%s,%s,%s)\n",cd->getReference(),
      //      cd->getOutputFileBase(),anchor.data(),resultName.stripWhiteSpace().data());
      ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),
          anchor,linkText.stripWhiteSpace());
      cName=cd->getOutputFileBase();
      aName=md->anchor();
    }
    else if (nd) // nameStr is a member of nd
    {
      //printf("writing namespace link\n");
      ol.writeObjectLink(nd->getReference(),nd->getOutputFileBase(),
          anchor,linkText.stripWhiteSpace());
      cName=nd->getOutputFileBase();
      aName=md->anchor();
    }
    else if (fd) // nameStr is a global in file fd
    {
      //printf("addFileLink(%s,%s,%s)\n",fd->getOutputFileBase(),anchor.data(),
      //        resultName.stripWhiteSpace().data());
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),
          anchor,linkText.stripWhiteSpace());
      cName=fd->getOutputFileBase();
      aName=md->anchor();
    }
    else if (gd)
    {
      //printf("addGroupLink(%s,%s,%s)\n",fd->getOutputFileBase().data(),anchor.data(),
      //        gd->name().data());
      ol.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),
          anchor,linkText.stripWhiteSpace());
      cName=gd->getOutputFileBase();
      aName=md->anchor();
    }
    else // should not be reached
    {
      //printf("add no link fd=cd=0\n");
      ol.docify(linkText);
    }

    // for functions we add the arguments if explicitly specified or else "()"
    if (!rt && (md->isFunction() || md->isPrototype() || md->isSignal() || md->isSlot() || md->isDefine())) 
    {
      if (argsStr.isEmpty() && (!md->isDefine() || md->argsString()!=0))
      //  ol.writeString("()")
        ;
      else
        ol.docify(argsStr);
    }

    // generate the page reference (for LaTeX)
    if (!cName.isEmpty() || !aName.isEmpty())
    {
      writePageRef(ol,cName,aName);
    }
    return TRUE;
  }
  else if (inSeeBlock && !nameStr.isEmpty() && (gd=groupDict[nameStr]))
  { // group link
    ol.startTextLink(gd->getOutputFileBase(),0);
    if (rt) // explict link text
      ol.docify(rt);
    else // use group title as the default link text
    {
      ol.docify(gd->groupTitle());
    }
    ol.endTextLink();
    return TRUE;
  }

  // nothing found
  if (rt) 
    ol.docify(rt); 
  else 
  {
    ol.docify(linkText);
    if (!argsStr.isEmpty()) ol.docify(argsStr);
  }
  return FALSE;
}

//----------------------------------------------------------------------
// General function that generates the HTML code for a reference to some
// file, class or member from text `lr' within the context of class `clName'. 
// This link has the text 'lt' (if not 0), otherwise `lr' is used as a
// basis for the link's text.
// returns TRUE if a link could be generated.

bool generateLink(OutputList &ol,const char *clName,
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
    ol.docify(lt);
    return FALSE;
  }
  else if ((pi=pageSDict->find(linkRef))) // link to a page
  {
    GroupDef *gd = pi->inGroup;
    if (gd)
    {
      SectionInfo *si=0;
      if (!pi->name.isEmpty()) si=sectionDict[pi->name];
      ol.writeObjectLink(gd->getReference(),gd->getOutputFileBase(),si ? si->label.data() : 0,lt);  
      writePageRef(ol,gd->getOutputFileBase(),si ? si->label.data() : 0);
    }
    else
    {
      ol.writeObjectLink(pi->getReference(),pi->getOutputFileBase(),0,lt);  
      writePageRef(ol,pi->name,0);
    }
    return TRUE;
  }
  else if ((pi=exampleSDict->find(linkRef))) // link to an example
  {
    ol.writeObjectLink(0,convertFileName(pi->name)+"-example",0,lt);
    writePageRef(ol,convertFileName(pi->name)+"-example",0);
    return TRUE;
  }
  else if ((gd=groupDict[linkRef])) // link to a group
  {
    ol.startTextLink(gd->getOutputFileBase(),0);
    if (lt)
      ol.docify(lt);
    else
      ol.docify(gd->groupTitle());
    ol.endTextLink();
    writePageRef(ol,gd->getOutputFileBase(),0);
    return TRUE;
  }
  else if ((fd=findFileDef(inputNameDict,linkRef,ambig))
       && fd->isLinkable())
  {
        // link to documented input file
    ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,lt);
    writePageRef(ol,fd->getOutputFileBase(),0);
    return TRUE;
  }
  else // probably a class or member reference
  {
    return generateRef(ol,clName,lr,inSeeBlock,lt);
  }
}

void generateFileRef(OutputList &ol,const char *name,const char *text)
{
  QCString linkText = text ? text : name;
  //FileInfo *fi;
  FileDef *fd;
  bool ambig;
  if ((fd=findFileDef(inputNameDict,name,ambig)) && 
      fd->isLinkable()) 
    // link to documented input file
    ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,linkText);
  else
    ol.docify(linkText); 
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

QCString convertFileName(const QCString &s)
{
  QCString result;
  int i,l=s.length();
  for (i=0;i<l;i++)
  {
    if (s.at(i)!='/' && s.at(i)!='.')
    {
      if (Config::caseSensitiveNames)
      {
        result+=s[i]; 
      }
      else
      {
        result+=tolower(s[i]); 
      }
    }
    else 
    {
      result+="_";
    }
  }
  return result;
}

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
  QCString name=n;
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

void setFileNameForSections(QList<QCString> *anchorList,const char *fileName)
{
  if (!anchorList) return;
  QCString *s=anchorList->first();
  while (s)
  {
    SectionInfo *si=0;
    if (!s->isEmpty() && (si=sectionDict[*s])) si->fileName=fileName;
    s=anchorList->next();
  }
}

//----------------------------------------------------------------------

QCString substituteKeywords(const QCString &s,const char *title)
{
  QCString result = s.copy();
  if (title) result = substitute(result,"$title",title);
  result = substitute(result,"$datetime",dateToString(TRUE));
  result = substitute(result,"$date",dateToString(FALSE));
  result = substitute(result,"$doxygenversion",versionString);
  result = substitute(result,"$projectname",Config::projectName);
  result = substitute(result,"$projectnumber",Config::projectNumber);
  return result;
}
    
//----------------------------------------------------------------------

/*! Returns the character index within \a name of the first prefix
 *  in Config::ignorePrefixList that matches \a name at the left hand side,
 *  or zero if no match was found
 */ 
int getPrefixIndex(const QCString &name)
{
  int ni = name.findRev("::");
  if (ni==-1) ni=0; else ni+=2;
  //printf("getPrefixIndex(%s) ni=%d\n",name.data(),ni);
  char *s = Config::ignorePrefixList.first();
  while (s)
  {
    const char *ps=s;
    const char *pd=name.data()+ni;
    int i=0;
    while (*ps!=0 && *pd!=0 && *ps==*pd) ps++,pd++,i++;
    if (*ps==0 && *pd!=0)
    {
      return ni+i;
    }
    s = Config::ignorePrefixList.next();
  }
  return ni;
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

void initClassHierarchy(ClassList *cl)
{
  ClassListIterator cli(*cl);
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

QCString convertNameToFile(const char *name,bool allowDots)
{
  QCString result;
  char c;
  const char *p=name;
  while ((c=*p++)!=0)
  {
    switch(c)
    {
      case ':': result+="_"; break;
      case '<': result+="_lt"; break;
      case '>': result+="_gt"; break;
      case '*': result+="_ast"; break;
      case '&': result+="_amp"; break;
      case '|': result+="_p_"; break;
      case '!': result+="_e_"; break;
      case ',': result+="_x_"; break;
      case '.': if (allowDots) result+="."; else result+="_"; break;
      case ' ': break;
      default: 
        if (Config::caseSensitiveNames)
          result+=c;
        else
          result+=tolower(c); 
        break;
    }
  }
  return result;
}

/*! Input is a scopeName, output is the scopename split into a
 *  namespace part (as large as possible) and a classname part.
 */
void extractNamespaceName(const QCString &scopeName,
                          QCString &className,QCString &namespaceName)
{
  QCString clName=scopeName.copy();
  //QCString nsName;
  NamespaceDef *nd = 0;
  if (!clName.isEmpty() && (nd=getResolvedNamespace(clName)) && getClass(clName)==0)
  { // the whole name is a namespace (and not a class)
    namespaceName=nd->name().copy();
    className.resize(0);
    //printf("extractNamespace `%s' => `%s|%s'\n",scopeName.data(),
    //     className.data(),namespaceName.data());
    return;
  }
  int i,p=clName.length()-2;
  while (p>=0 && (i=clName.findRev("::",p))!=-1) 
    // see if the first part is a namespace (and not a class)
  {
    if (i>0 && (nd=getResolvedNamespace(clName.left(i))) && getClass(clName.left(i))==0)
    {
      namespaceName=nd->name().copy();
      className=clName.right(clName.length()-i-2);
      //printf("extractNamespace `%s' => `%s|%s'\n",scopeName.data(),
      //   className.data(),namespaceName.data());
      return;
    } 
    p=i-2; // try a smaller piece of the scope
  }
  className=scopeName.copy();
  namespaceName.resize(0);
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

QCString stripScope(const char *name)
{
  QCString result = name;
  int ti=result.find('<'); // find start of template
  if (ti==-1) ti=result.length();
  int i = ti>2 ? result.findRev("::",ti-2) : -1; // find scope just before template
  if (i!=-1) // found scope
  {
    result=result.right(result.length()-i-2);
  }
  return result;
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

