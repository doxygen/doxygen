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

#include <qtextstream.h>
#include <qdatetime.h>
#include <qdir.h>

#include "message.h"
#include "index.h"
#include "doxygen.h"
#include "doc.h"
#include "code.h"
#include "config.h"
#include "filedef.h"
#include "outputlist.h"
#include "util.h"
#include "groupdef.h"
#include "language.h"
#include "htmlhelp.h"
#include "ftvhelp.h"
#include "dot.h"
#include "page.h"

//----------------------------------------------------------------------------

static bool g_memberIndexLetterUsed[256];
static bool g_fileIndexLetterUsed[256];
static bool g_namespaceIndexLetterUsed[256];

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
  result=result.stripWhiteSpace();
  // strip trailing .
  if (!result.isEmpty() && result.at(result.length()-1)=='.') 
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
  if (!result.isEmpty())
  {
    int c=result[0];
    if (c>='a' && c<='z') c+='A'-'a';
    result[0]=c;
  }
  return result;
}

//----------------------------------------------------------------------------

void writeClassTree(OutputList &ol,BaseClassList *bcl,bool hideSuper)
{
  HtmlHelp *htmlHelp=0;
  FTVHelp  *ftvHelp=0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag /*&& !Config::htmlHelpGroupsOnly*/;
  bool hasFtvHelp  = Config::generateHtml && Config::ftvHelpFlag  /*&& !Config::htmlHelpGroupsOnly*/;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
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
        if (hasFtvHelp)  ftvHelp->incContentsDepth();
        started=TRUE;
      }
      //printf("Passed...\n");
      bool hasChildren = !cd->visited && !hideSuper && cd->superClasses()->count()>0;
      if (cd->isLinkable())
      {
        ol.writeIndexItem(cd->getReference(),cd->getOutputFileBase(),cd->displayName());
        if (cd->isReference()) 
        { 
          ol.startTypewriter(); 
          ol.docify(" [external]");
          ol.endTypewriter();
        }
        if (hasHtmlHelp)
        {
          htmlHelp->addContentsItem(hasChildren,cd->name(),cd->getOutputFileBase());
        }
        if (hasFtvHelp)
        {
          ftvHelp->addContentsItem(hasChildren,cd->getReference(),cd->getOutputFileBase(),0,cd->name());
        }
      }
      else
      {
        ol.writeIndexItem(0,0,cd->name());
        if (hasHtmlHelp)
        {
          htmlHelp->addContentsItem(hasChildren,cd->name(),0);
        }
        if (hasFtvHelp)
        {
          ftvHelp->addContentsItem(hasChildren,0,0,0,cd->name());
        }
      }
      if (hasChildren)
      {
        //printf("Class %s at %p visited=%d\n",cd->name().data(),cd,cd->visited);
        bool wasVisited=cd->visited;
        cd->visited=TRUE;
        writeClassTree(ol,cd->superClasses(),wasVisited);
      }
    }
  }
  if (started) 
  {
    ol.endIndexList(); 
    if (hasHtmlHelp) htmlHelp->decContentsDepth();
    if (hasFtvHelp)  ftvHelp->decContentsDepth();
  }
}


#if 0
//----------------------------------------------------------------------------
/*! Generates HTML Help tree of classes */

void writeClassTree(BaseClassList *cl)
{
  HtmlHelp *htmlHelp=0;
  FTVHelp  *ftvHelp=0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag && Config::htmlHelpGroupsOnly;
  bool hasFtvHelp  = Config::generateHtml && Config::ftvHelpFlag  && Config::htmlHelpGroupsOnly;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
  }
  BaseClassListIterator cli(*cl);
  bool started=FALSE;
  for ( ; cli.current() ; ++cli)
  {
    ClassDef *cd=cli.current()->classDef;
    if (cd->isVisibleInHierarchy() && !cd->visited)
    {
      if (!started)
      {
        if (hasHtmlHelp) htmlHelp->incContentsDepth();
        if (hasFtvHelp)  ftvHelp->incContentsDepth();
        started=TRUE;
      }
      bool hasChildren = cd->superClasses()->count()>0;
      if (cd->isLinkable())
      {
        if (hasHtmlHelp)
        {
            htmlHelp->addContentsItem(hasChildren,cd->name(),cd->getOutputFileBase());
        }
        if (hasFtvHelp)
        {
            ftvHelp->addContentsItem(hasChildren,cd->name(),cd->getOutputFileBase());
        }
      }
      if (hasChildren)
      {
        writeClassTree(cd->superClasses());
      }
      cd->visited=TRUE;
    }
  }
  if (started) 
  {
    if (hasHtmlHelp) htmlHelp->decContentsDepth();
    if (hasFtvHelp)  ftvHelp->decContentsDepth();
  }
}

//----------------------------------------------------------------------------
/*! Generates HTML Help tree of classes */

void writeClassTree(ClassList *cl)
{
  HtmlHelp *htmlHelp=0;
  FTVHelp  *ftvHelp=0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag && Config::htmlHelpGroupsOnly;
  bool hasFtvHelp  = Config::generateHtml && Config::ftvHelpFlag  && Config::htmlHelpGroupsOnly;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
  }
  ClassListIterator cli(*cl);
  bool started=FALSE;
  for ( ; cli.current() ; ++cli)
  {
    ClassDef *cd=cli.current();
    if (cd->isVisibleInHierarchy() && !cd->visited)
    {
      if (!started)
      {
        started=TRUE;
      }
      bool hasChildren = cd->superClasses()->count()>0;
      if (cd->isLinkable())
      {
        if (hasHtmlHelp)
        {
          htmlHelp->addContentsItem(hasChildren,cd->name(),cd->getOutputFileBase());
        }
        if (hasFtvHelp)
        {
          ftvHelp->addContentsItem(hasChildren,cd->getReference(),cd->getOutputFileBase(),0,cd->name());
        }
      }
      if (hasChildren)
      {
        writeClassTree(cd->superClasses());
      }
      cd->visited=TRUE;
    }
  }
}
#endif

//----------------------------------------------------------------------------

