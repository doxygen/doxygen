/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#include <stdlib.h>
#include <ctype.h>

#include "qtbc.h"
#include <qregexp.h>
#include "util.h"
#include "message.h"
#include "classdef.h"
#include "filedef.h"
#include "doxygen.h"
#include "scanner.h"
#include "outputlist.h"
#include "defargs.h"
#include "language.h"
#include "config.h"

// an inheritance tree of depth of 100000 should be enough for everyone :-)
const int maxInheritanceDepth = 100000; 

bool isId(char c)
{
  return c=='_' || isalnum(c);
}

// strip annonymous part of the scope
QCString stripAnnonymousScope(const QCString &s)
{
  QCString result=s;
  int i;
  while (!result.isEmpty() && result.at(0)=='@' && (i=result.find("::"))!=-1)
  { 
    result=result.right(result.length()-i-2);
  }
  return result;
}


QCString generateMarker(int id)
{
  QCString result;
  result.sprintf("@%d\n",id);
  return result;
}

// try to determine if this files is a source or a header file by looking
// at the extension (5 variations are allowed in both upper and lower case)
// If anyone knows or uses another extension please let me know :-)
int guessSection(const char *name)
{
  QCString n=((QCString)name).lower();
  if (n.right(2)==".c"   ||
      n.right(3)==".cc"  ||
      n.right(4)==".cxx" ||
      n.right(4)==".cpp" ||
      n.right(4)==".c++"
     ) return Entry::SOURCE_SEC;
  if (n.right(2)==".h"   ||
      n.right(3)==".hh"  ||
      n.right(4)==".hxx" ||
      n.right(4)==".hpp" ||
      n.right(4)==".h++"
     ) return Entry::HEADER_SEC;
  return 0;
}


//QCString resolveDefines(const char *n)
//{
//  return n;
//  if (n)
//  {
//    Define *def=defineDict[n]; 
//    if (def && def->nargs==0 && !def->definition.isNull())
//    {
//      return def->definition;
//    }
//    return n;
//  }
//  return 0;
//}

QCString resolveTypedefs(const QCString &n)
{
  QCString *subst=typedefDict[n];
  if (subst && !subst->isNull())
  {
    return *subst;
  }
  else
  {
    return n;
  }
}

ClassDef *getClass(const char *name)
{
  if (!name) return 0;
  //QCString key=resolveTypedefs(resolveDefines(name));
  //Define *def=defineDict[key];
  //if (def && def->nargs==0 && def->definition.length()>0) // normal define
  //  key=def->definition; // use substitution
  
  return classDict[resolveTypedefs(name)];
}

QCString removeRedundantWhiteSpace(const QCString &s)
{
  if (s.length()==0) return s;
  QCString result;
  uint i;
  for (i=0;i<s.length();i++)
  {
    char c=s.at(i);
    if (c!=' ' ||
	(i!=0 && i!=s.length()-1 && isId(s.at(i-1)) && isId(s.at(i+1)))
       )
    {
      if ((c=='*' || c=='&') && 
	  result.length()>0 && isId(result.at(result.length()-1))
	 ) result+=' ';
      result+=c;
    }
  }
  return result;
}  

void writeTemplatePrefix(OutputList &ol,ArgumentList *al)
{
  ol.docify("template<");
  Argument *a=al->first();
  while (a)
  {
    ol.docify(a->type);
    ol.docify(a->name);
    if (a->defval.length()!=0)
    {
      ol.docify(" = ");
      ol.docify(a->defval);
    } 
    a=al->next();
    if (a) ol.docify(", ");
  }
  ol.docify("> ");
  bool latexEnabled = ol.isEnabled(OutputGenerator::Latex);
  bool manEnabled   = ol.isEnabled(OutputGenerator::Man);
  if (latexEnabled) ol.disable(OutputGenerator::Latex);
  if (manEnabled)   ol.disable(OutputGenerator::Man);
  ol.lineBreak();
  if (latexEnabled) ol.enable(OutputGenerator::Latex);
  if (manEnabled)   ol.enable(OutputGenerator::Man);
}

QCString addTemplateNames(const QCString &s,const QCString &n,const QCString &t)
{
  //printf("addTemplateNames(%s)\n",s.data());
  QCString result;
  QCString clRealName=n;
  int p=0,i;
  if ((i=clRealName.find('<'))!=-1)
  {
    clRealName=clRealName.left(i); // strip template specialization
  }
  while ((i=s.find(clRealName,p))!=-1)
  {
    result+=s.mid(p,i-p);
    uint j=clRealName.length()+i;
    if (s.length()==j || (s.at(j)!='<' && !isId(s.at(j))))
    { // add template names
      //printf("Adding %s+%s\n",clRealName.data(),t.data());
      result+=clRealName+t; 
    }
    else 
    { // template names already present
      //printf("Adding %s\n",clRealName.data());
      result+=clRealName;
    }
    p=i+clRealName.length();
  }
  result+=s.right(s.length()-p);
  //printf("result=%s\n",result.data());
  return result;
}

static void linkifyText(OutputList &ol,const char *scName,const char *name,const char *text)
{
  //printf("scope=`%s' name=`%s' Text: `%s'\n",scName,name,text);
  QRegExp regExp("[a-z_A-Z0-9:<>]+");
  QCString txtStr=text;
  OutputList result(&ol);
  int matchLen;
  int index=0;
  int newIndex;
  int skipIndex=0;
  // read a word from the text string
  while ((newIndex=regExp.match(txtStr,index,&matchLen))!=-1)
  {
    // add non-word part to the result
    result.docify(txtStr.mid(skipIndex,newIndex-skipIndex)); 
    // get word from string
    QCString word=txtStr.mid(newIndex,matchLen);
    ClassDef *cd=0;
    FileDef *fd=0;
    MemberDef *md=0;
    NamespaceDef *nd=0;

    QCString scopeName=scName;
    QCString searchName=name;
    //printf("word=`%s' scopeName=`%s' searchName=`%s'\n",
    //        word.data(),scopeName.data(),searchName.data());
    // check if `word' is a documented class name
    if (word.length()>0 && 
        word.right(searchName.length())!=searchName && 
        word!=scopeName.right(word.length())
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
          if (cd->isVisible())
          {
            result.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,word);
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

      if (!found && 
          getDefs(scName,word,0,md,cd,fd,nd) && 
          (md->isTypedef() || md->isEnumerate()) &&
          md->hasDocumentation() 
         )
      {
        if (cd && cd->isVisible()) // fullName is a member of cd
        {
          result.writeObjectLink(cd->getReference(),
              cd->getOutputFileBase(),md->anchor(),word);
          found=TRUE;
        }
        else if (nd && nd->hasDocumentation())
        {
          result.writeObjectLink(nd->getReference(),
              nd->getOutputFileBase(),md->anchor(),word);
          found=TRUE;
        }
        else if (fd && fd->hasDocumentation()) // fullName is a global in file fd
        {
          result.writeObjectLink(fd->getReference(),
              fd->getOutputFileBase(),md->anchor(),word);
          found=TRUE;
        }
      }

      if (!found) // add word to the result
      {
        result.docify(word);
      }
    }
    else
    {
      result.docify(word);
    }
    // set next start point in the string
    skipIndex=index=newIndex+matchLen;
  }
  // add last part of the string to the result.
  result.docify(txtStr.right(txtStr.length()-skipIndex));
  //printf("linkify: %s\n",result.data());
  ol+=result; 
}

static void writeDefArgumentList(OutputList &ol,ClassDef *cd,
                                 const QCString &scopeName,MemberDef *md)
{
  ArgumentList *argList=md->argumentList();
  if (argList==0) return; // member has no function like argument list
  ol.docify(" ("); // start argument list
  Argument *a=argList->first();
  QCString cName;
  if (cd && cd->templateArguments())
  {
    cName=cd->getTemplateNameString(); 
  }
  while (a)
  {
    QRegExp re(")(");
    int vp;
    if ((vp=a->type.find(re))!=-1) // argument type is a function pointer
    {
      QCString n=a->type.left(vp);
      if (cName.length()>0) n=addTemplateNames(n,cd->name(),cName);
      linkifyText(ol,scopeName,md->name(),n);
    }
    else // non-function pointer type
    {
      QCString n=a->type;
      if (cName.length()>0) n=addTemplateNames(n,cd->name(),cName);
      linkifyText(ol,scopeName,md->name(),n);
    }
    if (a->name.length()>0) // argument has a name
    { 
      ol.docify(" ");
      ol.disable(OutputGenerator::Man);
      ol.startEmphasis();
      ol.enable(OutputGenerator::Man);
      ol.docify(a->name);
      ol.disable(OutputGenerator::Man);
      ol.endEmphasis();
      ol.enable(OutputGenerator::Man);
    }
    if (vp!=-1) // write the part of the argument type 
                // that comes after the name
    {
      linkifyText(ol,scopeName,md->name(),a->type.right(a->type.length()-vp));
    }
    if (a->defval.length()>0) // write the default value
    {
      QCString n=a->defval;
      if (cName.length()>0) n=addTemplateNames(n,cd->name(),cName);
      ol.docify(" = ");
      linkifyText(ol,scopeName,md->name(),n); 
    }
    a=argList->next();
    if (a) ol.docify(", "); // there are more arguments
  }
  ol.docify(")"); // end argument list
  if (argList->constSpecifier)
  {
    ol.docify(" const");
  }
  if (argList->volatileSpecifier)
  {
    ol.docify(" volatile");
  }
}

