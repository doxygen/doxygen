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

#include <qtstream.h>
#include <qdatetm.h>
#include <qdir.h>
#include <stdlib.h>

#include "message.h"
#include "index.h"
#include "doxygen.h"
#include "scanner.h"
#include "code.h"
#include "config.h"
#include "filedef.h"
#include "outputlist.h"
#include "util.h"
#include "groupdef.h"
#include "language.h"
#include "htmlhelp.h"

//----------------------------------------------------------------------------

// strips w from s iff s starts with w
bool stripWord(QCString &s,QCString w)
{
  bool success=FALSE;
  if (s.left(w.length())==w) 
  {
    success=TRUE;
    s=s.right(s.length()-w.length());
  }
  return success;
}

//----------------------------------------------------------------------------
// some quasi intelligent brief description abbreviator :^)
QCString abbreviate(const char *s,const char *name)
{
  QCString result=s;
  QCString start1=(QCString)"The "+name+" class ";
  QCString start2=(QCString)"The "+name+" widget ";
  QCString start3=(QCString)"The "+name+" file ";
  result=result.simplifyWhiteSpace();
  // strip trailing .
  if (result.length()>0 && result.at(result.length()-1)=='.') 
    result=result.left(result.length()-1);
  bool found=FALSE;
  found = found || stripWord(result,start1);
  found = found || stripWord(result,start2);
  found = found || stripWord(result,start3);
  if (found)
  {
    bool found=FALSE;
    found = found || stripWord(result,"is ");
    found = found || stripWord(result,"provides ");
    found = found || stripWord(result,"specifies ");
    found = found || stripWord(result,"contains ");
    found = found || stripWord(result,"represents ");
    if (found)
    {
      stripWord(result,"a ");
      stripWord(result,"an ");
      stripWord(result,"the ");
    }
  }
  if (result.length()>0)
  {
    int c=result[0];
    if (c>='a' && c<='z') c+='A'-'a';
    result[0]=c;
  }
  return result;
}

//----------------------------------------------------------------------------

void initBaseClassHierarchy(BaseClassList *bcl)
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

//----------------------------------------------------------------------------

void writeClassTree(OutputList &ol,BaseClassList *bcl,bool hideSuper)
{
  HtmlHelp *htmlHelp=0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp = HtmlHelp::getInstance();
  }
  BaseClassListIterator bcli(*bcl);
  bool started=FALSE;
  for ( ; bcli.current() ; ++bcli)
  {
    ClassDef *cd=bcli.current()->classDef;
    if (cd->isVisibleInHierarchy() && hasVisibleRoot(cd->baseClasses()))
    {
      if (!started)
      {
        ol.startIndexList();
        if (hasHtmlHelp) htmlHelp->incContentsDepth();
        started=TRUE;
      }
      //printf("Passed...\n");
      if (cd->isLinkable())
      {
        ol.writeIndexItem(cd->getReference(),cd->getOutputFileBase(),cd->name());
        if (cd->isReference()) 
        { 
          ol.startTypewriter(); 
          ol.docify(" [external]");
          ol.endTypewriter();
        }
        if (hasHtmlHelp)
        {
          htmlHelp->addContentsItem(cd->name(),cd->getOutputFileBase());
          //cd->writeMembersToContents();
        }
      }
      else
      {
        ol.writeIndexItem(0,0,cd->name());
        if (hasHtmlHelp)
        {
          htmlHelp->addContentsItem(cd->name(),"nodoc");
        }
      }
      if (!cd->visited && !hideSuper && cd->superClasses()->count()>0)
      {
        writeClassTree(ol,cd->superClasses(),cd->visited);
        cd->visited=TRUE;
      }
    }
  }
  if (started) 
  {
    ol.endIndexList(); 
    if (hasHtmlHelp) htmlHelp->decContentsDepth();
  }
}

//----------------------------------------------------------------------------