void writeClassHierarchy(OutputList &ol)
{
  initClassHierarchy(&classList);

  HtmlHelp *htmlHelp=0;
  FTVHelp  *ftvHelp=0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag /*&& !Config::htmlHelpGroupsOnly*/;
  bool hasFtvHelp  = Config::generateHtml && Config::ftvHelpFlag  /*&& !Config::htmlHelpGroupsOnly*/;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
  }

  bool started=FALSE;
  ClassListIterator cli(classList);
  for (;cli.current(); ++cli)
  {
    ClassDef *cd=cli.current();
    //printf("class %s hasVisibleRoot=%d isVisibleInHierarchy=%d\n",
    //             cd->name().data(),
    //              hasVisibleRoot(cd->baseClasses()),
    //              cd->isVisibleInHierarchy()
    //      );
    if (!hasVisibleRoot(cd->baseClasses()))
    {
      if (cd->isVisibleInHierarchy()) 
      {
        if (!started)
        {
          ol.startIndexList();
          if (hasHtmlHelp) htmlHelp->incContentsDepth();
          if (hasFtvHelp)  ftvHelp->incContentsDepth();
          started=TRUE;
        }
        bool hasChildren = !cd->visited && cd->superClasses()->count()>0; 
        if (cd->isLinkable())
        {
          ol.writeIndexItem(cd->getReference(),cd->getOutputFileBase(),cd->displayName());
          if (cd->isReference()) 
          {
            ol.startTypewriter(); 
            ol.docify(" [external]");
            ol.endTypewriter();
          }
          if (hasHtmlHelp)
          {
            htmlHelp->addContentsItem(hasChildren,cd->name(),cd->getOutputFileBase());
          }
          if (hasFtvHelp)
          {
            ftvHelp->addContentsItem(hasChildren,cd->getReference(),cd->getOutputFileBase(),0,cd->name());
          }
        }
        else
        {
          ol.writeIndexItem(0,0,cd->displayName());
          if (hasHtmlHelp)
          {
            htmlHelp->addContentsItem(hasChildren,cd->name(),0);
          }
          if (hasFtvHelp)
          {
            ftvHelp->addContentsItem(hasChildren,0,0,0,cd->name());
          }
        }
        if (hasChildren) 
        {
          writeClassTree(ol,cd->superClasses(),cd->visited);
          cd->visited=TRUE;
        }
      }
    }
  }
  if (started) 
  {
    ol.endIndexList();
    if (hasHtmlHelp) htmlHelp->decContentsDepth();
    if (hasFtvHelp)  ftvHelp->decContentsDepth();
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
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"hierarchy","Hierarchical Index");
  startTitle(ol,0);
  QCString title = theTranslator->trClassHierarchy();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  if (Config::generateHtml && Config::htmlHelpFlag /*&& !Config::htmlHelpGroupsOnly*/)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(TRUE,htmlHelpTitle,"hierarchy"); 
  }
  if (Config::generateHtml && Config::ftvHelpFlag /*&& !Config::htmlHelpGroupsOnly*/)
  {
    ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(TRUE,0,"hierarchy",0,ftvHelpTitle); 
  }
  if (Config::haveDotFlag && Config::gfxHierarchyFlag)
  {
    ol.disable(OutputGenerator::Latex);
    ol.disable(OutputGenerator::RTF);
    ol.startTextLink("inherits",0);
    parseText(ol,theTranslator->trGotoGraphicalHierarchy());
    ol.endTextLink();
    ol.newParagraph();
    ol.enable(OutputGenerator::Latex);
    ol.enable(OutputGenerator::RTF);
  }
  parseText(ol,theTranslator->trClassHierarchyDescription());
  //ol.newParagraph();
  ol.endTextBlock();
  writeClassHierarchy(ol);
  endFile(ol);
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeGraphicalClassHierarchy(OutputList &ol)
{
  if (hierarchyClasses==0) return;
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,"inherits","Graphical Class Hierarchy");
  startTitle(ol,0);
  QCString title = theTranslator->trGraphicalHierarchy();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(FALSE,htmlHelpTitle,"inherits"); 
  }
  if (Config::generateHtml && Config::ftvHelpFlag)
  {
    ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(FALSE,0,"inherits",0,ftvHelpTitle); 
  }
  ol.startTextLink("hierarchy",0);
  parseText(ol,theTranslator->trGotoTextualHierarchy());
  ol.endTextLink();
  ol.newParagraph();
  //parseText(ol,theTranslator->trClassHierarchyDescription());
  //ol.newParagraph();
  ol.endTextBlock();
  DotGfxHierarchyTable g;
  ol.writeGraphicalHierarchy(g);
  endFile(ol);
  ol.enableAll();
}

//----------------------------------------------------------------------------