void writeExample(OutputList &ol,ExampleList *el)
{
  QCString exampleLine=theTranslator->trWriteList(el->count());
 
  QRegExp marker("@[0-9]+");
  int index=0,newIndex,matchLen;
  // now replace all markers in inheritLine with links to the classes
  while ((newIndex=marker.match(exampleLine,index,&matchLen))!=-1)
  {
    bool ok;
    parseText(ol,exampleLine.mid(index,newIndex-index));
    uint entryIndex = exampleLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
    Example *e=el->at(entryIndex);
    if (ok && e) ol.writeObjectLink(0,e->file,e->anchor,e->name);
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
    result+= a->type+" "+a->name;
    a = al->next();
    if (a) result+=","; 
  }
  result+=")";
  if (al->constSpecifier) result+=" const";
  if (al->volatileSpecifier) result+=" volatile";
  return result;
}
          
static void writeLink(OutputList &ol,ClassDef *cd,NamespaceDef *nd,
                      FileDef *fd,MemberDef *md,const char *name)
{
  if (nd)
    ol.writeObjectLink(nd->getReference(),nd->getOutputFileBase(),
                       md->anchor(),name);
  else if (fd) 
    ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),
                       md->anchor(),name);
  else    
    ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),
                       md->anchor(),name);
}

static void warnForUndocumentedMember(MemberDef *md)
{
  ClassDef *cd=md->memberClass();
  FileDef *fd=md->getFileDef();
  if (cd)
  {
    if (!md->hasDocumentation() && md->name() && md->name()[0]!='@') 
      warn("Warning: Member %s of class %s is not documented\n",
        md->name().data(),cd->name().data());
  }
  else if (fd)
  {
    if (!md->hasDocumentation() && md->name() && md->name()[0]!='@') 
      warn("Warning: Member %s of file %s is not documented\n",
        md->name().data(),fd->name().data());
  }
}

static bool manIsEnabled;

void startTitle(OutputList &ol)
{
  ol.startTitleHead();
  manIsEnabled=ol.isEnabled(OutputGenerator::Man);
  if (manIsEnabled) ol.disable(OutputGenerator::Man);
}

void endTitle(OutputList &ol,const char *name)
{
  if (manIsEnabled) ol.enable(OutputGenerator::Man); 
  ol.endTitleHead(name);
}

void writeQuickLinks(OutputList &ol,bool compact,bool ext)
{
  bool manEnabled = ol.isEnabled(OutputGenerator::Man);
  bool texEnabled = ol.isEnabled(OutputGenerator::Latex);
  QCString extLink,absPath;
  if (ext) { extLink="_doc:"; absPath="/"; }
  if (manEnabled) ol.disable(OutputGenerator::Man);
  if (texEnabled) ol.disable(OutputGenerator::Latex);
  if (compact) ol.startCenter(); else ol.startItemList();
  if (documentedGroups>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,absPath+"modules.html");
    parseText(ol,theTranslator->trModules());
    ol.endQuickIndexItem();
  } 
  if (documentedNamespaces>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,absPath+"namespaces.html");
    parseText(ol,theTranslator->trNamespaceList());
    ol.endQuickIndexItem();
  }
  if (hierarchyClasses>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,absPath+"hierarchy.html");
    parseText(ol,theTranslator->trClassHierarchy());
    ol.endQuickIndexItem();
  } 
  if (annotatedClasses>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,absPath+"annotated.html");
    parseText(ol,theTranslator->trCompoundList());
    ol.endQuickIndexItem();
  } 
  if (documentedFiles>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,absPath+"files.html");
    parseText(ol,theTranslator->trFileList());
    ol.endQuickIndexItem();
  } 
  if (includeFiles.count()>0 && Config::verbatimHeaderFlag)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,absPath+"headers.html");
    parseText(ol,theTranslator->trHeaderFiles());
    ol.endQuickIndexItem();
  } 
  if (documentedNamespaceMembers>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,absPath+"namespacemembers.html");
    parseText(ol,theTranslator->trNamespaceMembers());
    ol.endQuickIndexItem();
  }
  if (documentedMembers>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,absPath+"functions.html");
    parseText(ol,theTranslator->trCompoundMembers());
    ol.endQuickIndexItem();
  } 
  if (documentedFunctions>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,absPath+"globals.html");
    parseText(ol,theTranslator->trFileMembers());
    ol.endQuickIndexItem();
  } 
  if (pageList.count()>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,absPath+"pages.html");
    parseText(ol,theTranslator->trRelatedPages());
    ol.endQuickIndexItem();
  } 
  if (exampleList.count()>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,absPath+"examples.html");
    parseText(ol,theTranslator->trExamples());
    ol.endQuickIndexItem();
  } 
  if (Config::searchEngineFlag)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem("_cgi:","");
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
  if (manEnabled) ol.enable(OutputGenerator::Man);
  if (texEnabled) ol.enable(OutputGenerator::Latex);
}

void startFile(OutputList &ol,const char *name,const char *title,bool external)
{
  ol.startFile(name,title,external);
  if (!Config::noIndexFlag) writeQuickLinks(ol,TRUE,external);
}

void endFile(OutputList &ol,bool external)
{
  bool latexEnabled = ol.isEnabled(OutputGenerator::Latex);
  bool manEnabled   = ol.isEnabled(OutputGenerator::Man);
  if (latexEnabled) ol.disable(OutputGenerator::Latex);
  if (manEnabled)   ol.disable(OutputGenerator::Man);
  ol.writeFooter(0,external); // write the footer
  if (Config::footerFile.length()==0)
  {
    parseText(ol,theTranslator->trGeneratedAt(
              dateToString(TRUE),
              Config::projectName
             ));
  }
  ol.writeFooter(1,external); // write the link to the picture
  if (Config::footerFile.length()==0)
  {
    parseText(ol,theTranslator->trWrittenBy());
  }
  ol.writeFooter(2,external); // end the footer
  if (latexEnabled) ol.enable(OutputGenerator::Latex);
  if (manEnabled)   ol.enable(OutputGenerator::Man);
  ol.endFile();
}