void writeClassHierarchy(OutputList &ol)
{
  initClassHierarchy(&classList);

  HtmlHelp *htmlHelp=0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp = HtmlHelp::getInstance();
  }

  bool started=FALSE;
  ClassListIterator cli(classList);
  for (;cli.current(); ++cli)
  {
    ClassDef *cd=cli.current();
    if (!hasVisibleRoot(cd->baseClasses()))
    {
      if (cd->isVisibleInHierarchy()) // WAS: isVisible()!
      {
        if (!started)
        {
          ol.startIndexList();
          if (hasHtmlHelp) htmlHelp->incContentsDepth();
          started=TRUE;
        }
        if (cd->isLinkable())
        {
          ol.writeIndexItem(cd->getReference(),cd->getOutputFileBase(),cd->name());
          if (cd->isReference()) 
          {
            ol.startTypewriter(); 
            ol.docify(" [external]");
            ol.endTypewriter();
          }
          if (hasHtmlHelp)
          {
            htmlHelp->addContentsItem(cd->name(),cd->getOutputFileBase());
            //cd->writeMembersToContents();
          }
        }
        else
        {
          ol.writeIndexItem(0,0,cd->name());
          if (hasHtmlHelp) htmlHelp->addContentsItem(cd->name(),"nodoc");
        }
      }
      if (!cd->visited && cd->superClasses()->count()>0) 
      {
        writeClassTree(ol,cd->superClasses(),cd->visited);
        cd->visited=TRUE;
      }
    }
  }
  if (started) 
  {
    ol.endIndexList();
    if (hasHtmlHelp) htmlHelp->decContentsDepth();
  }
}

//----------------------------------------------------------------------------

// TODO: let this function return the real number of items in the hierarchy.
int countClassHierarchy()
{
  initClassHierarchy(&classList);
  int count=0;
  ClassListIterator cli(classList);
  for ( ; cli.current(); ++cli)
  {
    if (cli.current()->superClasses()->count()>0) count++;
  }
  return count;
}

//----------------------------------------------------------------------------

void writeHierarchicalIndex(OutputList &ol)
{
  if (hierarchyClasses==0) return;
  ol.disable(OutputGenerator::Man);
  startFile(ol,"hierarchy","Hierarchical Index");
  startTitle(ol);
  QCString title = Config::projectName+" "+theTranslator->trClassHierarchy();
  parseText(ol,title);
  endTitle(ol,0);
  HtmlHelp *htmlHelp = 0;
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"hierarchy"); 
  }
  parseText(ol,theTranslator->trClassHierarchyDescription());
  ol.newParagraph();
  writeClassHierarchy(ol);
  endFile(ol);
  ol.enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------

int countFiles()
{
  int count=0;
  FileNameListIterator fnli(inputNameList);
  FileName *fn;
  for (;(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      if (fd->isLinkable()) count++;
    }
  }
  return count;
}

//----------------------------------------------------------------------------

void writeFileIndex(OutputList &ol)
{
  if (documentedFiles==0) return;
  ol.disable(OutputGenerator::Man);
  startFile(ol,"files","File Index");
  startTitle(ol);
  QCString title = Config::projectName+" "+theTranslator->trFileList();
  parseText(ol,title);
  endTitle(ol,0);
  HtmlHelp *htmlHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"files"); 
    htmlHelp->incContentsDepth();
  }
  ol.newParagraph();
  parseText(ol,theTranslator->trFileListDescription(Config::extractAllFlag));
  ol.newParagraph();

  ol.startIndexList();
  FileName *fn=inputNameList.first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      if (fd->isLinkableInProject())
      {
        //ol.writeIndexItem(fd->getReference(),fd->diskName(),
        //    fd->name());
        QCString path;
        if (Config::fullPathNameFlag) 
        {
          path=stripFromPath(fd->getPath().copy());
        }

        ol.writeStartAnnoItem("file",
                              fd->getOutputFileBase(),
                              path,
                              fd->name()
                             );
        ol.docify(" (");
        if (fd->briefDescription())
        {
          OutputList briefOutput(&ol);
          parseDoc(briefOutput,0,0,
              abbreviate(fd->briefDescription(),fd->name()));
          ol+=briefOutput;
        }
        else
        {
          ol.startEmphasis();
          parseText(ol,theTranslator->trNoDescriptionAvailable());
          ol.endEmphasis();
        }
        ol.docify(")");
        ol.writeEndAnnoItem(fd->name());
        if (hasHtmlHelp)
        {
          htmlHelp->addContentsItem(fd->name(),fd->getOutputFileBase());
        }
      }
      fd=fn->next();
    }
    fn=inputNameList.next();
  }
  ol.endIndexList();
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  endFile(ol);
  ol.enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------
