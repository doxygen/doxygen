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
    if (cd->isVisibleExt()) return TRUE;
    hasVisibleRoot(cd->baseClasses());
  }
  return FALSE;
}

//----------------------------------------------------------------------------

void writeClassTree(OutputList &ol,BaseClassList *bcl,bool hideSuper)
{
  BaseClassListIterator bcli(*bcl);
  bool started=FALSE;
  for ( ; bcli.current() ; ++bcli)
  {
    ClassDef *cd=bcli.current()->classDef;
    if (cd->isVisibleExt() && hasVisibleRoot(cd->baseClasses()))
    {
      if (!started)
      {
        ol.startIndexList();
        started=TRUE;
      }
      //printf("Passed...\n");
      if (cd->hasDocumentation() || cd->isReference())
      {
        ol.writeIndexItem(cd->getReference(),cd->getOutputFileBase(),cd->name());
        if (cd->isReference()) 
        { 
          ol.startTypewriter(); 
          ol.docify(" [external]");
          ol.endTypewriter();
        }
      }
      else
      {
        ol.writeIndexItem(0,0,cd->name());
      }
      if (!cd->visited && !hideSuper && cd->superClasses()->count()>0)
      {
        writeClassTree(ol,cd->superClasses(),cd->visited);
        cd->visited=TRUE;
      }
    }
  }
  if (started) ol.endIndexList(); 
}

//----------------------------------------------------------------------------

void writeClassHierarchy(OutputList &ol)
{
  initClassHierarchy(&classList);

  bool started=FALSE;
  ClassListIterator cli(classList);
  for (;cli.current(); ++cli)
  {
    ClassDef *cd=cli.current();
    if (!hasVisibleRoot(cd->baseClasses()))
    {
      if (cd->isVisibleExt())
      {
        if (!started)
        {
          ol.startIndexList();
          started=TRUE;
        }
        if (cd->hasDocumentation() || cd->isReference())
        {
          ol.writeIndexItem(cd->getReference(),cd->getOutputFileBase(),cd->name());
          if (cd->isReference()) 
          {
            ol.startTypewriter(); 
            ol.docify(" [external]");
            ol.endTypewriter();
          }
        }
        else
        {
          ol.writeIndexItem(0,0,cd->name());
        }
      }
      if (!cd->visited && cd->superClasses()->count()>0) 
      {
        writeClassTree(ol,cd->superClasses(),cd->visited);
        cd->visited=TRUE;
      }
    }
  }
  if (started) ol.endIndexList();
}

//----------------------------------------------------------------------------

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
  parseText(ol,Config::projectName+" "+theTranslator->trClassHierarchy());
  endTitle(ol,0);
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
      if (fd->hasDocumentation()) count++;
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
  parseText(ol,Config::projectName+" "+theTranslator->trFileList());
  endTitle(ol,0);
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
      if (fd->hasDocumentation() && !fd->isReference())
      {
        //ol.writeIndexItem(fd->getReference(),fd->diskName(),
        //    fd->name());
        QCString path;
        if (Config::fullPathNameFlag) 
        {
          path=fd->getPath().copy();
          // strip part of the path
          if (path.left(Config::stripFromPath.length())==Config::stripFromPath)
            path=path.right(path.length()-Config::stripFromPath.length());
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
      }
      fd=fn->next();
    }
    fn=inputNameList.next();
  }
  ol.endIndexList();
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
    if (nd->isVisible()) count++;
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
  parseText(ol,Config::projectName+" "+theTranslator->trNamespaceList());
  endTitle(ol,0);
  ol.newParagraph();
  parseText(ol,theTranslator->trNamespaceListDescription(Config::extractAllFlag));
  ol.newParagraph();

  ol.startIndexList();
  NamespaceDef *nd=namespaceList.first();
  while (nd)
  {
    if (nd->isVisible())
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
    }
    nd=namespaceList.next();
  }
  ol.endIndexList();
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
    if (!cd->isReference() && cd->isVisible()) 
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
    if (!cd->isReference() && 
        //!cd->name().isEmpty() && cd->name()[0]!='@' && 
        //(cd->protection()!=Private || Config::extractPrivateFlag) &&
        //(cd->hasDocumentation() || !hideClassFlag)
        cd->isVisible()
       )
    {
      QCString type;
      switch (cd->compoundType())
      {
        case ClassDef::Class:  type="class";   break;
        case ClassDef::Struct: type="struct";  break;
        case ClassDef::Union:  type="union";   break;
        default:               type="unknown"; break; // an error
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
    }
    cd=classList.next(); 
  }
  ol.endIndexList();
}

//----------------------------------------------------------------------------