static void writeMemberDef(OutputList &ol, ClassDef *cd, NamespaceDef *nd,
                           FileDef *fd, MemberDef *md)
{
  int i,l;
  bool hasDocs=md->hasDocumentation();
  if ((!hasDocs && Config::hideMemberFlag) || 
      (Config::hideMemberFlag && 
       md->documentation().isEmpty() && 
       !Config::briefMemDescFlag && 
       !Config::repeatBriefFlag
      ) 
     ) return;
  QCString type=md->typeString();
  QRegExp r("@[0-9]+");
  if ((i=r.match(type,0,&l))==-1 || !md->enumUsed())
  {
    // strip `static' keyword from type
    if (type.left(7)=="static ") type=type.right(type.length()-7);
    // strip `friend' keyword from type
    if (type.left(7)=="friend ") type=type.right(type.length()-7);
    
    if (Config::genTagFile.length()>0)
    {
      tagFile << md->name() << " " << md->anchor() << " \""
              << md->argsString() << "\"\n";
    }
      
    QCString cname;
    if (cd)      cname=cd->name(); 
    else if (nd) cname=nd->name();
    else if (fd) cname=fd->name();

    ol.startMemberItem();
    // If there is no detailed description we need to write the anchor here.
    bool detailsVisible = md->detailsAreVisible();
    if (!detailsVisible && !Config::extractAllFlag)
    {
      QCString doxyName=md->name().copy();
      if (!cname.isEmpty()) doxyName.prepend(cname+"::");
      ol.writeDoxyAnchor(cname,md->anchor(),doxyName);
      ol.addToIndex(md->name(),cname);
      ol.addToIndex(cname,md->name());
      ol.docify("\n");
    }
    else if (!detailsVisible) // when extractAll it true we have to write
                              // a index reference and label in LaTeX because
                              // detailed section not shown in LaTeX
    {
      ol.addToIndex(md->name(),cname);
      ol.addToIndex(cname,md->name());
      ol.writeLatexLabel(cname,md->anchor());
    }

    // write type
    if (i!=-1)
    {
      QCString newType = type.left(i) + " { ... } " +
        type.right(type.length()-i-l);
      type = newType;
      //ol.docify(type);
      linkifyText(ol,cname,md->name(),type); 
    }
    else
    {
      //ol.docify(type);
      linkifyText(ol,cname,md->name(),type); 
    }
    QCString name=md->name().copy();
    bool htmlOn = ol.isEnabled(OutputGenerator::Html);
    if (htmlOn && Config::htmlAlignMemberFlag && type.length()>0)
    {
      ol.disable(OutputGenerator::Html);
    }
    if (!type.isEmpty()) ol.docify(" ");
    if (htmlOn) 
    {
      ol.enable(OutputGenerator::Html);
    }

    ol.insertMemberAlign();
    
    // write name
    if (md->hasDocumentation())
    {
      //printf("writeLink %s->%d\n",name.data(),md->hasDocumentation());
      writeLink(ol,cd,nd,fd,md,name);
    }
    else // there is a brief member description and brief member 
         // descriptions are enabled or there is no detailed description.
    {
      ol.writeBoldString(name);
    }

    if (md->argsString()) 
    {
      ol.writeString(" ");
      //ol.docify(md->argsString());
      linkifyText(ol,cname,md->name(),md->argsString()); 
    }
    
    if (md->excpString())
    {
      ol.writeString(" ");
      ol.docify(md->excpString());
    }

    ol.endMemberItem();

    // write brief description
    if (!md->briefDescription().isEmpty() && Config::briefMemDescFlag)
    {
      ol.startMemberDescription();
      parseDoc(ol,cname,md->name(),md->briefDescription());
      //if (!md->documentation().isEmpty()) 
      //{
      //  ol.disableAllBut(OutputGenerator::Html);
      //  ol.endEmphasis();
      //  ol.docify(" ");
      //  ol.startTextLink(0,md->anchor());
      //  parseText(ol,theTranslator->trMore());
      //  ol.endTextLink();
      //  ol.startEmphasis();
      //  ol.enableAll();
      //}
      ol.endMemberDescription();
      ol.newParagraph();
    }
  }
  warnForUndocumentedMember(md);
}


// write a list in HTML of all members of a certain category
// cd!=0 => ml is a list of class members
// fd!=0 => ml is a list of file `members'
void writeMemberDecs(OutputList &ol,ClassDef *cd,NamespaceDef *nd, FileDef *fd,
                 const char *title, const char *subtitle,MemberList *ml)
{
  ml->countDecMembers();
  if (ml->totalCount()==0) return;
  if (title) 
  {
    ol.startMemberHeader();
    parseText(ol,title);
    ol.endMemberHeader();
  }
  if (subtitle) 
  {
    ol.startMemberSubtitle();
    parseText(ol,subtitle);
    ol.endMemberSubtitle();
  }

  if (!fd && !nd) ol.startMemberList();
  MemberDef *md;

  if (fd && ml->defineCount()>0)
  {
    ol.startMemberHeader();
    parseText(ol,theTranslator->trDefines());
    ol.endMemberHeader();
    ol.startMemberList();
    MemberListIterator mli(*ml);
    for ( ; (md=mli.current()); ++mli )
    {
      if (md->isDefine() && 
          (md->argsString() || md->hasDocumentation() || Config::extractAllFlag)
         ) 
        writeMemberDef(ol,cd,nd,fd,md);
    }
    ol.endMemberList();
  }
  
  if ((fd || nd) && ml->protoCount()>0)
  {
    ol.startMemberHeader();
    parseText(ol,theTranslator->trFuncProtos());
    ol.startMemberList();
    MemberListIterator mli(*ml);
    for ( ; (md=mli.current()); ++mli )
    {
      if (md->isPrototype()) writeMemberDef(ol,cd,nd,fd,md);
    }
    ol.endMemberList();
  }
  
  if (ml->typedefCount()>0)
  {
    if (fd || nd) 
    {
      ol.startMemberHeader();
      parseText(ol,theTranslator->trTypedefs());
      ol.endMemberHeader();
      //ol.writeMemberHeader("Typedefs");
      ol.startMemberList();
    }
    MemberListIterator mli(*ml);
    for ( ; (md=mli.current()) ; ++mli )
    {
      if (md->isTypedef()) writeMemberDef(ol,cd,nd,fd,md);
    }
    if (fd || nd) ol.endMemberList();
  }
 
  // write enums 
  if (ml->enumCount()>0)
  {
    if (fd || nd) 
    {
      ol.startMemberHeader();
      parseText(ol,theTranslator->trEnumerations());
      ol.endMemberHeader();
      ol.startMemberList();
    }
    MemberListIterator mli(*ml);
    for ( ; (md=mli.current()) ; ++mli )
    {
      /*bool hasDocs=md->hasDocumentation();*/
      QCString type=md->typeString();
      type=type.stripWhiteSpace();
      if (md->isEnumerate() /*&& (hasDocs || !Config::hideMemberFlag)*/) 
      {
        if (!Config::hideMemberFlag ||                // do not hide undocumented members or
            !md->documentation().isEmpty() || // member has detailed descr. or
            md->hasDocumentedEnumValues() ||  // member has documented enum vales.
            Config::briefMemDescFlag ||               // brief descr. is shown or
            Config::repeatBriefFlag                   // brief descr. is repeated.
           )
        {
          OutputList typeDecl(&ol);
          QCString name=md->name().copy();
          int i=name.findRev("::");
          if (i!=-1) name=name.right(name.length()-i-2); // strip scope
          if (name[0]!='@') // not an anonymous enum
          {
            //if (Config::extractAllFlag ||
            //    (md->briefDescription().isEmpty() || !Config::briefMemDescFlag) &&
            //    (!md->documentation().isEmpty() || md->hasDocumentedEnumValues() ||
            //     (!md->briefDescription().isEmpty() && 
            //      !Config::briefMemDescFlag &&
            //      Config::repeatBriefFlag
            //     )
            //    )
            //   )
            if (md->hasDocumentation() || md->hasDocumentedEnumValues())
            {
              if (Config::genTagFile.length()>0)
                tagFile << md->name() << " " << md->anchor() 
                  << " \"" << md->argsString() << "\"";
              writeLink(typeDecl,cd,nd,fd,md,name);
            }
            else
            {
              typeDecl.writeBoldString(name);
            }
            typeDecl.writeChar(' ');
          }

          typeDecl.docify("{ ");
          QList<MemberDef> *fmdl=md->enumFieldList();
          if (fmdl)
          {
            MemberDef *fmd=fmdl->first();
            while (fmd)
            {
              if (fmd->hasDocumentation())
              {
                if (Config::genTagFile.length()>0)
                  tagFile << fmd->name() << " " << fmd->anchor() 
                    << " \"" << fmd->argsString() << "\"";
                writeLink(typeDecl,cd,nd,fd,fmd,fmd->name());
              }
              else
                typeDecl.writeBoldString(fmd->name());
              fmd=fmdl->next();
              if (fmd) typeDecl.writeString(", ");
              typeDecl.disable(OutputGenerator::Man);
              typeDecl.writeString("\n"); // to prevent too long lines in LaTeX
              typeDecl.enable(OutputGenerator::Man);
            }
          }
          typeDecl.docify(" }");
          md->setEnumDecl(typeDecl);
          int enumVars=0;
          MemberListIterator vmli(*ml);
          MemberDef *vmd;
          if (name[0]=='@') // anonymous enum => append variables
          {
            for ( ; (vmd=vmli.current()) ; ++vmli)
            {
              QCString vtype=vmd->typeString();
              if ((vtype.find(name))!=-1) enumVars++;
            }
          }
          if (enumVars==0) // no variable of this enum type
          {
            ol.startMemberItem();
            ol.writeString("enum ");
            ol.insertMemberAlign();
            ol+=typeDecl;
            ol.endMemberItem();
            //QCString brief=md->briefDescription();
            //brief=brief.stripWhiteSpace();
            if (!md->briefDescription().isEmpty() && Config::briefMemDescFlag)
            {
              ol.startMemberDescription();
              parseDoc(ol,cd?cd->name().data():0,
                  md->name().data(),md->briefDescription());
              //if (!md->documentation().isEmpty() || md->hasDocumentedEnumValues())
              //{
              //  ol.disableAllBut(OutputGenerator::Html);
              //  ol.endEmphasis();
              //  ol.docify(" ");
              //  ol.startTextLink(0,md->anchor());
              //  //ol.writeObjectLink(0,0,md->anchor()," More...");
              //  parseText(ol,theTranslator->trMore());
              //  ol.endTextLink();
              //  ol.startEmphasis();
              //  ol.enableAll();
              //}
              ol.endMemberDescription();
              ol.disable(OutputGenerator::Man);
              ol.newParagraph();
              ol.enable(OutputGenerator::Man);
            }
          }
          warnForUndocumentedMember(md);
        }
      } // md->isEnumerate()
    } // enum loop
    if (fd || nd) ol.endMemberList();
  } // write enums
 
  // write functions
  if (ml->funcCount()>0)
  {
    if (fd || nd) 
    {
      ol.startMemberHeader();
      parseText(ol,theTranslator->trFunctions());
      ol.endMemberHeader();
      ol.startMemberList();
    }
    MemberListIterator mli(*ml);
    for ( ; (md=mli.current()) ; ++mli )
    {
      if ( md->isFunction() || md->isSignal() || 
           md->isSlot()) 
        writeMemberDef(ol,cd,nd,fd,md);
    }
    if (fd || nd) ol.endMemberList();
  }
  
  if (ml->friendCount()>0)
  {
    MemberListIterator mli(*ml);
    for ( ; (md=mli.current()) ; ++mli )
    {
      if ( md->isFriend()) 
      {
        QCString type=md->typeString();
        //printf("Friend: type=%s name=%s\n",type.data(),md->name().data());
        if (md->hasDocumentation() && type!="friend class")
        {
          writeMemberDef(ol,cd,nd,fd,md);
        }
        else // friend is undocumented as a member but it is a class, 
             // so generate a link to the class if that is documented.
        {
          ClassDef *cd=getClass(md->name());
          if (md->hasDocumentation()) // friend is documented
          {
            ol.startMemberItem();
            ol.docify("class ");
            ol.insertMemberAlign();
            ol.writeObjectLink(0,0,md->anchor(),md->name());
            ol.endMemberItem();
          }
          else if (cd && cd->isVisibleExt()) // class is documented
          {
            ol.startMemberItem();
            ol.docify("class ");
            ol.insertMemberAlign();
            ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),0,cd->name());
            ol.endMemberItem();
          }
          else if (!Config::hideMemberFlag) // no documentation
          {
            ol.startMemberItem();
            ol.docify("class ");
            ol.insertMemberAlign();
            ol.writeBoldString(md->name());
            ol.endMemberItem();
          }
        }
      }
    }
  }

  // write variables
  if (ml->varCount()>0)
  {
    if (fd || nd) 
    {
      ol.startMemberHeader();
      parseText(ol,theTranslator->trVariables());
      ol.endMemberHeader();
      ol.startMemberList();
    }
    MemberListIterator mli(*ml);
    for ( ; (md=mli.current()) ; ++mli )
    {
      if (md->isVariable()) writeMemberDef(ol,cd,nd,fd,md);
    }
    if (fd || nd) ol.endMemberList();
  }
 
  if (!fd && !nd) { ol.endMemberList(); ol.writeChar('\n'); }
}