int countNamespaces()
{
  int count=0;
  NamespaceListIterator nli(namespaceList);
  NamespaceDef *nd;
  for (;(nd=nli.current());++nli)
  {
    if (nd->isLinkableInProject()) count++;
  }
  return count;
}

//----------------------------------------------------------------------------

void writeNamespaceIndex(OutputList &ol)
{
  if (documentedNamespaces==0) return;
  ol.disable(OutputGenerator::Man);
  startFile(ol,"namespaces","Namespace Index");
  startTitle(ol);
  QCString title = Config::projectName+" "+theTranslator->trNamespaceList();
  parseText(ol,title);
  endTitle(ol,0);
  HtmlHelp *htmlHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"namespaces"); 
    htmlHelp->incContentsDepth();
  }
  ol.newParagraph();
  parseText(ol,theTranslator->trNamespaceListDescription(Config::extractAllFlag));
  ol.newParagraph();

  ol.startIndexList();
  NamespaceDef *nd=namespaceList.first();
  while (nd)
  {
    if (nd->isLinkableInProject())
    {
      ol.writeStartAnnoItem("namespace",nd->getOutputFileBase(),0,nd->name());
      ol.docify(" (");
      if (nd->briefDescription())
      {
        OutputList briefOutput(&ol);
        parseDoc(briefOutput,nd->name(),0,
                      abbreviate(nd->briefDescription(),nd->name()));
        ol+=briefOutput;
      }
      else
      {
        ol.startEmphasis();
        parseText(ol,theTranslator->trNoDescriptionAvailable());
        ol.endEmphasis();
      }
      ol.docify(")");
      ol.writeEndAnnoItem(nd->name());
      if (hasHtmlHelp)
      {
        htmlHelp->addContentsItem(nd->name(),nd->getOutputFileBase());
      }
    }
    nd=namespaceList.next();
  }
  ol.endIndexList();
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  endFile(ol);
  ol.enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------

int countAnnotatedClasses()
{
  int count=0;
  //ClassDef *cd=classList.first();
  ClassListIterator cli(classList);
  ClassDef *cd;
  for (;(cd=cli.current());++cli)
  {
    if (cd->isLinkableInProject()) 
    { 
      //printf("Annotated class %s\n",cd->name().data()); 
      count++; 
    }
  }
  return count;
}

//----------------------------------------------------------------------

void writeAnnotatedClassList(OutputList &ol)
{
  ol.startIndexList(); 
  //ClassDef *cd=classList.first();
  //while (cd)
  ClassListIterator cli(classList);
  ClassDef *cd;
  for (;(cd=cli.current());++cli)
  {
    if (cd->isLinkableInProject())
    {
      QCString type;
      switch (cd->compoundType())
      {
        case ClassDef::Class:      type="class";     break;
        case ClassDef::Struct:     type="struct";    break;
        case ClassDef::Union:      type="union";     break;
        default:                   type="interface"; break;
      }
      ol.writeStartAnnoItem(type,cd->getOutputFileBase(),0,cd->name());
      ol.docify(" (");
      if (cd->briefDescription())
      {
        OutputList briefOutput(&ol);
        parseDoc(briefOutput,cd->name(),0,
                      abbreviate(cd->briefDescription(),cd->name()));
        ol+=briefOutput;
      }
      else
      {
        ol.startEmphasis();
        parseText(ol,theTranslator->trNoDescriptionAvailable());
        ol.endEmphasis();
      }
      ol.docify(")");
      ol.writeEndAnnoItem(cd->name());
      if (Config::generateHtml && Config::htmlHelpFlag)
      {
        HtmlHelp::getInstance()->addContentsItem(
            cd->name(),cd->getOutputFileBase());
        //cd->writeMembersToContents();
      }
    }
    cd=classList.next(); 
  }
  ol.endIndexList();
}

//----------------------------------------------------------------------------