void countFiles(int &htmlFiles,int &files)
{
  htmlFiles=0;
  files=0;
  FileNameListIterator fnli(inputNameList);
  FileName *fn;
  for (;(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      bool doc = fd->isLinkableInProject();
      bool src = fd->generateSourceFile();
      bool nameOk = fd->name().right(4)!=".doc" && 
                    fd->name().right(4)!=".txt";
      if (nameOk)
      {
        if (doc || src)
        {
          htmlFiles++;
        }
        if (doc)
        {
          files++;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------

void writeFileIndex(OutputList &ol)
{
  if (documentedHtmlFiles==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  if (documentedFiles==0) ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,"files","File Index");
  startTitle(ol,0);
  QCString title = theTranslator->trFileList();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag /*&& !Config::htmlHelpGroupsOnly*/;
  bool hasFtvHelp =  Config::generateHtml && Config::ftvHelpFlag  /*&& !Config::htmlHelpGroupsOnly*/;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(TRUE,htmlHelpTitle,"files"); 
    htmlHelp->incContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(TRUE,0,"files",0,ftvHelpTitle); 
    ftvHelp->incContentsDepth();
  }
  //ol.newParagraph();
  parseText(ol,theTranslator->trFileListDescription(Config::extractAllFlag));
  //ol.newParagraph();
  ol.endTextBlock();

  OutputNameDict outputNameDict(1009);
  OutputNameList outputNameList;
  outputNameList.setAutoDelete(TRUE);
  
  // re-sort input files in (dir,file) output order instead of (file,dir) input order 
  FileName *fn=inputNameList.first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      QCString path=fd->getPath();
      if (path.isEmpty()) path="[external]";
      FileList *fl = outputNameDict.find(path);
      if (fl)
      {
        fl->inSort(fd);
        //printf("+ inserting %s---%s\n",fd->getPath().data(),fd->name().data());
      }
      else
      {
        //printf("o inserting %s---%s\n",fd->getPath().data(),fd->name().data());
        fl = new FileList(path);
        fl->inSort(fd);
        outputNameList.inSort(fl);
        outputNameDict.insert(path,fl);
      }
      fd=fn->next();
    }
    fn=inputNameList.next();
  }
  
  ol.startIndexList();
  FileList *fl=outputNameList.first();
  while (fl)
  {
    FileDef *fd=fl->first();
    while (fd)
    {
      //printf("Found filedef %s\n",fd->name().data());
      bool doc = fd->isLinkableInProject();
      bool src = fd->generateSourceFile();
      bool nameOk = fd->name().right(4)!=".doc" && 
                    fd->name().right(4)!=".txt";
      if (nameOk && (doc || src) && 
              !fd->isReference())
      {
        QCString path;
        if (Config::fullPathNameFlag) 
        {
          path=stripFromPath(fd->getPath().copy());
        }

        // --------------- LaTeX/RTF only -------------------------
        if (doc)
        {
          ol.pushGeneratorState();
          ol.disable(OutputGenerator::Html);
          ol.writeStartAnnoItem("file",
              fd->getOutputFileBase(),
              path,
              fd->name()
                               );
          if (!fd->briefDescription().isEmpty())
          {
            ol.docify(" (");
            OutputList briefOutput(&ol);
            parseDoc(briefOutput,fd->absFilePath(),1,
                0,0,
                abbreviate(fd->briefDescription(),fd->name()));
            ol+=briefOutput;
            ol.docify(")");
          }
          ol.writeEndAnnoItem(fd->getOutputFileBase());
          ol.popGeneratorState();
        }
        // --------------------------------------------------------

        // ----------------- HTML only ----------------------------
        ol.pushGeneratorState();
        ol.disableAllBut(OutputGenerator::Html);
        ol.writeListItem();
        ol.docify(path);
        if (doc)
        {
          ol.writeObjectLink(0,fd->getOutputFileBase(),0,fd->name());
          if (hasHtmlHelp)
          {
            htmlHelp->addContentsItem(FALSE,fd->name(),fd->getOutputFileBase());
          }
          if (hasFtvHelp)
          {
            ftvHelp->addContentsItem(FALSE,fd->getReference(),fd->getOutputFileBase(),0,fd->name());
          }
        }
        else
        {
          ol.startBold();
          ol.docify(fd->name());
          ol.endBold();
          if (hasHtmlHelp)
          {
            htmlHelp->addContentsItem(FALSE,fd->name(),0);
          }
          if (hasFtvHelp)
          {
            ftvHelp->addContentsItem(FALSE,0,0,0,fd->name());
          }
        }
        if (src)
        {
          ol.docify(" ");
          ol.startTextLink(fd->includeName(),0);
          ol.docify("[");
          parseText(ol,theTranslator->trCode());
          ol.docify("]");
          ol.endTextLink();
        }
        if (!fd->briefDescription().isEmpty())
        {
          ol.docify(" (");
          OutputList briefOutput(&ol);
          parseDoc(briefOutput,
              fd->absFilePath(),1,
              0,0,
              abbreviate(fd->briefDescription(),fd->name()));
          ol+=briefOutput;
          ol.docify(")");
        }
        ol.popGeneratorState();
        // --------------------------------------------------------
      }
      fd=fl->next();
    }
    fl=outputNameList.next();
  }
  ol.endIndexList();
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp->decContentsDepth();
  }
  endFile(ol);
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------
int countNamespaces()
{
  int count=0;
  NamespaceListIterator nli(namespaceList);
  NamespaceDef *nd;
  for (;(nd=nli.current());++nli)
  {
    if (nd->isLinkableInProject() && nd->countMembers()>0) count++;
  }
  return count;
}

//----------------------------------------------------------------------------

void writeNamespaceIndex(OutputList &ol)
{
  if (documentedNamespaces==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"namespaces","Namespace Index");
  startTitle(ol,0);
  QCString title = theTranslator->trNamespaceList();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp  = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag /*&& !Config::htmlHelpGroupsOnly*/;
  bool hasFtvHelp =  Config::generateHtml && Config::ftvHelpFlag  /*&& !Config::htmlHelpGroupsOnly*/;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(TRUE,htmlHelpTitle,"namespaces"); 
    htmlHelp->incContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(TRUE,0,"namespaces",0,ftvHelpTitle); 
    ftvHelp->incContentsDepth();
  }
  //ol.newParagraph();
  parseText(ol,theTranslator->trNamespaceListDescription(Config::extractAllFlag));
  //ol.newParagraph();
  ol.endTextBlock();

  ol.startIndexList();
  NamespaceDef *nd=namespaceList.first();
  while (nd)
  {
    if (nd->isLinkableInProject() && nd->countMembers()>0)
    {
      ol.writeStartAnnoItem("namespace",nd->getOutputFileBase(),0,nd->name());
      if (!nd->briefDescription().isEmpty())
      {
        ol.docify(" (");
        OutputList briefOutput(&ol);
        parseDoc(briefOutput,
                 nd->getDefFileName(),nd->getDefLine(),
                 nd->name(),0,
                 abbreviate(nd->briefDescription(),nd->name()));
        ol+=briefOutput;
        ol.docify(")");
      }
      ol.writeEndAnnoItem(nd->getOutputFileBase());
      if (hasHtmlHelp)
      {
        htmlHelp->addContentsItem(FALSE,nd->name(),nd->getOutputFileBase());
      }
      if (hasFtvHelp)
      {
        ftvHelp->addContentsItem(FALSE,nd->getReference(),nd->getOutputFileBase(),0,nd->name());
      }
    }
    nd=namespaceList.next();
  }
  ol.endIndexList();
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp->decContentsDepth();
  }
  endFile(ol);
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
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
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag /*&& !Config::htmlHelpGroupsOnly*/;
  bool hasFtvHelp =  Config::generateHtml && Config::ftvHelpFlag  /*&& !Config::htmlHelpGroupsOnly*/;
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
      ol.writeStartAnnoItem(type,cd->getOutputFileBase(),0,cd->displayName());
      if (!cd->briefDescription().isEmpty())
      {
        ol.docify(" (");
        OutputList briefOutput(&ol);
        parseDoc(briefOutput,
                 cd->getDefFileName(),cd->getDefLine(),
                 cd->name(),0,
                 abbreviate(cd->briefDescription(),cd->name()));
        ol+=briefOutput;
        ol.docify(")");
      }
      ol.writeEndAnnoItem(cd->getOutputFileBase());
      if (hasHtmlHelp)
      {
        HtmlHelp::getInstance()->addContentsItem(FALSE,cd->name(),cd->getOutputFileBase());
      }
      if (hasFtvHelp)
      {
        FTVHelp::getInstance()->addContentsItem(FALSE,cd->getReference(),cd->getOutputFileBase(),0,cd->name());
      }
    }
    cd=classList.next(); 
  }
  ol.endIndexList();
}

//----------------------------------------------------------------------------