// compute the HTML anchors for a list of members
void setAnchors(char id,MemberList *ml)
{
  int count=0;
  MemberDef *md=ml->first();
  while (md)
  {
    QCString anchor;
    anchor.sprintf("%c%d",id,count++);
    //printf("Member %s anchor %s\n",md->name(),anchor.data());
    md->setAnchor(anchor);
    md=ml->next();
  }
}

void writeMemberDocs(OutputList &ol,MemberList *ml,const char *scopeName,
                         MemberDef::MemberType m)
{
  MemberListIterator mli(*ml);
  MemberDef *md;
  for ( ; (md=mli.current()) ; ++mli)
  {
    bool hasDocs = md->detailsAreVisible();
    //     !md->documentation().isEmpty() ||          // member has a detailed description
    //     (md->memberType()==MemberDef::Enumeration && // or member is an enum and
    //      md->hasDocumentedEnumValues()             // one of its values is documented
    //     ) ||                                       // or 
    //     (!md->briefDescription().isEmpty() &&      // member has brief description and
    //      !Config::briefMemDescFlag &&                      // brief description not shown earlier and
    //      Config::repeatBriefFlag                           // brief description should be repeated.
    //     );
    if (md->memberType()==m &&                      // filter member type
        (Config::extractAllFlag || hasDocs) 
       )
    {
      if (Config::extractAllFlag && !hasDocs) 
      {
        ol.disable(OutputGenerator::Latex); // Latex cannot insert a pagebreak 
                                            // if there are a lot of empty sections,
                                            // so we disable LaTeX for all empty 
                                            // sections even if Config::extractAllFlag is enabled
      }
      QCString cname;
      NamespaceDef *nd=md->getNamespace();
      ClassDef     *cd=md->memberClass();
      FileDef      *fd=md->getFileDef();
      if (cd)      cname=cd->name(); 
      else if (nd) cname=nd->name();
      else if (fd) cname=fd->name();
      // get member name
      QCString doxyName=md->name().copy();
      // prepend scope if there is any 
      if (scopeName) doxyName.prepend((QCString)scopeName+"::");
      
      QCString def = md->definition();
      if (md->isEnumerate()) def.prepend("enum ");
      MemberDef *smd;
      if (md->isEnumValue() && def[0]=='@') def = def.right(def.length()-2);
      int i=0,l,dummy;
      QRegExp r("@[0-9]+");
      if (md->isEnumerate() && r.match(def,0,&l)!=-1) continue;
      if (md->isEnumValue() && (smd = md->getEnumScope()) 
          && r.match(smd->name(),0,&dummy)==-1) continue;
      if ((md->isVariable() || md->isTypedef()) && (i=r.match(def,0,&l))!=-1)
      {
        // find enum type an insert it in the definition
        MemberListIterator vmli(*ml);
        MemberDef *vmd;
        bool found=FALSE;
        for ( ; (vmd=vmli.current()) && !found ; ++vmli)
        {
          if (vmd->isEnumerate() && def.mid(i,l)==vmd->name())
          {
            ol.startMemberDoc(cname,md->name(),md->anchor());
            ol.writeDoxyAnchor(cname,md->anchor(),doxyName);
            linkifyText(ol,scopeName,md->name(),def.left(i));
            ol+=*vmd->enumDecl();
            linkifyText(ol,scopeName,md->name(),def.right(def.length()-i-l));
            found=TRUE;
          }
        }
        if (!found) // anonymous compound
        {
          ol.startMemberDoc(cname,md->name(),md->anchor());
          ol.writeDoxyAnchor(cname,md->anchor(),doxyName);
          linkifyText(ol,scopeName,md->name(),def.left(i));
          ol.docify(" { ... } ");
          linkifyText(ol,scopeName,md->name(),def.right(def.length()-i-l));
        }
      }
      else
      {
        ol.startMemberDoc(cname,md->name(),md->anchor());
        ol.writeDoxyAnchor(cname,md->anchor(),doxyName);
        ArgumentList *al=0;
        if (cd && (!md->isRelated() || !md->templateArguments()) && 
            ((al=md->scopeTemplateArguments()) || (al=cd->templateArguments()))
           ) // class template prefix
        {
          writeTemplatePrefix(ol,al);
        }
        if (al && md->templateArguments()) ol.docify(" ");
        al=md->templateArguments();
        if (al) // function template prefix
        {
          writeTemplatePrefix(ol,al);
        }
        if (cd && md->scopeTemplateArguments())
        {
          def=addTemplateNames(def,cd->name(),md->getScopeTemplateNameString());
        }
        else if (cd && cd->templateArguments())
        {
          // add template name lists to all occurrences of the class name.
          def=addTemplateNames(def,cd->name(),cd->getTemplateNameString());
        }
        linkifyText(ol,scopeName,md->name(),def);
        writeDefArgumentList(ol,cd,scopeName,md);
        if (md->excpString())
        {
          ol.docify(" ");
          linkifyText(ol,scopeName,md->name(),md->excpString());
        }
      }
      
      Specifier virt=md->virtualness();
      MemberDef *rmd=md->reimplements();
      while (rmd && virt==Normal)
      {
        virt = rmd->virtualness()==Normal ? Normal : Virtual;
        rmd  = rmd->reimplements();
      }

      if (md->isStatic() || md->protection()!=Public || 
          virt!=Normal || md->isSignal() || md->isFriend() || 
          md->isRelated() || md->isSlot()
         )
      {
        // write the member specifier list
        ol.writeLatexSpacing();
        ol.startTypewriter();
        ol.docify(" [");
        QStrList sl;
        if (md->isFriend()) sl.append("friend");
        else if (md->isRelated()) sl.append("related");
        else
        {
          if      (md->isStatic())              sl.append("static");
          if      (md->protection()==Protected) sl.append("protected");
          else if (md->protection()==Private)   sl.append("private");
          if      (virt==Virtual)               sl.append("virtual");
          else if (virt==Pure)                  sl.append("pure virtual");
          if      (md->isSignal())              sl.append("signal");
          if      (md->isSlot())                sl.append("slot");
        }
        const char *s=sl.first();
        while (s)
        {
          ol.docify(s);
          s=sl.next();
          if (s) ol.docify(", ");
        }
        ol.docify("]");
        ol.endTypewriter();
      }
      ol.endMemberDoc();
      ol.startIndent();
      ol.newParagraph();

      if (!md->briefDescription().isEmpty() && 
          (Config::repeatBriefFlag || 
             (!Config::briefMemDescFlag && md->documentation().isEmpty())
          )
         )  
      { 
        parseDoc(ol,scopeName,md->name(),md->briefDescription());
        ol.newParagraph();
      }
      if (!md->documentation().isEmpty())
      { 
        parseDoc(ol,scopeName,md->name(),md->documentation()+"\n");
      }
      if (!md->bodyCode().isEmpty())
      {
        ol.startCodeFragment();
        parseCode(ol,scopeName,md->bodyCode(),FALSE,0);
        ol.endCodeFragment();
      }
      
      if (md->isEnumerate())
      {
        bool first=TRUE;
        MemberList *fmdl=md->enumFieldList();
        if (fmdl)
        {
          MemberDef *fmd=fmdl->first();
          while (fmd)
          {
            if (fmd->hasDocumentation())
            {
              if (first)
              {
                ol.newParagraph();
                ol.startBold();
                parseText(ol,theTranslator->trEnumerationValues());
                //ol.writeBoldString("Enumeration values:");
                ol.docify(":");
                ol.endBold();
                ol.startItemList();
              }
              ol.writeDoxyAnchor(cname,fmd->anchor(),fmd->name());
              ol.addToIndex(fmd->name(),cname);
              ol.addToIndex(cname,fmd->name());
              ol.writeListItem();
              first=FALSE;
              ol.startBold();
              ol.docify(fmd->name());
              ol.endBold();
              ol.newParagraph();

              if (!fmd->briefDescription().isEmpty())
              { 
                parseDoc(ol,scopeName,fmd->name(),fmd->briefDescription());
                ol.newParagraph();
              }
              if (!fmd->documentation().isEmpty())
              { 
                parseDoc(ol,scopeName,fmd->name(),fmd->documentation()+"\n");
              }
              ol.disable(OutputGenerator::Man);
              ol.newParagraph();
              ol.enable(OutputGenerator::Man);
            }
            fmd=fmdl->next();
          }
        }
        if (!first) { ol.endItemList(); ol.writeChar('\n'); }
      }
      
      MemberDef *bmd=md->reimplements();
      ClassDef  *bcd=0; 
      if (bmd && (bcd=bmd->memberClass()))
      {
        if (virt!=Normal) // search for virtual member of the deepest base class
        {
          MemberDef *lastBmd=bmd;
          while (lastBmd) 
          {
            ClassDef *lastBcd = lastBmd->memberClass();
            if (lastBmd->virtualness()!=Normal && 
                lastBmd->hasDocumentation() &&
                (lastBmd->protection()!=Private || Config::extractPrivateFlag) &&
                lastBcd->hasDocumentation() &&
                (lastBcd->protection()!=Private || Config::extractPrivateFlag)
               ) { bmd=lastBmd; bcd=lastBcd; }
            lastBmd=lastBmd->reimplements();
          }
        }
        // write class that contains a member that is reimplemented by this one
        if (bcd->hasDocumentation() || bcd->isReference())
        {
          ol.newParagraph();

          QCString reimplFromLine = theTranslator->trReimplementedFromList(1);
          int markerPos = reimplFromLine.find("@0");
          if (markerPos!=-1) // should always pass this.
          {
            parseText(ol,reimplFromLine.left(markerPos)); //text left from marker
            if (bmd->hasDocumentation() && 
                (bmd->protection()!=Private || Config::extractPrivateFlag)
               ) // replace marker with link
            {
              ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
                  bmd->anchor(),bcd->name());
              if (
                  !bcd->isReference() &&
                  //(bcd->hasDocumentation() || !Config::hideClassFlag) &&
                  //(bcd->protection()!=Private || Config::extractPrivateFlag)
                  bcd->isVisible() 
                  /*&& bmd->detailsAreVisible()*/
                 ) ol.writePageRef(bcd->name(),bmd->anchor());
            }
            else
            {
              ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
                  0,bcd->name());
              if (
                  !bcd->isReference() &&
                  //(bcd->hasDocumentation() || !Config::hideClassFlag) &&
                  //(bcd->protection()!=Private || Config::extractPrivateFlag)
                  bcd->isVisible()
                 ) ol.writePageRef(bcd->name(),0);
            }
            parseText(ol,reimplFromLine.right(
                  reimplFromLine.length()-markerPos-2)); // text right from marker

          }
          else
          {
            err("Error: translation error: no marker in trReimplementsFromList()\n");
          }
        }
        
        //ol.writeString(".");
      }
      MemberList *bml=md->reimplementedBy();
      if (bml)
      {
        MemberListIterator mli(*bml);
        MemberDef *bmd=0;
        uint count=0;
        ClassDef *bcd=0;
        for (mli.toFirst();(bmd=mli.current()) && (bcd=bmd->memberClass());++mli)
        {
          // count the members that directly inherit from md and for
          // which the member and class are visible in the docs.
          if (bmd->hasDocumentation() && 
              (bmd->protection()!=Private || Config::extractPrivateFlag) &&
              bcd->hasDocumentation() &&
              (bcd->protection()!=Private || Config::extractPrivateFlag)
             ) count++;
        }
        if (count>0)
        {
          mli.toFirst();
          // write the list of classes that overwrite this member
          ol.newParagraph();
          //parseText(ol,theTranslator->trReimplementedIn());
          //ol.writeString("Reimplemented in ");
          //ol.docify(" ");

          QCString reimplInLine = 
            theTranslator->trReimplementedInList(count);
          QRegExp marker("@[0-9]+");
          int index=0,newIndex,matchLen;
          // now replace all markers in reimplInLine with links to the classes
          while ((newIndex=marker.match(reimplInLine,index,&matchLen))!=-1)
          {
            parseText(ol,reimplInLine.mid(index,newIndex-index));
            bool ok;
            uint entryIndex = reimplInLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
            //bmd=bml->at(entryIndex);
            
            count=0;
            // find the entryIndex-th documented entry in the inheritance list.
            for (mli.toFirst();(bmd=mli.current()) && (bcd=bmd->memberClass());++mli)
            {
              if (bmd->hasDocumentation() && 
                  (bmd->protection()!=Private || Config::extractPrivateFlag) &&
                  bcd->hasDocumentation() &&
                  (bcd->protection()!=Private || Config::extractPrivateFlag)
                 ) 
              {
                if (count==entryIndex) break;
                count++;
              }
            }
            
            if (ok && bcd && bmd) // write link for marker
            {
              //if (bmd->hasDocumentation() &&
              //    (bmd->protection()!=Private || Config::extractPrivateFlag)
              //   )
              //{
              ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
                  bmd->anchor(),bcd->name());
              if (!bcd->isReference() && bcd->isVisible()) 
                ol.writePageRef(bcd->name(),bmd->anchor());
              //}
              //else
              //{
              //  ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
              //      0,bcd->name());
              //  if (!bcd->isReference() && bcd->isVisible()) 
              //    ol.writePageRef(bcd->name(),0);
              //}
            }
            ++mli;
            index=newIndex+matchLen;
          } 
          parseText(ol,reimplInLine.right(reimplInLine.length()-index));

        }
      }
      // write the list of examples that use this member
      if (md->hasExamples())
      {
        ol.startDescList();
        ol.startBold();
        parseText(ol,theTranslator->trExamples()+": ");
        //ol.writeBoldString("Examples: ");
        ol.endBold();
        ol.endDescTitle();
        ol.writeDescItem();
        writeExample(ol,md->getExampleList());
        //ol.endDescItem();
        ol.endDescList();
      }
      ol.endIndent();
      // enable LaTeX again
      if (Config::extractAllFlag && !hasDocs) ol.enable(OutputGenerator::Latex); 
                                          
    }
  }
}