void writeAlfabeticalClassList(OutputList &ol)
{
  ol.startAlfabeticalIndexList(); 
  //ClassDef *cd=classList.first();
  //while (cd)
  ClassListIterator cli(classList);
  ClassDef *cd;
  char startLetter=0;
  for (;(cd=cli.current());++cli)
  {
    if (cd->isLinkableInProject())
    {
      if (cd->name().at(0)!=startLetter)
      {
        startLetter=cd->name().at(0);
        char s[2]; s[0]=startLetter; s[1]=0;
        ol.writeIndexHeading(s);
      }
      ol.writeObjectLink(cd->getReference(),
                         cd->getOutputFileBase(),0,cd->name());
      ol.lineBreak();
    }
    cd=classList.next(); 
  }
  ol.endAlfabeticalIndexList();
}

//----------------------------------------------------------------------------

void writeAlphabeticalIndex(OutputList &ol)
{
  ol.disableAllBut(OutputGenerator::Html);
  if (annotatedClasses==0) return;
  startFile(ol,"classes.html","Alfabetical index");
  startTitle(ol);
  parseText(ol,Config::projectName+" "+theTranslator->trCompoundIndex());
  endTitle(ol,0);
  writeAlfabeticalClassList(ol);
  endFile(ol);
  ol.enableAll();
}

//----------------------------------------------------------------------------

void writeAnnotatedIndex(OutputList &ol)
{
  if (annotatedClasses==0) return;
  
  //if (classList.count()==0) return;
  ol.disable(OutputGenerator::Man);
  startFile(ol,"annotated","Annotated Index");
  startTitle(ol);
  QCString title = Config::projectName+" "+theTranslator->trCompoundList();
  parseText(ol,title);
  endTitle(ol,0);
  HtmlHelp *htmlHelp = 0;
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"annotated"); 
    htmlHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trCompoundListDescription());
  writeAnnotatedClassList(ol);
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp->decContentsDepth();
  }
  
  endFile(ol);
  ol.enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------

void writeMemberList(OutputList &ol)
{
  ol.startItemList();
  MemberName *mn=memberNameList.first();
  while (mn)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    bool isFunc=FALSE;
    while (md && !found)
    {
      ClassDef *cd;
      if (//(md->isFriend() || md->protection()!=Private || Config::extractPrivateFlag) && 
          //!md->isReference() && md->hasDocumentation() &&
          // md->name()[0]!='@' && 
           md->isLinkableInProject() &&
           (cd=md->memberClass()) &&
           cd->isLinkableInProject()
         ) 
      { 
        found=TRUE; 
        isFunc=md->isFunction(); 
      }
      md=mn->next();
    }
    if (found)
    {
      ol.writeListItem();
      ol.docify(substituteClassNames(mn->memberName()));
      if (isFunc) ol.docify("()");
      ol.writeString("\n");

      int count=0;
      md=mn->last();
      QCString prevName;
      while (md)
      {
        ClassDef *cd=md->memberClass();
        if (//cd && (md->isFriend() || md->protection()!=Private || Config::extractPrivateFlag) && 
            //!md->isReference() && md->hasDocumentation() && 
            md->isLinkableInProject() &&
            prevName!=cd->name() && 
            cd->isLinkableInProject()
           )
        {
          if (count==0) 
            ol.docify(": ");
          else 
            ol.docify(", ");
          ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),md->anchor(),
                            cd->name());
          count++;
          prevName=cd->name();
        }
        md=mn->prev();
      }
    }
    mn=memberNameList.next();
  }
  ol.endItemList();
}

//----------------------------------------------------------------------------

int countClassMembers()
{
  int count=0;
  MemberName *mn=memberNameList.first();
  while (mn)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    MemberDef *otherMd=0;
    ClassDef *cd;
    while (md && !found)
    {
      if (//(md->isFriend() || md->protection()!=Private || Config::extractPrivateFlag) && 
          //!md->isReference() && !md->isRelated() && md->hasDocumentation() &&
          //md->name()[0]!='@' && (cd=md->memberClass()) && cd->isLinkable()) 
          md->isLinkableInProject() && 
          !md->isRelated() &&
          (cd=md->memberClass()) && 
          cd->isLinkableInProject()
         )
      {
        if (!md->isRelated())
          otherMd=md;
        if (md->isRelated())
          found=TRUE;
      }
         
      //  otherMd=md;
      //if (//(md->isFriend() || md->protection()!=Private || Config::extractPrivateFlag) && 
      //    //!md->isReference() && md->isRelated() && md->hasDocumentation() &&
      //    //md->name()[0]!='@' && (cd=md->memberClass()) && cd->isLinkable()
      //    md->isLinkableInProject() &&
      //    md->isRelated() &&
      //    (cd=md->memberClass()) &&
      //    cd->isLinkableInProject()
      //   )
      //  found=TRUE;
      md=mn->next();
    }
    if (found || otherMd) count++;
    mn=memberNameList.next();
  }
  return count;
}