// write an alphabetical index of all class with a header for each letter
void writeAlphabeticalClassList(OutputList &ol)
{
  ol.startAlphabeticalIndexList(); 

  // first count the number of headers
  ClassListIterator cli(classList);
  ClassDef *cd;
  char startLetter=0;
  int headerItems=0;
  for (;(cd=cli.current());++cli)
  {
    if (cd->isLinkableInProject())
    {
      int index = getPrefixIndex(cd->name());
      if (toupper(cd->name().at(index))!=startLetter) // new begin letter => new header
      {
        startLetter=toupper(cd->name().at(index));
        headerItems++;
      }
    }
  }

  // the number of columns in the table
  const int columns = Config::colsInAlphaIndex;

  int i,j;
  int totalItems = headerItems + annotatedClasses;            // number of items in the table
  int rows = (totalItems + columns - 1)/columns;              // number of rows in the table
  int itemsInLastRow = (totalItems + columns -1)%columns + 1; // number of items in the last row

  //printf("headerItems=%d totalItems=%d columns=%d rows=%d itemsInLastRow=%d\n",
  //    headerItems,totalItems,columns,rows,itemsInLastRow);

  // create one class list for each column 
  ClassList *colList = new ClassList[columns];

  // fill the columns with the class list (row elements in each column,
  // expect for the columns with number >= itemsInLastRow, which get on
  // item less.
  int col=0,row=0;
  //int icount=0;
  startLetter=0;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    if (cd->isLinkableInProject())
    {
      int index = getPrefixIndex(cd->name());
      if (toupper(cd->name().at(index))!=startLetter)
      {
        // insert a new header using a dummy class pointer.
        startLetter=toupper(cd->name().at(index));
        colList[col].append((ClassDef *)8); // insert dummy for the header
        row++;
        if ( row >= rows + ((col<itemsInLastRow) ? 0 : -1)) 
        { 
          // if the header is the last item in the row, we add an extra
          // row to make it easier to find the text of the header (this
          // is then contained in the next cell)
          colList[col].append(cd); 
          col++; 
          row=0; 
        }
      }
      // add the class definition to the correct column list
      colList[col].append(cd);
      row++;
      if ( row >= rows + ((col<itemsInLastRow) ? 0 : -1)) { col++; row=0; }
    }
  }

  // create iterators for each column
  ClassListIterator **colIterators = new ClassListIterator*[columns];
  for (i=0;i<columns;i++)
  {
    colIterators[i] = new ClassListIterator(colList[i]);
  }

  // generate table
  for (i=0;i<rows;i++) // foreach table row
  {
    ol.nextTableRow();
    // the last column may contain less items then the others
    int colsInRow = (i<rows-1) ? columns : itemsInLastRow; 
    //printf("row [%d]\n",i);
    for (j=0;j<colsInRow;j++) // foreach table column
    {
      ClassDef *cd = colIterators[j]->current();
      //printf("columns [%d] cd=%p\n",j,cd);
      if (cd==(ClassDef *)8) // the class pointer is really a header
      {
        cd=++(*colIterators[j]); // get the next item
        if (cd)
        {
          //printf("head ClassDef=%p %s\n",cd,cd ? cd->name().data() : "<none>");
          int index = getPrefixIndex(cd->name());
          startLetter=toupper(cd->name().at(index));
          char s[2]; s[0]=startLetter; s[1]=0;
          ol.writeIndexHeading(s);
        }
      }
      else if (cd) // a real class, insert a link
      {
        QCString cname;
        QCString namesp;
        if (Config::hideScopeNames)
        {
          cname=cd->displayName();
        }
        else
        {
          extractNamespaceName(cd->name(),cname,namesp);
        }

        ol.writeObjectLink(cd->getReference(),
                           cd->getOutputFileBase(),0,cname);
        if (!namesp.isEmpty())
        {
          ol.docify(" (");
          NamespaceDef *nd = getResolvedNamespace(namesp);
          if (nd && nd->isLinkable())
          {
            ol.writeObjectLink(nd->getReference(),
                           nd->getOutputFileBase(),0,namesp);
          }
          else
          {
            ol.docify(namesp);
          }
          ol.docify(")");
        }
        ol.writeNonBreakableSpace();
        //printf("item ClassDef=%p %s\n",cd,cd ? cd->name().data() : "<none>");
        ++(*colIterators[j]);
      }
      ol.endTableColumn();
      if (j<colsInRow-1) ol.nextTableColumn();
    }
    ol.endTableRow();
  }
  ol.endAlphabeticalIndexList();

  // release the temporary memory
  for (i=0;i<columns;i++)
  {
    delete colIterators[i];
  }
  delete[] colIterators;
  delete[] colList;
}

//----------------------------------------------------------------------------