void writeAnnotatedIndex(OutputList &ol)
{
  if (annotatedClasses==0) return;
  
  //if (classList.count()==0) return;
  ol.disable(OutputGenerator::Man);
  startFile(ol,"annotated","Annotated Index");
  startTitle(ol);
  parseText(ol,Config::projectName+" "+theTranslator->trCompoundList());
  endTitle(ol,0);
  parseText(ol,theTranslator->trCompoundListDescription());
  writeAnnotatedClassList(ol);
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
      if ((md->isFriend() || md->protection()!=Private || Config::extractPrivateFlag) && 
          !md->isReference() && md->hasDocumentation() &&
           md->name()[0]!='@' && (cd=md->memberClass()) &&
           cd->isVisible()
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
        if (cd && (md->isFriend() || md->protection()!=Private || Config::extractPrivateFlag) && 
            !md->isReference() && md->hasDocumentation() && 
            prevName!=cd->name() && 
            cd->isVisible()
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
      if ((md->isFriend() || md->protection()!=Private || Config::extractPrivateFlag) && 
          !md->isReference() && !md->isRelated() && md->hasDocumentation() &&
          md->name()[0]!='@' && (cd=md->memberClass()) && cd->isVisible()) 
        otherMd=md;
      if ((md->isFriend() || md->protection()!=Private || Config::extractPrivateFlag) && 
          !md->isReference() && md->isRelated() && md->hasDocumentation() &&
          md->name()[0]!='@' && (cd=md->memberClass()) && cd->isVisible())
        found=TRUE;
      else
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
         (md->getFileDef() && md->getFileDef()->hasDocumentation()) ||
         (md->getFileDec() && md->getFileDec()->hasDocumentation());
      
      if (fd && hasDocs && 
          !md->isReference() && 
          md->hasDocumentation() && 
          md->name()[0]!='@') found=TRUE;
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
         (md->getFileDef() && md->getFileDef()->hasDocumentation()) ||
         (md->getFileDec() && md->getFileDec()->hasDocumentation());
        if (fd && hasDocs && 
            !md->isReference() && 
            md->hasDocumentation() && 
            md->name()[0]!='@' && 
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
      if (nd && nd->isVisible() &&
          !md->isReference() && md->hasDocumentation() && 
          !md->name().isEmpty() && md->name().at(0)!='@') found=TRUE;
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
        if (nd && nd->isVisible() &&
            !md->isReference() && md->hasDocumentation() && 
            !md->name().isEmpty() && md->name().at(0)!='@' &&
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
      if (nd && nd->isVisible() &&
          !md->isReference() && md->hasDocumentation() && 
          !md->name().isEmpty() && md->name().at(0)!='@'
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
      if (!md->isReference() && md->hasDocumentation() && 
          !md->name().isEmpty() && md->name()[0]!='@' && 
          (((fd=md->getFileDef()) && fd->hasDocumentation())
            || 
           ((fd=md->getFileDec()) && fd->hasDocumentation())
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

void writeHeaderFileList(OutputList &ol)
{
  if (includeFiles.count()>0)
  {
    ol.startItemList();
    FileDef *fd=includeFiles.first();
    while (fd)
    {
      ol.writeListItem();
      QCString path;
      if (Config::fullPathNameFlag) 
      {
        path=fd->getPath().copy();
        // strip part of the path
        if (path.left(Config::stripFromPath.length())==Config::stripFromPath)
          path=path.right(path.length()-Config::stripFromPath.length());
      }
      if (!path.isEmpty()) ol.docify(path);
      ol.writeObjectLink(0,fd->includeName(),0,fd->name());
      ol.writeString("\n");
      fd=includeFiles.next();
    }  
    ol.endItemList();
  }
}

//----------------------------------------------------------------------------

void writeHeaderIndex(OutputList &ol)
{
  if (includeFiles.count()==0) return;
  ol.disable(OutputGenerator::Man);
  ol.disable(OutputGenerator::Latex);
  startFile(ol,"headers","Header File Index");
  startTitle(ol);
  parseText(ol,Config::projectName+" "+theTranslator->trHeaderFiles());
  endTitle(ol,0);
  parseText(ol,theTranslator->trHeaderFilesDescription());
  writeHeaderFileList(ol);
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
  parseText(ol,Config::projectName+" "+theTranslator->trExamples());
  endTitle(ol,0);
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
    }
    else
    {
      ol.writeObjectLink(0,n,0,pi->name);
    }
    ol.writeString("\n");
    pi=exampleList.next();
  }
  ol.endIndexList();
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
  ol.docify(Config::projectName+" "+theTranslator->trRelatedPages());
  endTitle(ol,0);
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
    pi=pageList.next();
  }
  ol.endIndexList();
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
  parseText(ol,Config::projectName+" "+theTranslator->trModules());
  endTitle(ol,0);
  parseText(ol,theTranslator->trModulesDescription());
  writeGroupList(ol);
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