//----------------------------------------------------------------------------

void writeMemberIndex(OutputList &ol)
{
  if (memberNameList.count()==0) return;
  ol.disable(OutputGenerator::Man);
  ol.disable(OutputGenerator::Latex);
  startFile(ol,"functions","Compound Member Index");
  startTitle(ol);
  parseText(ol,Config::projectName+" "+theTranslator->trCompoundMembers());
  endTitle(ol,0);
  parseText(ol,theTranslator->trCompoundMembersDescription(Config::extractAllFlag));
  writeMemberList(ol);
  endFile(ol);
  ol.enable(OutputGenerator::Latex);
  ol.enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------

void writeFileMemberList(OutputList &ol)
{
  ol.startItemList();
  MemberName *mn=functionNameList.first();
  while (mn)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    while (md && !found)
    {
      FileDef *fd=md->getFileDef() ? md->getFileDef() : md->getFileDec();
      bool hasDocs = 
         (md->getFileDef() && md->getFileDef()->isLinkableInProject()) ||
         (md->getFileDec() && md->getFileDec()->isLinkableInProject());
      
      if (fd && hasDocs && 
          md->isLinkableInProject()
          //!md->isReference() && 
          //md->hasDocumentation() && 
          //md->name()[0]!='@'
         ) found=TRUE;
      else
        md=mn->next();
    }
    if (found) // function is documented
    {
      ol.writeListItem();
      ol.docify(md->name());
      if (md->isFunction()) ol.docify("()");
      ol.writeString("\n");

      int count=0;
      md=mn->last();
      QCString prevName;
      while (md)
      {
        FileDef *fd=md->getFileDef() ? md->getFileDef() : md->getFileDec();
        bool hasDocs = 
         (md->getFileDef() && md->getFileDef()->isLinkableInProject()) ||
         (md->getFileDec() && md->getFileDec()->isLinkableInProject());
        if (fd && hasDocs && 
            md->isLinkableInProject() &&
            //!md->isReference() && 
            //md->hasDocumentation() && 
            //md->name()[0]!='@' && 
            prevName!=fd->name())
        {
          if (count==0) 
            ol.docify(": ");
          else 
            ol.docify(", ");
          QCString baseName=fd->name().copy();
          //int s;
          //if ((s=baseName.findRev("/"))!=-1) 
          //  baseName=baseName.right(baseName.length()-s-1);
          ol.writeObjectLink(fd->getReference(),
              fd->getOutputFileBase(),md->anchor(), baseName);
          count++;
          prevName=fd->name();
        }
        md=mn->prev();
      }
    }
    mn=functionNameList.next();
  }
  ol.endItemList();
}

//----------------------------------------------------------------------------

void writeNamespaceMemberList(OutputList &ol)
{
  ol.startItemList();
  MemberName *mn=functionNameList.first();
  while (mn)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    while (md && !found)
    {
      NamespaceDef *nd=md->getNamespace();
      if (nd && nd->isLinkableInProject() && md->isLinkableInProject()) 
        found=TRUE;
      else
        md=mn->next();
    }
    if (found) // member is documented and in a documented namespace
    {
      ol.writeListItem();
      ol.docify(md->name());
      if (md->isFunction()) ol.docify("()");
      ol.writeString("\n");

      int count=0;
      md=mn->last();
      QCString prevName;
      while (md)
      {
        NamespaceDef *nd=md->getNamespace();
        if (nd && nd->isLinkableInProject() && md->isLinkableInProject() &&
            prevName!=nd->name()
           )
        {
          if (count==0) 
            ol.docify(": ");
          else 
            ol.docify(", ");
          ol.writeObjectLink(nd->getReference(),nd->getOutputFileBase(),
                             md->anchor(),nd->name());
          count++;
          prevName=nd->name();
        }
        md=mn->prev();
      }
    }
    mn=functionNameList.next();
  }
  ol.endItemList();
}

//----------------------------------------------------------------------------