//----------------------------------------------------------------------------
// read a file with `name' to a string.

QCString fileToString(const char *name)
{
  if (name==0 || name[0]==0) return 0;
  QFileInfo fi(name);
  if (!fi.exists() || !fi.isFile())
  {
    err("Error: file `%s' not found\n",name);
    exit(1);
  }
  QFile f(name);
  if (!f.open(IO_ReadOnly))
  {
    err("Error: cannot open file `%s'\n",name);
    exit(1);
  }
  int fsize=fi.size();
  QCString contents(fsize+1);
  f.readBlock(contents.data(),fsize);
  contents[fsize]='\0';
  f.close();
  return contents;
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

static int minClassDistance(ClassDef *cd,ClassDef *bcd,int level=0)
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
//    printf("t=`%s' n=`%s' v=`%s' ",a->type.data(),a->name.length()>0?a->name.data():"",a->defval.length()>0?a->defval.data():""); 
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

static QCString trimBaseClassScope(BaseClassList *bcl,const QCString &s)
{
  BaseClassListIterator bcli(*bcl);
  BaseClassDef *bcd;
  for (;(bcd=bcli.current());++bcli)
  {
    ClassDef *cd=bcd->classDef;
    int spos=s.find(cd->name());
    if (spos!=-1)
    {
      return s.left(spos)+s.right(
                      s.length()-spos-cd->name().length()-2
                     );
    }
    if (cd->baseClasses()->count()>0)
      trimBaseClassScope(cd->baseClasses(),s); 
  }
  return s;
}

//----------------------------------------------------------------------
// Matches the arguments list srcAl with the argument list dstAl
// Returns TRUE if the argument lists are equal. Two argument list are 
// considered equal if the number of arguments is equal and the types of all 
// arguments are equal. Furthermore the const and volatile specifiers 
// stored in the list should be equal.

bool matchArguments(ArgumentList *srcAl,ArgumentList *dstAl,
                    const char *cl,const char *ns)
{
  QCString className=cl;
  QCString namespaceName=ns;

  //printf("matchArguments(%s,%s) className=%s namespaceName=%s\n",
  //    srcAl ? argListToString(srcAl).data() : "",
  //    dstAl ? argListToString(dstAl).data() : "",
  //    cl,ns);

  if (srcAl==0 || dstAl==0)
  {
    return srcAl==dstAl; // at least one of the members is not a function
  }
  if ( srcAl->count()==0 && dstAl->count()==1 && 
       dstAl->getFirst()->type=="void" )
  { // special case for finding match between func() and func(void)
    Argument *a=new Argument;
    a->type = "void";
    srcAl->append(a);
    return TRUE;
  }
  if ( dstAl->count()==0 && srcAl->count()==1 &&
       srcAl->getFirst()->type=="void" )
  { // special case for finding match between func(void) and func()
    Argument *a=new Argument;
    a->type = "void";
    dstAl->append(a);
    return TRUE;
  }
  if (srcAl->count() != dstAl->count())
  {
    //printf("Different number of arguments!\n");
    return FALSE; // different number of arguments -> no match
  }
  if (srcAl->constSpecifier != dstAl->constSpecifier) 
  {
    return FALSE; // one member is const, the other not -> no match
  }
  if (srcAl->volatileSpecifier != dstAl->volatileSpecifier)
  {
    return FALSE; // one member is volatile, the other not -> no match
  }

  // so far the argument list could match, so we need to compare the types of
  // all arguments.
  ArgumentListIterator srcAli(*srcAl),dstAli(*dstAl);
  Argument *srcA,*dstA;
  for (;(srcA=srcAli.current(),dstA=dstAli.current());++srcAli,++dstAli)
  {
    QCString srcAType=trimTemplateSpecifiers(className,srcA->type);
    QCString dstAType=trimTemplateSpecifiers(className,dstA->type);
    
    if (srcAType!=dstAType) // check if the argument only differs on name 
    {
      //printf("scope=`%s': `%s' <=> `%s'\n",className.data(),srcAType.data(),dstAType.data());

      QCString srcScope;
      QCString dstScope;

      // strip redundant scope specifiers
      if (!className.isEmpty())
      {
        srcAType=trimScope(className,srcAType);
        dstAType=trimScope(className,dstAType);
        ClassDef *cd;
        if (!namespaceName.isEmpty())
          cd=getClass(namespaceName+"::"+className);
        else
          cd=getClass(className);
        if (cd && cd->baseClasses()->count()>0)
        {
          srcAType=trimBaseClassScope(cd->baseClasses(),srcAType); 
          dstAType=trimBaseClassScope(cd->baseClasses(),dstAType); 
        }
      }
      if (!namespaceName.isEmpty())
      {
        srcAType=trimScope(namespaceName,srcAType);
        dstAType=trimScope(namespaceName,dstAType);
      }
      
      //printf("`%s' <=> `%s'\n",srcAType.data(),dstAType.data());
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
        //if (srcPos==0 || isalnum(srcAType.at(srcPos-1)) ||
        //    dstPos==0 || isalnum(dstAType.at(dstPos-1))) { printf("No match1\n"); return FALSE; }
        int srcStart=srcPos;
        int dstStart=dstPos;
        if (srcPos==0 || dstPos==0) return FALSE;
        if (isId(srcAType.at(srcPos)) && isId(dstAType.at(dstPos)))
        {
          // check if a name if already found -> if no then there is no match
          if (srcA->name.length()>0 || dstA->name.length()>0) return FALSE;
          while (srcPos<srcAType.length() && isId(srcAType.at(srcPos))) srcPos++;
          while (dstPos<dstAType.length() && isId(dstAType.at(dstPos))) dstPos++;
          if (srcPos<srcAType.length() || dstPos<dstAType.length()) return FALSE;
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
          // otherwise we assume that a name starts at the current position.
          while (srcPos<srcAType.length() && isId(srcAType.at(srcPos))) srcPos++;
          while (dstPos<dstAType.length() && isId(dstAType.at(dstPos))) dstPos++;
          // if nothing more follows for both types then we assume we have
          // found a match. Note that now `signed int' and `signed' match, but
          // seeing that int is not a name can only be done by looking at the
          // semantics.

          if (srcPos!=srcAType.length() || dstPos!=dstAType.length()) { return FALSE; }
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
          while (dstPos<dstAType.length() && isId(dstAType.at(dstPos))) dstPos++;
          if (dstPos!=dstAType.length()) return FALSE; // more than a difference in name -> no match
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
          while (dstPos<dstAType.length() && isId(dstAType.at(dstPos))) dstPos++;
          if (dstPos!=dstAType.length()) return FALSE; // nope not a name -> no match
          else // its a name (most probably) so move it
          {
            dstA->name=dstAType.right(dstAType.length()-startPos);
            dstA->type=dstAType.left(startPos).stripWhiteSpace();
          }
        }
      }
      else if (srcPos<srcAType.length())
      {
        if (!isspace(srcAType.at(srcPos))) // maybe the names differ
        {
          int startPos=srcPos;
          while (srcPos<srcAType.length() && isId(srcAType.at(srcPos))) srcPos++;
          if (srcPos!=srcAType.length()) return FALSE; // more than a difference in name -> no match
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
          while (srcPos<srcAType.length() && isId(srcAType.at(srcPos))) srcPos++;
          if (srcPos!=srcAType.length()) return FALSE; // nope not a name -> no match
          else // its a name (most probably) so move it
          {
            srcA->name=srcAType.right(srcAType.length()-startPos);
            srcA->type=srcAType.left(startPos).stripWhiteSpace();
          }
        }
      }
      else // without scopes the names match exactly
      {
      }
    }
    else if (srcA->name.length()==0 && dstA->name.length()==0) 
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
  }
  //printf("Match found!\n");
  return TRUE; // all arguments match 
}