void writeAlphabeticalIndex(OutputList &ol)
{
  if (annotatedClasses==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,"classes.html","Alphabetical index");
  startTitle(ol,0);
  parseText(ol,Config::projectName+" "+theTranslator->trCompoundIndex());
  endTitle(ol,0,0);
  writeAlphabeticalClassList(ol);
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeAnnotatedIndex(OutputList &ol)
{
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag /*&& !Config::htmlHelpGroupsOnly*/;
  bool hasFtvHelp  = Config::generateHtml && Config::ftvHelpFlag  /*&& !Config::htmlHelpGroupsOnly*/;

  if (annotatedClasses==0) return;
  
  //if (classList.count()==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"annotated","Annotated Index");
  startTitle(ol,0);
  QCString title = theTranslator->trCompoundList();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle =  title;
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(TRUE,htmlHelpTitle,"annotated"); 
    htmlHelp->incContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(TRUE,0,"annotated",0,ftvHelpTitle); 
    ftvHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trCompoundListDescription());
  //ol.newParagraph();
  ol.endTextBlock();
  writeAnnotatedClassList(ol);
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp->decContentsDepth();
  }
  
  endFile(ol);
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeMemberList(OutputList &ol,bool useSections)
{
  bool first = TRUE;
  char lastChar = 0;
  MemberName *mn=memberNameList.first();
  while (mn)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    bool isFunc=FALSE;
    while (md && !found)
    {
      ClassDef *cd;
      if (
           md->isLinkableInProject() &&
           (cd=md->getClassDef()) &&
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
      if (useSections)
      {
        QCString name = mn->memberName();
        if (tolower(name.at(0))!=lastChar)
        {
          if (!first) ol.endItemList();
          char cs[2];
          lastChar=cs[0]=tolower(name.at(0));cs[1]='\0';
          QCString anchor=(QCString)"index_"+cs;
          QCString title=(QCString)"- "+cs+" -";
          ol.startSection(anchor,title,TRUE);
          ol.docify(title);
          ol.endSection(anchor,TRUE);
          ol.startItemList();
          first=FALSE;
        }
      }
      else if (first)
      {
        first=FALSE;
        ol.startItemList();
      }
      ol.writeListItem();
      ol.docify(mn->memberName());
      if (isFunc) ol.docify("()");
      ol.writeString("\n");

      int count=0;
      md=mn->last();
      QCString prevName;
      while (md)
      {
        ClassDef *cd=md->getClassDef();
        if (
            md->isLinkableInProject() &&
            prevName!=cd->displayName() && 
            cd->isLinkableInProject()
           )
        {
          if (count==0) 
            ol.docify(": ");
          else 
            ol.docify(", ");
          ol.writeObjectLink(cd->getReference(),cd->getOutputFileBase(),md->anchor(),
                            cd->displayName());
          count++;
          prevName=cd->displayName();
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
  int i=0;for (i=0;i<256;i++) g_memberIndexLetterUsed[i]=FALSE;
  int count=0;
  MemberName *mn=memberNameList.first();
  while (mn)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    ClassDef *cd;
    while (md && !found)
    {
      if (
          md->isLinkableInProject() && 
          (cd=md->getClassDef()) && 
          cd->isLinkableInProject()
         )
      {
        found=TRUE;
      }
      md=mn->next();
    }
    if (found)
    {
      QCString n = mn->memberName();
      if (!n.isEmpty()) g_memberIndexLetterUsed[tolower(n.at(0))]=TRUE;
      count++;
    }
    mn=memberNameList.next();
  }
  return count;
}

//----------------------------------------------------------------------------

void writeQuickMemberIndex(OutputList &ol,bool *charUsed)
{
  bool first=TRUE;
  int i;
  ol.startCenter();
  for (i=33;i<127;i++)
  {
    QCString anchor="index_";
    char is[2];is[0]=(char)i;is[1]='\0';
    if (charUsed[i])
    {
      if (!first) 
      {
        ol.writeString("&nbsp;|&nbsp;");
      }
      ol.startTextLink(0,anchor+is);
      ol.writeString(is);
      ol.endTextLink();
      first=FALSE;
    }
  }
  ol.endCenter();
  ol.newParagraph();
}

//----------------------------------------------------------------------------

void writeMemberIndex(OutputList &ol)
{
  if (documentedMembers==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,"functions","Compound Member Index");
  startTitle(ol,0);
  parseText(ol,Config::projectName+" "+theTranslator->trCompoundMembers());
  endTitle(ol,0,0);
  bool quickIndex = documentedMembers>50;
  if (quickIndex)
  {
    writeQuickMemberIndex(ol,g_memberIndexLetterUsed);
  }
  parseText(ol,theTranslator->trCompoundMembersDescription(Config::extractAllFlag));
  writeMemberList(ol,quickIndex);
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeFileMemberList(OutputList &ol,bool useSections)
{
  char lastChar=0;
  bool first=TRUE;
  MemberName *mn=functionNameList.first();
  while (mn)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    while (md && !found)
    {
      FileDef *fd=md->getFileDef();
      bool hasDocs = md->getFileDef() && 
                     md->getFileDef()->isLinkableInProject();
      
      if (fd && hasDocs && 
          md->isLinkableInProject()
         ) found=TRUE;
      else
        md=mn->next();
    }
    if (found) // function is documented
    {
      if (useSections)
      {
        QCString name = mn->memberName();
        if (tolower(name.at(0))!=lastChar)
        {
          if (!first) ol.endItemList();
          char cs[2];
          lastChar=cs[0]=tolower(name.at(0));cs[1]='\0';
          QCString anchor=(QCString)"index_"+cs;
          QCString title=(QCString)"- "+cs+" -";
          ol.startSection(anchor,title,TRUE);
          ol.docify(title);
          ol.endSection(anchor,TRUE);
          ol.startItemList();
          first=FALSE;
        }
      }
      else if (first)
      {
        first=FALSE;
        ol.startItemList();
      }
      ol.writeListItem();
      ol.docify(md->name());
      if (md->isFunction()) ol.docify("()");
      ol.writeString("\n");

      int count=0;
      md=mn->last();
      QCString prevName;
      while (md)
      {
        FileDef *fd=md->getFileDef();
        bool hasDocs = md->getFileDef() && 
                       md->getFileDef()->isLinkableInProject();
        if (fd && hasDocs && 
            md->isLinkableInProject() &&
            prevName!=fd->name())
        {
          if (count==0) 
            ol.docify(": ");
          else 
            ol.docify(", ");
          QCString baseName=fd->name().copy();
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

void writeNamespaceMemberList(OutputList &ol,bool useSections)
{
  char lastChar=0;
  bool first=TRUE;
  //ol.startItemList();
  MemberName *mn=functionNameList.first();
  while (mn)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    while (md && !found)
    {
      NamespaceDef *nd=md->getNamespaceDef();
      if (nd && nd->isLinkableInProject() && md->isLinkableInProject()) 
        found=TRUE;
      else
        md=mn->next();
    }
    if (found) // member is documented and in a documented namespace
    {
      if (useSections)
      {
        QCString name = mn->memberName();
        if (tolower(name.at(0))!=lastChar)
        {
          if (!first) ol.endItemList();
          char cs[2];
          lastChar=cs[0]=tolower(name.at(0));cs[1]='\0';
          QCString anchor=(QCString)"index_"+cs;
          QCString title=(QCString)"- "+cs+" -";
          ol.startSection(anchor,title,TRUE);
          ol.docify(title);
          ol.endSection(anchor,TRUE);
          ol.startItemList();
          first=FALSE;
        }
      }
      ol.writeListItem();
      ol.docify(md->name());
      if (md->isFunction()) ol.docify("()");
      ol.writeString("\n");

      int count=0;
      md=mn->last();
      QCString prevName;
      while (md)
      {
        NamespaceDef *nd=md->getNamespaceDef();
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
  int i=0;for (i=0;i<256;i++) g_namespaceIndexLetterUsed[i]=FALSE;
  int count=0;
  MemberName *mn=functionNameList.first();
  while (mn)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    while (md && !found)
    {
      NamespaceDef *nd=md->getNamespaceDef();
      if (nd && nd->isLinkableInProject() && md->isLinkableInProject())
      {
        QCString n = mn->memberName();
        if (!n.isEmpty()) g_namespaceIndexLetterUsed[tolower(n.at(0))]=TRUE;
        found=TRUE;
      }
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
  int i=0;for (i=0;i<256;i++) g_fileIndexLetterUsed[i]=FALSE;
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
          (fd=md->getFileDef()) && 
          fd->isLinkableInProject()
         ) 
      {
        QCString n = mn->memberName();
        if (!n.isEmpty()) g_fileIndexLetterUsed[tolower(n.at(0))]=TRUE;
        found=TRUE;
      }
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
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,"globals","File Member Index");
  startTitle(ol,0);
  parseText(ol,Config::projectName+" "+theTranslator->trFileMembers());
  endTitle(ol,0,0);
  bool quickIndex = documentedMembers>50;
  if (quickIndex)
  {
    writeQuickMemberIndex(ol,g_fileIndexLetterUsed);
  }
  parseText(ol,theTranslator->trFileMembersDescription(Config::extractAllFlag));
  writeFileMemberList(ol,quickIndex);
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeNamespaceMemberIndex(OutputList &ol)
{
  if (documentedNamespaceMembers==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,"namespacemembers","Namespace Member Index");
  startTitle(ol,0);
  parseText(ol,Config::projectName+" "+theTranslator->trNamespaceMembers());
  endTitle(ol,0,0);
  bool quickIndex = documentedMembers>50;
  if (quickIndex)
  {
    writeQuickMemberIndex(ol,g_namespaceIndexLetterUsed);
  }
  parseText(ol,theTranslator->trNamespaceMemberDescription(Config::extractAllFlag));
  writeNamespaceMemberList(ol,quickIndex);
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeExampleIndex(OutputList &ol)
{
  if (exampleSDict->count()==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"examples","Example Index");
  startTitle(ol,0);
  QCString title = theTranslator->trExamples();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag /*&& !Config::htmlHelpGroupsOnly*/;
  bool hasFtvHelp  = Config::generateHtml && Config::ftvHelpFlag  /*&& !Config::htmlHelpGroupsOnly*/;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(TRUE,htmlHelpTitle,"examples"); 
    htmlHelp->incContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(TRUE,0,"examples",0,ftvHelpTitle); 
    ftvHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trExamplesDescription());
  //ol.newParagraph();
  ol.endTextBlock();
  ol.startIndexList();
  PageSDictIterator pdi(*exampleSDict);
  PageInfo *pi=0;
  for (pdi.toFirst();(pi=pdi.current());++pdi)
  {
    ol.writeListItem();
    QCString n=convertFileName(pi->name)+"-example";
    if (!pi->title.isEmpty())
    {
      ol.writeObjectLink(0,n,0,pi->title);
      if (hasHtmlHelp) htmlHelp->addContentsItem(FALSE,pi->title,n);
      if (hasFtvHelp)  ftvHelp->addContentsItem(FALSE,0,n,0,pi->title);
    }
    else
    {
      ol.writeObjectLink(0,n,0,pi->name);
      if (hasHtmlHelp) htmlHelp->addContentsItem(FALSE,pi->name,n);
      if (hasFtvHelp)  ftvHelp->addContentsItem(FALSE,0,n,0,pi->name);
    }
    ol.writeString("\n");
  }
  ol.endIndexList();
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp->decContentsDepth();
  }
  endFile(ol);
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

int countRelatedPages()
{
  int count=0;
  PageSDictIterator pdi(*pageSDict);
  PageInfo *pi=0;
  for (pdi.toFirst();(pi=pdi.current());++pdi)
  {
    if (!pi->inGroup) count++;
  }
  return count;
}

//----------------------------------------------------------------------------

void writePageIndex(OutputList &ol)
{
  if (documentedPages==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"pages","Page Index");
  startTitle(ol,0);
  QCString title = theTranslator->trRelatedPages();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag /*&& !Config::htmlHelpGroupsOnly*/;
  bool hasFtvHelp  = Config::generateHtml && Config::ftvHelpFlag  /*&&  !Config::htmlHelpGroupsOnly*/;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(TRUE,htmlHelpTitle,"pages"); 
    htmlHelp->incContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(TRUE,0,"pages",0,ftvHelpTitle); 
    ftvHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trRelatedPagesDescription());
  //ol.newParagraph();
  ol.endTextBlock();
  ol.startIndexList();
  PageSDictIterator pdi(*pageSDict);
  PageInfo *pi=0;
  for (pdi.toFirst();(pi=pdi.current());++pdi)
  {
    if (!pi->inGroup)
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

      //ol.writeListItem();
      ol.writeStartAnnoItem("pages",pageName,0,pageTitle);
      //ol.writeObjectLink(0,pageName,0,pageTitle);
      ol.writeEndAnnoItem(pageName);
      ol.writeString("\n");
      if (hasHtmlHelp) htmlHelp->addContentsItem(FALSE,pageTitle,pageName);
      if (hasFtvHelp)  ftvHelp->addContentsItem(FALSE,0,pageName,0,pageTitle);
    }
  }
  ol.endIndexList();
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp->decContentsDepth();
  }
  endFile(ol);
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

int countGroups()
{
  int count=0;
  GroupListIterator gli(groupList);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
  {
    if (!gd->isReference())
    {
      gd->visited=FALSE;
      count++;
    }
  }
  return count;
}


//----------------------------------------------------------------------------

void writeGraphInfo(OutputList &ol)
{
  if (!Config::haveDotFlag || !Config::generateHtml) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  generateGraphLegend(Config::htmlOutputDir);
  startFile(ol,"graph_legend","Graph Legend");
  startTitle(ol,0);
  parseText(ol,theTranslator->trLegendTitle());
  endTitle(ol,0,0);
  bool oldStripCommentsState = Config::stripCommentsFlag;
  // temporarily disable the stripping of comments for our own code example!
  Config::stripCommentsFlag = FALSE;
  parseDoc(ol,"graph_legend",1,0,0,theTranslator->trLegendDocs());
  Config::stripCommentsFlag = oldStripCommentsState;
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------
/*!
 * write groups as hierarchial trees
 * \author KPW
 */

void writeGroupTreeNode(OutputList &ol, GroupDef *gd)
{
  HtmlHelp *htmlHelp=0;
  FTVHelp  *ftvHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  bool hasFtvHelp  = Config::generateHtml && Config::ftvHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
  }

  GroupDef *subgd = 0;
  GroupListIterator gli(*gd->groupList);
  if (!gd->visited)
  {
    //printf("gd->name()=%s #members=%d\n",gd->name().data(),gd->countMembers());
    // write group info
    bool hasSubGroups = gd->groupList->count()>0;
    bool hasSubPages = gd->pageDict->count()>0;
    //printf("gd=`%s': pageDict=%d\n",gd->name().data(),gd->pageDict->count());
    if(htmlHelp)
    {
        htmlHelp->addContentsItem(hasSubGroups || hasSubPages,gd->groupTitle(),gd->getOutputFileBase()); 
        htmlHelp->incContentsDepth();
    }
    if(ftvHelp)
    {
        ftvHelp->addContentsItem(hasSubGroups || hasSubPages,gd->getReference(),gd->getOutputFileBase(),0,gd->groupTitle()); 
        ftvHelp->incContentsDepth();
    }

    //ol.writeListItem();
    //ol.startTextLink(gd->getOutputFileBase(),0);
    //parseText(ol,gd->groupTitle());
    //ol.endTextLink();
    
    ol.writeIndexItem(gd->getReference(),gd->getOutputFileBase(),gd->groupTitle());
    if (gd->isReference()) 
    { 
      ol.startTypewriter(); 
      ol.docify(" [external]");
      ol.endTypewriter();
    }
    
    //ol.writeStartAnnoItem(0,gd->getOutputFileBase(),0,gd-);
    //parseText(ol,gd->groupTitle());
    //ol.writeEndAnnoItem(gd->getOutputFileBase());

    // write pages
    PageSDictIterator pli(*gd->pageDict);
    PageInfo *pi = 0;
    for (pli.toFirst();(pi=pli.current());++pli)
    {
      SectionInfo *si=0;
      if (!pi->name.isEmpty()) si=sectionDict[pi->name];
      if(htmlHelp) htmlHelp->addContentsItem(FALSE,
                                   convertToHtml(pi->title),
                                   gd->getOutputFileBase(),
                                   si ? si->label.data() : 0
                                  ); 
      if(ftvHelp)  ftvHelp->addContentsItem(FALSE,
                                   gd->getReference(),
                                   gd->getOutputFileBase(),
                                   si ? si->label.data() : 0,
                                   convertToHtml(pi->title)
                                  ); 
    }

    // write subgroups
    if (hasSubGroups)
    {
      ol.startIndexList();
      for (gli.toLast();(subgd=gli.current());--gli)
      {
        writeGroupTreeNode(ol,subgd);
      }
      ol.endIndexList(); 
    }

#if 0
    // write namespaces
    NamespaceList *namespaceList=gd->namespaceList;
    if (namespaceList->count()>0)
    {
        NamespaceDef *nsd=namespaceList->first();
        while (nsd)
        {
            if(htmlHelp)
                htmlHelp->addContentsItem(FALSE,convertToHtml(nsd->name()).data(),nsd->getOutputFileBase()); 
            if(ftvHelp)
                ftvHelp->addContentsItem(FALSE,convertToHtml(nsd->name()).data(),nsd->getOutputFileBase()); 
            nsd=namespaceList->next();
        }
    }

    // write classes
    writeClassTree(gd->classList);

    // write members
    MemberList memberLists[7] = {
        gd->defineMembers,
        gd->typedefMembers,
        gd->enumMembers,
        gd->enumValMembers,
        gd->funcMembers,
        gd->varMembers,
        gd->protoMembers
    };
    MemberList members;
    for (int i=0;i<7;i++)
    {
        members=memberLists[i];
        if (members.count()>0)
        {
            MemberDef *md=members.first();
            while (md)
            {
                if(htmlHelp)
                    htmlHelp->addContentsItem(FALSE,md->name(),md->getGroupDef()->getOutputFileBase().data(),md->anchor()); 
                if(ftvHelp)
                    ftvHelp->addContentsItem(FALSE,md->name(),md->getGroupDef()->getOutputFileBase().data(),md->anchor()); 
                md=members.next();
            }
        }
    }
       
    // write file list
    FileList *fileList=gd->fileList;
    if (fileList->count()>0)
    {
        FileDef *fd=fileList->first();
        while (fd)
        {
            if(htmlHelp)
                htmlHelp->addContentsItem(FALSE,convertToHtml(fd->name()),fd->getOutputFileBase().data()); 
            if(ftvHelp)
                ftvHelp->addContentsItem(FALSE,convertToHtml(fd->name()),fd->getOutputFileBase().data()); 
            fd=fileList->next();
        }
    }
    
    // write examples
    PageSDictIterator eli(*(gd->exampleDict));
    {
        PageInfo *pi=eli.toFirst();
        while (pi)
        {
            if(htmlHelp)
                htmlHelp->addContentsItem(FALSE,convertToHtml(pi->name),convertNameToFile(pi->name)+"-example"); 
            if(ftvHelp)
                ftvHelp->addContentsItem(FALSE,convertToHtml(pi->name),convertNameToFile(pi->name)+"-example"); 
            pi=++eli;
        }
    }
#endif

    if(htmlHelp) htmlHelp->decContentsDepth();
    if(ftvHelp)  ftvHelp->decContentsDepth();
    gd->visited=TRUE;
  }
}

void writeGroupHierarchy(OutputList &ol)
{
  ol.startIndexList();
  GroupListIterator gli(groupList);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
  {
    writeGroupTreeNode(ol,gd);
  }
  ol.endIndexList(); 
}

//----------------------------------------------------------------------------

#if 0
void writeGroupList(OutputList &ol)
{
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  bool hasFtvHelp  = Config::generateHtml && Config::ftvHelpFlag;
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp  = 0;
  if (hasHtmlHelp) htmlHelp = HtmlHelp::getInstance();
  if (hasFtvHelp) ftvHelp = FTVHelp::getInstance();
  ol.startDescription();
  GroupListIterator gli(groupList);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
  {
    //printf("gd->name()=%s #members=%d\n",gd->name().data(),gd->countMembers());
    //if (gd->countMembers()>0)
    //{
    if (gd->hasDocumentation())
    {
      ol.startDescItem();
      ol.startTextLink(gd->getOutputFileBase(),0);
      parseText(ol,gd->groupTitle());
      ol.endTextLink();
      ol.endDescItem();
      parseDoc(ol,
          gd->getDefFileName(),gd->getDefLine(),
          0,0,gd->briefDescription());
      ol.newParagraph();
      //}

      const char *docFile = 0;
      if (gd->hasDocumentation()) docFile = gd->getOutputFileBase();
      if (hasHtmlHelp)
      {
        htmlHelp = HtmlHelp::getInstance();
        if(!Config::htmlHelpGroupsOnly)
        {
          htmlHelp->addContentsItem(FALSE,gd->groupTitle(),docFile);
        }
      }
      if (hasFtvHelp)
      {
        ftvHelp = FTVHelp::getInstance();
        if(!Config::htmlHelpGroupsOnly)
        {
          ftvHelp->addContentsItem(FALSE,gd->groupTitle(),docFile); 
        }
      }
    }
  }
  ol.endDescription();
}
#endif

//----------------------------------------------------------------------------

void writeGroupIndex(OutputList &ol)
{
  if (documentedGroups==0) return; 
  ol.pushGeneratorState(); 
  ol.disable(OutputGenerator::Man);
  startFile(ol,"modules","Module Index");
  startTitle(ol,0);
  QCString title = theTranslator->trModules();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  bool hasFtvHelp  = Config::generateHtml && Config::ftvHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(TRUE,htmlHelpTitle,"modules"); 
    htmlHelp->incContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(TRUE,0,"modules",0,ftvHelpTitle); 
    ftvHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trModulesDescription());
  ol.endTextBlock();
  writeGroupHierarchy(ol);
  if (hasHtmlHelp)
  {
    //writeGroupTree(ol);   // KPW - modified to write hierarchial HMTL Help
    //if(!Config::htmlHelpGroupsOnly)
    //{
        htmlHelp->decContentsDepth();
    //}
  }
  if (hasFtvHelp)
  {
    //writeGroupTree(ol);   // KPW - modified to write hierarchial FTV Help
    //if(!Config::htmlHelpGroupsOnly)
    //{
        ftvHelp->decContentsDepth();
    //}
  }
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeIndex(OutputList &ol)
{
  // save old generator state
  ol.pushGeneratorState();

  QCString projPrefix;
  if (!Config::projectName.isEmpty())
  {
    projPrefix=Config::projectName+" ";
  }

  //--------------------------------------------------------------------
  // write HTML index
  //--------------------------------------------------------------------
  ol.disableAllBut(OutputGenerator::Html);

  QCString defFileName = 
    mainPage ? mainPage->defFileName.data() : "<generated>";
  int defLine =
    mainPage ? mainPage->defLine : 1;

  QCString title;
  if (!mainPage || mainPage->title.isEmpty())
  {
    title = theTranslator->trMainPage();
  }
  else 
  {
    title = substitute(mainPage->title,"%","");
  }

  QCString indexName="index";
  if (Config::ftvHelpFlag) indexName="main";
  ol.startFile(indexName,title,FALSE);
  
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    HtmlHelp::getInstance()->addContentsItem(FALSE,title,indexName); 
  }
  if (Config::generateHtml && Config::ftvHelpFlag)
  {
    FTVHelp::getInstance()->addContentsItem(FALSE,0,indexName,0,title); 
  }

  if (!Config::noIndexFlag) writeQuickLinks(ol,TRUE);
  ol.startTitleHead(0);
  if (mainPage && !mainPage->title.isEmpty())
  {
    parseDoc(ol,defFileName,defLine,0,0,mainPage->title);
  }
  else
  {
    parseText(ol,projPrefix+theTranslator->trDocumentation());
  }
  ol.endTitleHead(0,0);
  ol.newParagraph();
  if (!Config::projectNumber.isEmpty())
  {
    ol.startProjectNumber();
    parseDoc(ol,defFileName,defLine,0,0,Config::projectNumber);
    ol.endProjectNumber();
  }
  if (Config::noIndexFlag && mainPage==0) writeQuickLinks(ol,FALSE);

  if (mainPage)
  {
    parseDoc(ol,defFileName,defLine,0,0,mainPage->doc);
  }
  
  endFile(ol);
  ol.disable(OutputGenerator::Html);
  
  //--------------------------------------------------------------------
  // write LaTeX/RTF index
  //--------------------------------------------------------------------
  ol.enable(OutputGenerator::Latex);
  ol.enable(OutputGenerator::RTF);

  ol.startFile("refman",0,FALSE);
  ol.startIndexSection(isTitlePageStart);
  if (!Config::latexHeaderFile.isEmpty()) 
  {
    ol.disable(OutputGenerator::Latex);
  }

  parseText(ol,projPrefix+theTranslator->trReferenceManual());
  if (!Config::projectNumber.isEmpty())
  {
    ol.startProjectNumber(); 
    parseDoc(ol,defFileName,defLine,0,0,Config::projectNumber);
    ol.endProjectNumber();
  }
  ol.endIndexSection(isTitlePageStart);
  ol.startIndexSection(isTitlePageAuthor);
  parseText(ol,theTranslator->trGeneratedBy());
  ol.endIndexSection(isTitlePageAuthor);
  if (mainPage)
  {
    ol.startIndexSection(isMainPage);
    if (!mainPage->title.isEmpty())
    {
      parseDoc(ol,defFileName,defLine,0,0,mainPage->title);
    }
    else
    {
      parseText(ol,projPrefix+theTranslator->trMainPage());
    }
    ol.endIndexSection(isMainPage);
  }
  ol.enable(OutputGenerator::Latex);

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
  if (documentedPages>0)
  {
    ol.startIndexSection(isPageIndex);
    parseText(ol,projPrefix+theTranslator->trPageIndex());
    ol.endIndexSection(isPageIndex);
  }
  ol.lastIndexPage();
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
  if (exampleSDict->count()>0)
  {
    ol.startIndexSection(isExampleDocumentation);
    parseText(ol,projPrefix+theTranslator->trExampleDocumentation());
    ol.endIndexSection(isExampleDocumentation);
  }
  if (pageSDict->count()>0)
  {
    ol.startIndexSection(isPageDocumentation);
    parseText(ol,projPrefix+theTranslator->trPageDocumentation());
    ol.endIndexSection(isPageDocumentation);
  }
  ol.endIndexSection(isEndIndex);
  endFile(ol);

  if (mainPage)
  {
    ol.disable(OutputGenerator::Man);
    startFile(ol,mainPage->name,mainPage->title);
    //SectionInfo *si=0;
    //if (!mainPage->title.isEmpty() && !mainPage->name.isEmpty() &&
    //    (si=sectionDict[mainPage->name])!=0)
    //{
    //  ol.startSection(si->label,si->title,FALSE);
    //  ol.docify(si->title);
    //  ol.endSection(si->label,FALSE);
    //}
    ol.startTextBlock();
    parseDoc(ol,defFileName,defLine,0,0,mainPage->doc);
    ol.endTextBlock();
    endFile(ol);
    ol.enable(OutputGenerator::Man);
  }

  // restore generator state
  //if (manEnabled) ol.enable(OutputGenerator::Man); 
  //           else ol.disable(OutputGenerator::Man);
  //if (texEnabled) ol.enable(OutputGenerator::Latex);
  //           else ol.disable(OutputGenerator::Latex);
  //if (htmEnabled) ol.enable(OutputGenerator::Html);
  //           else ol.disable(OutputGenerator::Html);
  ol.popGeneratorState();
}