int countNamespaceMembers()
{
  int count=0;
  MemberName *mn=functionNameList.first();
  while (mn)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    while (md && !found)
    {
      NamespaceDef *nd=md->getNamespace();
      if (nd && nd->isLinkableInProject() && md->isLinkableInProject())
        found=TRUE;
      else
        md=mn->next();
    }
    if (found) count++;
    mn=functionNameList.next();
  }
  return count;
}

//----------------------------------------------------------------------------

int countFileMembers()
{
  int count=0;
  MemberName *mn=functionNameList.first();
  while (mn)
  {
    MemberDef *md=mn->first();
    FileDef *fd;
    bool found=FALSE;
    while (md && !found)
    {
      if (md->isLinkableInProject() &&
          (((fd=md->getFileDef()) && fd->isLinkableInProject())
            || 
           ((fd=md->getFileDec()) && fd->isLinkableInProject())
          )
         ) 
        found=TRUE;
      else
        md=mn->next();
    }
    if (found) count++;
    mn=functionNameList.next();
  }
  return count;
}

//----------------------------------------------------------------------------

void writeFileMemberIndex(OutputList &ol)
{
  if (documentedFunctions==0) return;
  ol.disable(OutputGenerator::Man);
  ol.disable(OutputGenerator::Latex);
  startFile(ol,"globals","File Member Index");
  startTitle(ol);
  parseText(ol,Config::projectName+" "+theTranslator->trFileMembers());
  endTitle(ol,0);
  parseText(ol,theTranslator->trFileMembersDescription(Config::extractAllFlag));
  writeFileMemberList(ol);
  endFile(ol);
  ol.enable(OutputGenerator::Latex);
  ol.enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------

void writeNamespaceMemberIndex(OutputList &ol)
{
  if (documentedNamespaceMembers==0) return;
  ol.disable(OutputGenerator::Man);
  ol.disable(OutputGenerator::Latex);
  startFile(ol,"namespacemembers","Namespace Member Index");
  startTitle(ol);
  parseText(ol,Config::projectName+" "+theTranslator->trNamespaceMembers());
  endTitle(ol,0);
  parseText(ol,theTranslator->trNamespaceMemberDescription(Config::extractAllFlag));
  writeNamespaceMemberList(ol);
  endFile(ol);
  ol.enable(OutputGenerator::Latex);
  ol.enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------

int countIncludeFiles()
{
  int count=0;
  FileDef *fd=includeFiles.first();
  while (fd)
  {
    if (fd->isLinkableInProject())
    {
      count++;
    }
    fd=includeFiles.next();
  }  
  return count;
}

void writeHeaderFileList(OutputList &ol)
{
  bool started=FALSE;
  FileDef *fd=includeFiles.first();
  while (fd)
  {
    if (fd->isLinkableInProject())
    {
      if (!started)
      {
        started=TRUE;
        ol.startItemList();
      }
      ol.writeListItem();
      QCString path;
      if (Config::fullPathNameFlag) 
      {
        path=stripFromPath(fd->getPath().copy());
      }
      if (!path.isEmpty()) ol.docify(path);
      ol.writeObjectLink(0,fd->includeName(),0,fd->name());
      ol.writeString("\n");
      if (Config::generateHtml && Config::htmlHelpFlag)
      {
        HtmlHelp::getInstance()->addContentsItem(
            fd->name(),fd->includeName());
      }
    }
    fd=includeFiles.next();
  }  
  if (started) ol.endItemList();
}

//----------------------------------------------------------------------------

void writeHeaderIndex(OutputList &ol)
{
  if (documentedIncludeFiles==0) return;
  ol.disable(OutputGenerator::Man);
  ol.disable(OutputGenerator::Latex);
  startFile(ol,"headers","Header File Index");
  startTitle(ol);
  QCString title = Config::projectName+" "+theTranslator->trHeaderFiles();
  parseText(ol,title);
  endTitle(ol,0);
  HtmlHelp *htmlHelp = 0;
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"headers"); 
    htmlHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trHeaderFilesDescription());
  writeHeaderFileList(ol);
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp->decContentsDepth();
  }
  endFile(ol);
  ol.enable(OutputGenerator::Latex);
  ol.enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------