// merges the initializer of two argument lists
// pre:  the types of the arguments in the list should match.
void mergeArguments(ArgumentList *srcAl,ArgumentList *dstAl)
{
  //printf("mergeArguments `%s', `%s'\n",
  //    argListToString(srcAl).data(),argListToString(dstAl).data());
  //printArgList(srcAl);printf(" <=> ");
  //printArgList(dstAl);printf("\n");
  if (srcAl==0 || dstAl==0 || srcAl->count()!=dstAl->count())
  {
    return; // invalid argument lists -> do not merge
  }

  ArgumentListIterator srcAli(*srcAl),dstAli(*dstAl);
  Argument *srcA,*dstA;
  for (;(srcA=srcAli.current(),dstA=dstAli.current());++srcAli,++dstAli)
  {
    if (srcA->defval.length()==0 && dstA->defval.length()>0)
    {
      //printf("Defval changing `%s'->`%s'\n",srcA->defval.data(),dstA->defval.data());
      srcA->defval=dstA->defval.copy();
    }
    else if (srcA->defval.length()>0 && dstA->defval.length()==0)
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
             MemberDef *&md, ClassDef *&cd, FileDef *&fd,NamespaceDef *&nd)
{
  fd=0, md=0, cd=0, nd=0;
  if (memberName.length()==0) return FALSE; /* empty name => nothing to link */

  QCString scopeName=scName.copy();
  //printf("Search for name=%s args=%s in scope=%s\n",
  //          memberName.data(),args,scopeName.data());
  
  int is,im,pm=0;
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
  if ((im=memberName.findRev("::"))!=-1)
  {
    mScope=memberName.left(im); 
    mName=memberName.right(memberName.length()-im-2);
  }
  
  // handle special the case where both scope name and member scope are equal
  if (mScope==scopeName) scopeName.resize(0);

  //printf("mScope=`%s' mName=`%s'\n",mScope.data(),mName.data());
  
  MemberName *mn = memberNameDict[mName];
  if (mn && scopeName.length()+mScope.length()>0)
  {
    //printf("  >member name found\n");
    int scopeOffset=scopeName.length();
    do
    {
      QCString className = scopeName.left(scopeOffset);
      if (!className.isEmpty() && !mScope.isEmpty())
      {
        className.append("::"+mScope);
      }
      else if (!mScope.isEmpty())
      {
        className=mScope.copy();
      }
      //printf("Trying class scope %s\n",className.data());

      ClassDef *fcd=0;
      if ((fcd=getClass(className)) &&  // is it a documented class
            fcd->isVisibleExt() 
         )
      {
        //printf("  Found fcd=%p\n",fcd);
        MemberDef *mmd=mn->first();
        int mdist=maxInheritanceDepth; 
        while (mmd)
        {
          if ((mmd->protection()!=Private || Config::extractPrivateFlag) &&
              mmd->hasDocumentation() 
              /*mmd->detailsAreVisible()*/
              /* && (args==0 || matchArgumentsOld(mmd->argsString(),args)) */
             )
          {
            bool match=TRUE;
            ArgumentList *argList=0;
            if (args)
            {
              argList=new ArgumentList;
              stringToArgumentList(args,argList);
              match=matchArguments(mmd->argumentList(),argList); 
            }
            if (match)
            {
              ClassDef *mcd=mmd->memberClass();
              int m=minClassDistance(fcd,mcd);
              if (m<mdist && mcd->isVisible())
              {
                mdist=m;
                cd=mcd;
                md=mmd;
              }
            }
            if (argList)
            {
              delete argList;
            }
          }
          mmd=mn->next();
        }
        if (mdist==maxInheritanceDepth && !strcmp(args,"()"))
          // no exact match found, but if args="()" an arbitrary member will do
        {
          //printf("  >Searching for arbitrary member\n");
          mmd=mn->last();
          while (mmd)
          {
            if ((mmd->protection()!=Private || Config::extractPrivateFlag) &&
                (
                 mmd->hasDocumentation() 
                 /*mmd->detailsAreVisible()*/
                 || mmd->isReference()
                )
               )
            {
              ClassDef *mcd=mmd->memberClass();
              //printf("  >Class %s found\n",mcd->name().data());
              int m=minClassDistance(fcd,mcd);
              if (m<mdist && mcd->isVisible())
              {
                //printf("Class distance %d\n",m);
                mdist=m;
                cd=mcd;
                md=mmd;
              }
            }
            mmd=mn->prev();
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
  else // maybe an namespace or file member ?
  {
    //printf("Testing for global function scopeName=`%s' mScope=`%s' :: mName=`%s'\n",
    //              scopeName.data(),mScope.data(),mName.data());
    //printf("  >member name found\n");
    if ((mn=functionNameDict[mName])) // name is known
    {
      NamespaceDef *fnd=0;
      int scopeOffset=scopeName.length();
      do
      {
        QCString namespaceName = scopeName.left(scopeOffset);
        if (!namespaceName.isEmpty() && !mScope.isEmpty())
        {
          namespaceName.append("::"+mScope);
        }
        else if (!mScope.isEmpty())
        {
          namespaceName=mScope.copy();
        }
        if (namespaceName.length()>0 && 
            (fnd=namespaceDict[namespaceName]) &&
            fnd->isVisibleExt()
           )
        {
          //printf("Function inside existing namespace `%s'\n",namespaceName.data());
          bool found=FALSE;
          MemberDef *mmd=mn->first();
          while (mmd && !found)
          {
            //printf("mmd->getNamespace()=%p fnd=%p\n",
            //    mmd->getNamespace(),fnd);
            if (mmd->getNamespace()==fnd && 
                (mmd->isReference() || mmd->hasDocumentation())
               )
            { // namespace is found
              bool match=TRUE;
              ArgumentList *argList=0;
              if (args)
              {
                argList=new ArgumentList;
                stringToArgumentList(args,argList);
                match=matchArguments(mmd->argumentList(),argList); 
              }
              if (match)
              {
                nd=fnd;
                md=mmd;
                found=TRUE;
              }
              if (args)
              {
                delete argList;
              }
            }
            mmd=mn->next();
          }
          if (!found && !strcmp(args,"()")) 
            // no exact match found, but if args="()" an arbitrary 
            // member will do
          {
            MemberDef *mmd=mn->last(); // searching backward will get 
            // the first defined!
            while (mmd && !found)
            {
              if (mmd->getNamespace()==fnd && 
                  (mmd->isReference() || mmd->hasDocumentation())
                 )
              {
                nd=fnd;
                md=mmd;
                found=TRUE;
              }
              mmd=mn->prev();
            }
          }
          if (found) return TRUE;
        }
        else // no scope => global function
        {
          //printf("Function with global scope `%s'\n",namespaceName.data());
          md=mn->first();
          while (md)
          {
            if (md->isReference() || md->hasDocumentation())
            {
              //printf("md->name()=`%s'\n",md->name().data());
              fd=md->getFileDef();
              if (fd && (fd->isReference() || fd->hasDocumentation()))
              {
                //printf("fd->name()=`%s'\n",fd->name().data());
                bool match=TRUE;
                ArgumentList *argList=0;
                if (args)
                {
                  argList=new ArgumentList;
                  stringToArgumentList(args,argList);
                  match=matchArguments(md->argumentList(),argList); 
                  delete argList;
                }
                if (match) return TRUE;
              }
            }
            md=mn->next();
          }
          if (!strcmp(args,"()"))
          {
            // no exact match found, but if args="()" an arbitrary 
            // member will do
            md=mn->last();
            while (md)
            {
              if (md->isReference() || md->hasDocumentation())
              {
                //printf("md->name()=`%s'\n",md->name().data());
                fd=md->getFileDef();
                if (fd && (fd->isReference() || fd->hasDocumentation()))
                {
                  return TRUE;
                }
              }
              md=mn->prev();
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
    }
    else
    {
      //printf("Unknown function `%s'\n",mName.data());
    }
  }
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
  if (scopeName.length()==0) return FALSE;

  QCString docScopeName=docScope;
  int scopeOffset=docScopeName.length();

  do // for each possible docScope (from largest to and including empty)
  {
    QCString fullName=scopeName.copy();
    if (scopeOffset>0) fullName.prepend(docScopeName.left(scopeOffset)+"::");
    
    if ((cd=getClass(fullName)) && cd->isVisibleExt())
    {
      return TRUE; // class link written => quit 
    }
    else if ((nd=namespaceDict[fullName]) && nd->isVisibleExt())
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
 *    4) "::memberName     a non-function member or define
 *    5) ("ScopeName::")+"memberName()" 
 *    6) ("ScopeName::")+"memberName(...)" 
 *    7) ("ScopeName::")+"memberName" 
 * instead of :: the # symbol may also be used.
 */

void generateRef(OutputList &ol,const char *scName,
                 const char *name,bool inSeeBlock,const char *rt)
{
  //printf("generateRef(scName=%s,name=%s,rt=%s)\n",scName,name,rt);
  
  QCString tmpName = substitute(name,"#","::");
  QCString linkText = rt;
  int scopePos=tmpName.findRev("::");
  int bracePos=tmpName.findRev('('); // reverse is needed for operator()(...)
  if (bracePos==-1) // simple name
  {
    ClassDef *cd=0;
    NamespaceDef *nd=0;
    if (linkText.isNull()) linkText=tmpName;
    // check if this is a class or namespace reference
    if (scName!=tmpName && getScopeDefs(scName,name,cd,nd))
    {
      if (cd) // scope matches that of a class
      {
        ol.writeObjectLink(cd->getReference(),
            cd->getOutputFileBase(),0,linkText);
        if (!cd->isReference()) 
          ol.writePageRef(cd->name(),0);
      }
      else // scope matches that of a namespace
      {
        ol.writeObjectLink(nd->getReference(),
            nd->getOutputFileBase(),0,linkText);
        if (!nd->getReference()) 
          ol.writePageRef(nd->name(),0);
      }
      // link has been written, stop now.
      return;
    }
    else if (scName==tmpName || (!inSeeBlock && scopePos==-1)) // nothing to link => output plain text
    {
      ol.docify(linkText);
      // text has been written, stop now.
      return;
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
  bool explicitLink=TRUE;
  if (linkText.isNull())
  {
    //if (!scopeUser.isEmpty()) linkText=scopeUser+"::";
    linkText=nameStr;
    if (linkText.left(2)=="::") linkText=linkText.right(linkText.length()-2);
    explicitLink=FALSE;
  } 
  //printf("scope=`%s' name=`%s' arg=`%s' linkText=`%s'\n",
  //       scopeStr.data(),nameStr.data(),argsStr.data(),linkText.data());

  MemberDef *md    = 0;
  ClassDef *cd     = 0;
  FileDef *fd      = 0;
  NamespaceDef *nd = 0;

  //printf("Try with scName=`%s' nameStr=`%s' argsStr=`%s'\n",
  //        scopeStr.data(),nameStr.data(),argsStr.data());

  // check if nameStr is a member or global.
  if (getDefs(scopeStr,nameStr,argsStr,md,cd,fd,nd))
  {
    //printf("after getDefs nd=%p\n",nd);
    QCString anchor = (md->isReference() || md->hasDocumentation()) ? md->anchor() : 0;
    QCString cName,aName;
    if (cd) // nameStr is a member of cd
    {
      //printf("addObjectLink(%s,%s,%s,%s)\n",cd->getReference(),
      //      cd->getOutputFileBase(),anchor.data(),resultName.stripWhiteSpace().data());
      ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),
          anchor,linkText.stripWhiteSpace());
      cName=cd->name();
      aName=md->anchor();
    }
    else if (nd) // nameStr is a member of nd
    {
      //printf("writing namespace link\n");
      ol.writeObjectLink(nd->getReference(),nd->getOutputFileBase(),
          anchor,linkText.stripWhiteSpace());
      cName=nd->name();
      aName=md->anchor();
    }
    else if (fd) // nameStr is a global in file fd
    {
      //printf("addFileLink(%s,%s,%s)\n",fd->getOutputFileBase(),anchor.data(),
      //        resultName.stripWhiteSpace().data());
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),
          anchor,linkText.stripWhiteSpace());
      cName=fd->name();
      aName=md->anchor();
    }
    else // should not be reached
    {
      //printf("add no link fd=cd=0\n");
      ol.docify(linkText);
    }

    // for functions we add the arguments if explicitly specified or else "()"
    if (!rt && (md->isFunction() || md->isPrototype() || md->isSignal() || md->isSlot())) 
    {
      if (argsStr.isNull())
        ol.writeString("()");
      else
        ol.docify(argsStr);
    }

    // generate the page reference (for LaTeX)
    if (cName.length()>0 || aName.length()>0)
    {
      if (
          (cd && !cd->isReference() && cd->isVisible()) || 
          (fd && !fd->isReference()) ||
          (nd /* TODO: && !nd->isReference() */)
         ) 
      {
        ol.writePageRef(cName,aName);
      }
    }
    return;
  }

  // nothing found
  if (rt) 
    ol.docify(rt); 
  else 
  {
    ol.docify(linkText);
    if (!argsStr.isNull()) ol.docify(argsStr);
  }
  return;
}

//----------------------------------------------------------------------
// General function that generates the HTML code for a reference to some
// file, class or member from text `lr' within the context of class `clName'. 
// This link has the text 'lt' (if not 0), otherwise `lr' is used as a
// basis for the link's text.

void generateLink(OutputList &ol,const char *clName,
                     const char *lr,bool inSeeBlock,const char *lt)
{
  QCString linkRef=lr;
  //PageInfo *pi=0;
  //printf("generateLink(%s,%s,%s) inSeeBlock=%d\n",clName,lr,lt,inSeeBlock);
  //FileInfo *fi=0;
  FileDef *fd;
  bool ambig;
  if (linkRef.length()==0) // no reference name!
    ol.docify(lt);
  else if ((pageDict[linkRef])) // link to a page
    ol.writeObjectLink(0,linkRef,0,lt);  
  else if ((exampleDict[linkRef])) // link to an example
    ol.writeObjectLink(0,linkRef+"-example",0,lt);
  else if ((fd=findFileDef(&inputNameDict,linkRef,ambig))
       && fd->hasDocumentation())
        // link to documented input file
    ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,lt);
  else // probably a class or member reference
    generateRef(ol,clName,lr,inSeeBlock,lt);
}

void generateFileRef(OutputList &ol,const char *name,const char *text)
{
  QCString linkText = text ? text : name;
  //FileInfo *fi;
  FileDef *fd;
  bool ambig;
  if ((fd=findFileDef(&inputNameDict,name,ambig)) && 
      fd->hasDocumentation()) 
    // link to documented input file
    ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),0,linkText);
  else
    ol.docify(linkText); 
}

//----------------------------------------------------------------------

QCString substituteClassNames(const QCString &s)
{
  int i=0,l,p;
  QCString result;
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

//----------------------------------------------------------------------

QCString convertSlashes(const QCString &s,bool dots)
{
  QCString result;
  int i,l=s.length();
  for (i=0;i<l;i++)
    if (s.at(i)!='/' && (!dots || s.at(i)!='.'))
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
      result+="_";
  return result;
}

//----------------------------------------------------------------------
// substitute all occurences of `src' in `s' by `dst'

QCString substitute(const char *s,const char *src,const char *dst)
{
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
  if (name.isNull()) return 0;
  int slashPos=QMAX(name.findRev('/'),name.findRev('\\'));
  if (slashPos!=-1)
  {
    path=name.left(slashPos+1);
    name=name.right(name.length()-slashPos-1); 
  }
  //printf("findFileDef path=`%s' name=`%s'\n",path.data(),name.data());
  if (name.isNull()) return 0;
  FileName *fn;
  if ((fn=(*fnDict)[name]))
  {
    if (fn->count()==1)
    {
      return fn->first();
    }
    else // file name alone is ambigious
    {
      int count=0;
      FileDef *fd=fn->first();
      FileDef *lastMatch=0;
      while (fd)
      {
        if (path.isNull() || fd->getPath().right(path.length())==path) 
        { 
          count++; 
          lastMatch=fd; 
        }
        fd=fn->next();
      }
      ambig=(count>1);
      return lastMatch;
    }
  }
  return 0;
}

//----------------------------------------------------------------------

void showFileDefMatches(const FileNameDict *fnDict,const char *n)
{
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
    FileDef *fd=fn->first();
    while (fd)
    {
      if (path.isNull() || fd->getPath().right(path.length())==path)
      {
        msg("   %s\n",fd->absFilePath().data());
      }
      fd=fn->next();
    }
  }
}

//----------------------------------------------------------------------

void setFileNameForSections(QList<QCString> *anchorList,const char *fileName)
{
  if (!anchorList) return;
  QCString *s=anchorList->first();
  while (s)
  {
    SectionInfo *si;
    if (!s->isEmpty() && (si=sectionDict[*s])) si->fileName=fileName;
    s=anchorList->next();
  }
}