void writeExampleIndex(OutputList &ol)
{
  if (exampleList.count()==0) return;
  ol.disable(OutputGenerator::Man);
  startFile(ol,"examples","Example Index");
  startTitle(ol);
  QCString title = Config::projectName+" "+theTranslator->trExamples();
  parseText(ol,title);
  endTitle(ol,0);
  HtmlHelp *htmlHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"examples"); 
    htmlHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trExamplesDescription());
  ol.startIndexList();
  PageInfo *pi=exampleList.first();
  while (pi)
  {
    ol.writeListItem();
    QCString n=convertSlashes(pi->name,TRUE)+"-example";
    if (!pi->title.isEmpty())
    {
      ol.writeObjectLink(0,n,0,pi->title);
      if (hasHtmlHelp) htmlHelp->addContentsItem(pi->title,n);
    }
    else
    {
      ol.writeObjectLink(0,n,0,pi->name);
      if (hasHtmlHelp) htmlHelp->addContentsItem(pi->name,n);
    }
    ol.writeString("\n");
    pi=exampleList.next();
  }
  ol.endIndexList();
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  endFile(ol);
  ol.enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------

void writePageIndex(OutputList &ol)
{
  if (pageList.count()==0) return;
  ol.disable(OutputGenerator::Man);
  startFile(ol,"pages","Page Index");
  startTitle(ol);
  QCString title = Config::projectName+" "+theTranslator->trRelatedPages();
  ol.docify(title);
  endTitle(ol,0);
  HtmlHelp *htmlHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"pages"); 
    htmlHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trRelatedPagesDescription());
  ol.startIndexList();
  PageInfo *pi=pageList.first();
  while (pi)
  {
    QCString pageName,pageTitle;
    
    if (Config::caseSensitiveNames)
      pageName=pi->name.copy();
    else
      pageName=pi->name.lower();
    
    if (pi->title.isEmpty())
      pageTitle=pi->name;
    else
      pageTitle=pi->title;
    
    ol.writeListItem();
    ol.writeObjectLink(0,pageName,0,pageTitle);
    ol.writeString("\n");
    if (hasHtmlHelp) htmlHelp->addContentsItem(pageTitle,pageName);
    pi=pageList.next();
  }
  ol.endIndexList();
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  endFile(ol);
  ol.enable(OutputGenerator::Man);
}

//----------------------------------------------------------------------------

int countGroups()
{
  int count=0;
  GroupListIterator gli(groupList);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
  {
    if (gd->countMembers()>0) count++;
    gd=groupList.next();
  }
  return count;
}

//----------------------------------------------------------------------------

void writeGroupList(OutputList &ol)
{
  ol.startDescription();
  GroupListIterator gli(groupList);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
  {
    if (gd->countMembers()>0)
    {
      ol.startDescItem();
      ol.startTextLink(gd->getOutputFileBase(),0);
      parseDoc(ol,0,0,gd->groupTitle());
      ol.endTextLink();
      ol.endDescItem();
      parseDoc(ol,0,0,gd->briefDescription());
      ol.newParagraph();
    }
  }
  ol.endDescription();
}

//----------------------------------------------------------------------------

void writeGroupIndex(OutputList &ol)
{
  if (documentedGroups==0) return; 
  ol.disable(OutputGenerator::Man);
  startFile(ol,"modules","Module Index");
  startTitle(ol);
  QCString title = Config::projectName+" "+theTranslator->trModules();
  parseText(ol,title);
  endTitle(ol,0);
  HtmlHelp *htmlHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"modules"); 
    htmlHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trModulesDescription());
  writeGroupList(ol);
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  endFile(ol);
  ol.enable(OutputGenerator::Man);
}

void writeIndex(OutputList &ol)
{
  // save old generator state
  bool manEnabled = ol.isEnabled(OutputGenerator::Man);
  bool texEnabled = ol.isEnabled(OutputGenerator::Latex);
  bool htmEnabled = ol.isEnabled(OutputGenerator::Html);

  QCString projPrefix;
  if (!Config::projectName.isEmpty())
  {
    projPrefix=Config::projectName+" ";
  }
  // write HTML index
  ol.disable(OutputGenerator::Man);
  ol.disable(OutputGenerator::Latex);
  ol.startFile("index","Main Index",FALSE);
  if (!Config::noIndexFlag) writeQuickLinks(ol,TRUE);
  ol.startTitleHead();
  parseText(ol,projPrefix+theTranslator->trDocumentation());
  ol.endTitleHead(0);
  ol.newParagraph();
  if (!Config::projectNumber.isEmpty())
  {
    ol.startProjectNumber();
    parseDoc(ol,0,0,Config::projectNumber);
    ol.endProjectNumber();
  }
  if (Config::noIndexFlag) writeQuickLinks(ol,FALSE);
  endFile(ol);
  ol.disable(OutputGenerator::Html);
  
  // write LaTeX index
  ol.enable(OutputGenerator::Latex);
  ol.startFile("refman",0,FALSE);
  ol.startIndexSection(isTitlePageStart);
  parseText(ol,projPrefix+theTranslator->trReferenceManual());
  if (!Config::projectNumber.isEmpty())
  {
    ol.startProjectNumber(); 
    parseDoc(ol,0,0,Config::projectNumber);
    ol.endProjectNumber();
  }
  ol.endIndexSection(isTitlePageStart);
  ol.startIndexSection(isTitlePageAuthor);
  parseText(ol,theTranslator->trGeneratedBy());
  ol.endIndexSection(isTitlePageAuthor);
  if (documentedGroups>0)
  {
    ol.startIndexSection(isModuleIndex);
    parseText(ol,projPrefix+theTranslator->trModuleIndex());
    ol.endIndexSection(isModuleIndex);
  }
  if (documentedNamespaces>0)
  {
    ol.startIndexSection(isNamespaceIndex);
    parseText(ol,projPrefix+theTranslator->trNamespaceIndex());
    ol.endIndexSection(isNamespaceIndex);
  }
  if (hierarchyClasses>0)
  {
    ol.startIndexSection(isClassHierarchyIndex);
    parseText(ol,projPrefix+theTranslator->trHierarchicalIndex());
    ol.endIndexSection(isClassHierarchyIndex);
  }
  if (annotatedClasses>0)
  {
    ol.startIndexSection(isCompoundIndex);
    parseText(ol,projPrefix+theTranslator->trCompoundIndex());
    ol.endIndexSection(isCompoundIndex);
  }
  if (documentedFiles>0)
  {
    ol.startIndexSection(isFileIndex);
    parseText(ol,projPrefix+theTranslator->trFileIndex());
    ol.endIndexSection(isFileIndex);
  }
  if (documentedGroups>0)
  {
    ol.startIndexSection(isModuleDocumentation);
    parseText(ol,projPrefix+theTranslator->trModuleDocumentation());
    ol.endIndexSection(isModuleDocumentation);
  }
  if (documentedNamespaces>0)
  {
    ol.startIndexSection(isNamespaceDocumentation);
    parseText(ol,projPrefix+theTranslator->trNamespaceDocumentation());
    ol.endIndexSection(isNamespaceDocumentation);
  }
  if (annotatedClasses>0)
  {
    ol.startIndexSection(isClassDocumentation);
    parseText(ol,projPrefix+theTranslator->trClassDocumentation());
    ol.endIndexSection(isClassDocumentation);
  }
  if (documentedFiles>0)
  {
    ol.startIndexSection(isFileDocumentation);
    parseText(ol,projPrefix+theTranslator->trFileDocumentation());
    ol.endIndexSection(isFileDocumentation);
  }
  if (exampleList.count()>0)
  {
    ol.startIndexSection(isExampleDocumentation);
    parseText(ol,projPrefix+theTranslator->trExampleDocumentation());
    ol.endIndexSection(isExampleDocumentation);
  }
  if (pageList.count()>0)
  {
    ol.startIndexSection(isPageDocumentation);
    parseText(ol,projPrefix+theTranslator->trPageDocumentation());
    ol.endIndexSection(isPageDocumentation);
  }
  ol.endIndexSection(isEndIndex);
  endFile(ol);

  // restore generator state
  if (manEnabled) ol.enable(OutputGenerator::Man); 
             else ol.disable(OutputGenerator::Man);
  if (texEnabled) ol.enable(OutputGenerator::Latex);
             else ol.disable(OutputGenerator::Latex);
  if (htmEnabled) ol.enable(OutputGenerator::Html);
             else ol.disable(OutputGenerator::Html);
}
