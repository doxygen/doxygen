/******************************************************************************
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
#include "packagedef.h"

int annotatedClasses;
int hierarchyClasses;
int documentedFunctions;
int documentedMembers;
int documentedHtmlFiles;
int documentedFiles;
int documentedGroups;
int documentedNamespaces;
int documentedNamespaceMembers;
int documentedIncludeFiles;
int documentedPages;
int indexedPages;
int documentedPackages;

int countClassHierarchy();
int countClassMembers();
int countFileMembers();
void countFiles(int &htmlFiles,int &files);
int countGroups();
int countNamespaces();
int countAnnotatedClasses();
int countNamespaceMembers();
int countIncludeFiles();
void countRelatedPages(int &docPages,int &indexPages);
int countPackages();

void countDataStructures()
{
  annotatedClasses           = countAnnotatedClasses();
  hierarchyClasses           = countClassHierarchy();
  documentedMembers          = countClassMembers();
  documentedFunctions        = countFileMembers();
  countFiles(documentedHtmlFiles,documentedFiles);
  countRelatedPages(documentedPages,indexedPages);
  documentedGroups           = countGroups();
  documentedNamespaces       = countNamespaces();
  documentedNamespaceMembers = countNamespaceMembers();
  documentedPackages         = countPackages();
}

//----------------------------------------------------------------------------

static bool g_memberIndexLetterUsed[256];
static bool g_fileIndexLetterUsed[256];
static bool g_namespaceIndexLetterUsed[256];

const int maxItemsBeforeQuickIndex = 50;

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

void writeQuickLinks(OutputList &ol,bool compact ,bool ext=FALSE)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  QCString extLink;
  if (ext) { extLink="_doc"; }
  if (compact) ol.startCenter(); else ol.startItemList();

  if (!compact) ol.writeListItem();
  if (Config_getBool("GENERATE_TREEVIEW"))
  {
    ol.startQuickIndexItem(extLink,"main.html");
  }
  else
  {
    ol.startQuickIndexItem(extLink,"index.html");
  }
  parseText(ol,theTranslator->trMainPage());
  ol.endQuickIndexItem();

  if (documentedPackages>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"packages.html");
    parseText(ol,theTranslator->trPackages());
    ol.endQuickIndexItem();
  }
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
    if (Config_getBool("ALPHABETICAL_INDEX"))
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
  //if (documentedIncludeFiles>0 && Config_getBool("VERBATIM_HEADERS"))
  //{
  //  if (!compact) ol.writeListItem();
  //  ol.startQuickIndexItem(extLink,"headers.html");
  //  parseText(ol,theTranslator->trHeaderFiles());
  //  ol.endQuickIndexItem();
  //}
  //if (Config_getBool("SOURCE_BROWSER")) 
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
  if (indexedPages>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"pages.html");
    parseText(ol,theTranslator->trRelatedPages());
    ol.endQuickIndexItem();
  } 
  if (Doxygen::exampleSDict->count()>0)
  {
    if (!compact) ol.writeListItem();
    ol.startQuickIndexItem(extLink,"examples.html");
    parseText(ol,theTranslator->trExamples());
    ol.endQuickIndexItem();
  } 
  if (Config_getBool("SEARCHENGINE"))
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
  ol.popGeneratorState();
}

void startTitle(OutputList &ol,const char *fileName)
{
  ol.startTitleHead(fileName);
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
}

void endTitle(OutputList &ol,const char *fileName,const char *name)
{
  ol.popGeneratorState();
  ol.endTitleHead(fileName,name);
}

void startFile(OutputList &ol,const char *name,const char *manName,
               const char *title,bool external)
{
  ol.startFile(name,manName,title,external);
  if (!Config_getBool("DISABLE_INDEX")) writeQuickLinks(ol,TRUE,external);
}

void endFile(OutputList &ol,bool external)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  ol.writeFooter(0,external); // write the footer
  if (Config_getString("HTML_FOOTER").isEmpty())
  {
    parseText(ol,theTranslator->trGeneratedAt(
              dateToString(TRUE),
              Config_getString("PROJECT_NAME")
             ));
  }
  ol.writeFooter(1,external); // write the link to the picture
  if (Config_getString("HTML_FOOTER").isEmpty())
  {
    parseText(ol,theTranslator->trWrittenBy());
  }
  ol.writeFooter(2,external); // end the footer
  ol.popGeneratorState();
  ol.endFile();
}

//----------------------------------------------------------------------------

void writeClassTree(OutputList &ol,BaseClassList *bcl,bool hideSuper)
{
  HtmlHelp *htmlHelp=0;
  FTVHelp  *ftvHelp=0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
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
      bool hasChildren = !cd->visited && !hideSuper && cd->subClasses()->count()>0;
      if (cd->isLinkable())
      {
        //printf("Writing class %s\n",cd->displayName().data());
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
        writeClassTree(ol,cd->subClasses(),wasVisited);
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
/*! Generates HTML Help tree of classes */

void writeClassTree(BaseClassList *cl)
{
  HtmlHelp *htmlHelp=0;
  FTVHelp  *ftvHelp=0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
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
      bool hasChildren = cd->subClasses()->count()>0;
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
        writeClassTree(cd->subClasses());
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

void writeClassTreeNode(ClassDef *cd,bool hasHtmlHelp,bool hasFtvHelp,bool &started)
{
    if (cd->isVisibleInHierarchy() && !cd->visited)
    {
      if (!started)
      {
        started=TRUE;
      }
      bool hasChildren = cd->subClasses()->count()>0;
      if (cd->isLinkable())
      {
        if (hasHtmlHelp)
        {
          HtmlHelp::getInstance()->addContentsItem(hasChildren,cd->name(),cd->getOutputFileBase());
        }
        if (hasFtvHelp)
        {
          FTVHelp::getInstance()->addContentsItem(hasChildren,cd->getReference(),cd->getOutputFileBase(),0,cd->name());
        }
      }
      if (hasChildren)
      {
        writeClassTree(cd->subClasses());
      }
      cd->visited=TRUE;
    }
}

void writeClassTree(ClassList *cl)
{
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
  ClassListIterator cli(*cl);
  bool started=FALSE;
  for ( ; cli.current() ; ++cli)
  {
    writeClassTreeNode(cli.current(),hasHtmlHelp,hasFtvHelp,started);
  }
}

void writeClassTree(ClassSDict *d)
{
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
  ClassSDict::Iterator cli(*d);
  bool started=FALSE;
  for ( ; cli.current() ; ++cli)
  {
    writeClassTreeNode(cli.current(),hasHtmlHelp,hasFtvHelp,started);
  }
}

//----------------------------------------------------------------------------

static void writeClassTreeForList(OutputList &ol,ClassSDict *cl,bool &started)
{
  HtmlHelp *htmlHelp=0;
  FTVHelp  *ftvHelp=0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
  }

  ClassSDict::Iterator cli(*cl);
  for (;cli.current(); ++cli)
  {
    ClassDef *cd=cli.current();
    //printf("class %s hasVisibleRoot=%d isVisibleInHierarchy=%d\n",
    //             cd->name().data(),
    //              hasVisibleRoot(cd->baseClasses()),
    //              cd->isVisibleInHierarchy()
    //      );
    if (!hasVisibleRoot(cd->baseClasses())) // filter on root classes
    {
      if (cd->isVisibleInHierarchy()) // should it be visible
      {
        if (!started)
        {
          ol.startIndexList();
          if (hasHtmlHelp) htmlHelp->incContentsDepth();
          if (hasFtvHelp)  ftvHelp->incContentsDepth();
          started=TRUE;
        }
        bool hasChildren = !cd->visited && cd->subClasses()->count()>0; 
        if (cd->isLinkable())
        {
          //printf("Writing class %s\n",cd->displayName().data());
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
          writeClassTree(ol,cd->subClasses(),cd->visited);
          cd->visited=TRUE;
        }
      }
    }
  }
}

void writeClassHierarchy(OutputList &ol)
{
  HtmlHelp *htmlHelp=0;
  FTVHelp  *ftvHelp=0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
  }

  initClassHierarchy(&Doxygen::classSDict);
  initClassHierarchy(&Doxygen::hiddenClasses);

  bool started=FALSE;
  writeClassTreeForList(ol,&Doxygen::classSDict,started);
  writeClassTreeForList(ol,&Doxygen::hiddenClasses,started);
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
  initClassHierarchy(&Doxygen::classSDict);
  int count=0;
  ClassSDict::Iterator cli(Doxygen::classSDict);
  for ( ; cli.current(); ++cli)
  {
    if (cli.current()->subClasses()->count()>0) count++;
  }
  return count;
}

//----------------------------------------------------------------------------

void writeHierarchicalIndex(OutputList &ol)
{
  if (hierarchyClasses==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"hierarchy",0,"Hierarchical Index");
  startTitle(ol,0);
  QCString title = theTranslator->trClassHierarchy();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp =  generateHtml && Config_getBool("GENERATE_TREEVIEW");
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(TRUE,htmlHelpTitle,"hierarchy"); 
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(TRUE,0,"hierarchy",0,ftvHelpTitle); 
  }
  if (Config_getBool("HAVE_DOT") && Config_getBool("GRAPHICAL_HIERARCHY"))
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
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeGraphicalClassHierarchy(OutputList &ol)
{
  if (hierarchyClasses==0) return;
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,"inherits",0,"Graphical Class Hierarchy");
  startTitle(ol,0);
  QCString title = theTranslator->trGraphicalHierarchy();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp =  generateHtml && Config_getBool("GENERATE_TREEVIEW");
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(FALSE,htmlHelpTitle,"inherits"); 
  }
  if (hasFtvHelp)
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
  FileNameListIterator fnli(Doxygen::inputNameList);
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
  startFile(ol,"files",0,"File Index");
  startTitle(ol,0);
  QCString title = theTranslator->trFileList();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp =  generateHtml && Config_getBool("GENERATE_TREEVIEW");
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
  parseText(ol,theTranslator->trFileListDescription(Config_getBool("EXTRACT_ALL")));
  //ol.newParagraph();
  ol.endTextBlock();

  OutputNameDict outputNameDict(1009);
  OutputNameList outputNameList;
  outputNameList.setAutoDelete(TRUE);
  
  if (Config_getBool("FULL_PATH_NAMES"))
  {
    // re-sort input files in (dir,file) output order instead of (file,dir) input order 
    FileName *fn=Doxygen::inputNameList.first();
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
      fn=Doxygen::inputNameList.next();
    }
  }
  
  ol.startIndexList();
  FileList *fl=0;
  if (Config_getBool("FULL_PATH_NAMES"))
  {
    fl = outputNameList.first();
  }
  else
  {
    fl = Doxygen::inputNameList.first();
  }
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
        if (Config_getBool("FULL_PATH_NAMES")) 
        {
          path=stripFromPath(fd->getPath().copy());
        }
        QCString fullName=fd->name();
        if (!path.isEmpty()) fullName.prepend(path+"/");

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
            htmlHelp->addContentsItem(FALSE,fullName,fd->getOutputFileBase());
          }
          if (hasFtvHelp)
          {
            ftvHelp->addContentsItem(FALSE,fd->getReference(),fd->getOutputFileBase(),0,fullName);
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
    if (Config_getBool("FULL_PATH_NAMES"))
    {
      fl=outputNameList.next();
    }
    else
    {
      fl=Doxygen::inputNameList.next();
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
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------
int countNamespaces()
{
  int count=0;
  NamespaceListIterator nli(Doxygen::namespaceList);
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
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"namespaces",0,"Namespace Index");
  startTitle(ol,0);
  QCString title = theTranslator->trNamespaceList();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp  = 0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp =  generateHtml && Config_getBool("GENERATE_TREEVIEW");
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
  parseText(ol,theTranslator->trNamespaceListDescription(Config_getBool("EXTRACT_ALL")));
  //ol.newParagraph();
  ol.endTextBlock();

  bool first=TRUE;
  NamespaceDef *nd=Doxygen::namespaceList.first();
  while (nd)
  {
    if (nd->isLinkableInProject())
    {
      if (first)
      {
        ol.startIndexList();
        first=FALSE;
      }
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
    nd=Doxygen::namespaceList.next();
  }
  if (!first) ol.endIndexList();
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp->decContentsDepth();
  }
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

int countAnnotatedClasses()
{
  int count=0;
  //ClassDef *cd=Doxygen::classList.first();
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd;
  for (;(cd=cli.current());++cli)
  {
    if (cd->isLinkableInProject() && cd->templateMaster()==0) 
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
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp =  generateHtml && Config_getBool("GENERATE_TREEVIEW");
  ol.startIndexList(); 
  //ClassDef *cd=Doxygen::classList.first();
  //while (cd)
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd;
  for (;(cd=cli.current());++cli)
  {
    if (cd->isLinkableInProject() && cd->templateMaster()==0)
    {
      QCString type=cd->compoundTypeString();
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
  }
  ol.endIndexList();
}

//----------------------------------------------------------------------------

void writePackageList(OutputList &ol)
{
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp =  generateHtml && Config_getBool("GENERATE_TREEVIEW");
  ol.startIndexList(); 
  PackageSDict::Iterator pdi(Doxygen::packageDict);
  PackageDef *pd;
  for (;(pd=pdi.current());++pdi)
  {
    if (!pd->isReference())
    {
      ol.writeStartAnnoItem("package",pd->getOutputFileBase(),0,pd->name());
      if (!pd->briefDescription().isEmpty())
      {
        ol.docify(" (");
        OutputList briefOutput(&ol);
        parseDoc(briefOutput,
            pd->getDefFileName(),pd->getDefLine(),
            pd->name(),0,
            abbreviate(pd->briefDescription(),pd->name()));
        ol+=briefOutput;
        ol.docify(")");
      }
      ol.writeEndAnnoItem(pd->getOutputFileBase());
      if (hasHtmlHelp)
      {
        HtmlHelp::getInstance()->addContentsItem(FALSE,pd->name(),pd->getOutputFileBase());
      }
      if (hasFtvHelp)
      {
        FTVHelp::getInstance()->addContentsItem(FALSE,pd->getReference(),pd->getOutputFileBase(),0,pd->name());
      }
    }
  }
  ol.endIndexList();
}

//----------------------------------------------------------------------------

// write an alphabetical index of all class with a header for each letter
void writeAlphabeticalClassList(OutputList &ol)
{
  ol.startAlphabeticalIndexList(); 

  // first count the number of headers
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd;
  char startLetter=0;
  int headerItems=0;
  for (;(cd=cli.current());++cli)
  {
    if (cd->isLinkableInProject() && cd->templateMaster()==0)
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
  const int columns = Config_getInt("COLS_IN_ALPHA_INDEX");

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
    if (cd->isLinkableInProject() && cd->templateMaster()==0)
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
        if (Config_getBool("HIDE_SCOPE_NAMES"))
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
        ol.writeNonBreakableSpace(3);
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
  startFile(ol,"classes.html",0,"Alphabetical index");
  startTitle(ol,0);
  parseText(ol,Config_getString("PROJECT_NAME")+" "+theTranslator->trCompoundIndex());
  endTitle(ol,0,0);
  writeAlphabeticalClassList(ol);
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeAnnotatedIndex(OutputList &ol)
{
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");

  if (annotatedClasses==0) return;
  
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"annotated",0,"Annotated Index");
  startTitle(ol,0);
  QCString title = theTranslator->trCompoundList();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle =  title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
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
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writePackageIndex(OutputList &ol)
{
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");

  if (documentedPackages==0) return;
  
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"packages",0,"Package Index");
  startTitle(ol,0);
  QCString title = theTranslator->trPackageList();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle =  title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(TRUE,htmlHelpTitle,"packages"); 
    htmlHelp->incContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(TRUE,0,"packages",0,ftvHelpTitle); 
    ftvHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trPackageListDescription());
  ol.endTextBlock();
  writePackageList(ol);
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  if (hasFtvHelp)
  {
    ftvHelp->decContentsDepth();
  }
  
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeMemberList(OutputList &ol,bool useSections)
{
  bool first = TRUE;
  char lastChar = 0;
  MemberName *mn=Doxygen::memberNameList.first();
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
           cd->isLinkableInProject() && cd->templateMaster()==0
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
            cd->isLinkableInProject() && cd->templateMaster()==0
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
    mn=Doxygen::memberNameList.next();
  }
  ol.endItemList();
}

//----------------------------------------------------------------------------

int countClassMembers()
{
  int i=0;for (i=0;i<256;i++) g_memberIndexLetterUsed[i]=FALSE;
  int count=0;
  MemberName *mn=Doxygen::memberNameList.first();
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
    mn=Doxygen::memberNameList.next();
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
  startFile(ol,"functions",0,"Compound Member Index");
  QCString title = theTranslator->trCompoundMembers();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle =  title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  startTitle(ol,0);
  parseText(ol,title);
  endTitle(ol,0,0);
  bool quickIndex = documentedMembers>maxItemsBeforeQuickIndex;
  if (quickIndex)
  {
    writeQuickMemberIndex(ol,g_memberIndexLetterUsed);
  }
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp =  generateHtml && Config_getBool("GENERATE_TREEVIEW");
  if (hasHtmlHelp)
  {
    HtmlHelp *htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(FALSE,htmlHelpTitle,"functions"); 
  }
  if (hasFtvHelp)
  {
    FTVHelp *ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(FALSE,0,"functions",0,ftvHelpTitle); 
  }
  parseText(ol,theTranslator->trCompoundMembersDescription(Config_getBool("EXTRACT_ALL")));
  writeMemberList(ol,quickIndex);
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeFileMemberList(OutputList &ol,bool useSections)
{
  char lastChar=0;
  bool first=TRUE;
  MemberName *mn=Doxygen::functionNameList.first();
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
    mn=Doxygen::functionNameList.next();
  }
  ol.endItemList();
}

//----------------------------------------------------------------------------

void writeNamespaceMemberList(OutputList &ol,bool useSections)
{
  char lastChar=0;
  bool first=TRUE;
  MemberName *mn=Doxygen::functionNameList.first();
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
      else if (first)
      {
        ol.startItemList();
        first=FALSE;
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
    mn=Doxygen::functionNameList.next();
  }
  if (!first) ol.endItemList();
}

//----------------------------------------------------------------------------

int countNamespaceMembers()
{
  int i=0;for (i=0;i<256;i++) g_namespaceIndexLetterUsed[i]=FALSE;
  int count=0;
  MemberName *mn=Doxygen::functionNameList.first();
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
    mn=Doxygen::functionNameList.next();
  }
  return count;
}

//----------------------------------------------------------------------------

int countFileMembers()
{
  int i=0;for (i=0;i<256;i++) g_fileIndexLetterUsed[i]=FALSE;
  int count=0;
  MemberName *mn=Doxygen::functionNameList.first();
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
    mn=Doxygen::functionNameList.next();
  }
  return count;
}

//----------------------------------------------------------------------------

void writeFileMemberIndex(OutputList &ol)
{
  if (documentedFunctions==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,"globals",0,"File Member Index");
  QCString title = theTranslator->trFileMembers();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle =  title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  startTitle(ol,0);
  parseText(ol,title);
  endTitle(ol,0,0);
  bool quickIndex = documentedMembers>maxItemsBeforeQuickIndex;
  if (quickIndex)
  {
    writeQuickMemberIndex(ol,g_fileIndexLetterUsed);
  }
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp =  generateHtml && Config_getBool("GENERATE_TREEVIEW");
  if (hasHtmlHelp)
  {
    HtmlHelp *htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(FALSE,htmlHelpTitle,"globals"); 
  }
  if (hasFtvHelp)
  {
    FTVHelp *ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(FALSE,0,"globals",0,ftvHelpTitle); 
  }
  parseText(ol,theTranslator->trFileMembersDescription(Config_getBool("EXTRACT_ALL")));
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
  startFile(ol,"namespacemembers",0,"Namespace Member Index");
  QCString title = theTranslator->trNamespaceMembers();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle =  title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  startTitle(ol,0);
  parseText(ol,title);
  endTitle(ol,0,0);
  bool quickIndex = documentedMembers>maxItemsBeforeQuickIndex;
  if (quickIndex)
  {
    writeQuickMemberIndex(ol,g_namespaceIndexLetterUsed);
  }
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp =  generateHtml && Config_getBool("GENERATE_TREEVIEW");
  if (hasHtmlHelp)
  {
    HtmlHelp *htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(FALSE,htmlHelpTitle,"namespacemembers"); 
  }
  if (hasFtvHelp)
  {
    FTVHelp *ftvHelp = FTVHelp::getInstance();
    ftvHelp->addContentsItem(FALSE,0,"namespacemembers",0,ftvHelpTitle); 
  }
  parseText(ol,theTranslator->trNamespaceMemberDescription(Config_getBool("EXTRACT_ALL")));
  writeNamespaceMemberList(ol,quickIndex);
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeExampleIndex(OutputList &ol)
{
  if (Doxygen::exampleSDict->count()==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"examples",0,"Example Index");
  startTitle(ol,0);
  QCString title = theTranslator->trExamples();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
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
  PageSDictIterator pdi(*Doxygen::exampleSDict);
  PageInfo *pi=0;
  for (pdi.toFirst();(pi=pdi.current());++pdi)
  {
    ol.writeListItem();
    QCString n=convertNameToFile(pi->name+"-example");
    if (!pi->title.isEmpty())
    {
      ol.writeObjectLink(0,n,0,pi->title);
      if (hasHtmlHelp) htmlHelp->addContentsItem(FALSE,pi->title,n);
      if (hasFtvHelp)  ftvHelp->addContentsItem(FALSE,pi->getReference(),n,0,pi->title);
    }
    else
    {
      ol.writeObjectLink(0,n,0,pi->name);
      if (hasHtmlHelp) htmlHelp->addContentsItem(FALSE,pi->name,n);
      if (hasFtvHelp)  ftvHelp->addContentsItem(FALSE,pi->getReference(),n,0,pi->name);
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
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void countRelatedPages(int &docPages,int &indexPages)
{
  docPages=indexPages=0;
  PageSDictIterator pdi(*Doxygen::pageSDict);
  PageInfo *pi=0;
  for (pdi.toFirst();(pi=pdi.current());++pdi)
  {
    if (!pi->inGroup && (!pi->isReference() || Config_getBool("ALLEXTERNALS")))
    {
      indexPages++;
      if (!pi->isReference()) docPages++;
    }
  }
}

//----------------------------------------------------------------------------

int countPackages()
{
  int count=0;
  PackageSDict::Iterator pdi(Doxygen::packageDict);
  PackageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    if (!pd->isReference())
    {
      count++;
    }
  }
  return count;
}

//----------------------------------------------------------------------------

void writePageIndex(OutputList &ol)
{
  if (indexedPages==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"pages",0,"Page Index");
  startTitle(ol,0);
  QCString title = theTranslator->trRelatedPages();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
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
  PageSDictIterator pdi(*Doxygen::pageSDict);
  PageInfo *pi=0;
  for (pdi.toFirst();(pi=pdi.current());++pdi)
  {
    if (!pi->inGroup && (!pi->isReference() || Config_getBool("ALLEXTERNALS")))
    {
      QCString pageName,pageTitle;

      if (Config_getBool("CASE_SENSE_NAMES"))
        pageName=pi->name.copy();
      else
        pageName=pi->name.lower();

      if (pi->title.isEmpty())
        pageTitle=pi->name;
      else
        pageTitle=pi->title;

      ol.writeIndexItem(pi->getReference(),pi->getOutputFileBase(),pageTitle);
      if (pi->isReference()) 
      { 
        ol.startTypewriter(); 
        ol.docify(" [external]");
        ol.endTypewriter();
      }
      //ol.writeStartAnnoItem("pages",pageName,0,pageTitle);
      ////ol.writeObjectLink(0,pageName,0,pageTitle);
      //ol.writeEndAnnoItem(pageName);
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
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

int countGroups()
{
  int count=0;
  GroupListIterator gli(Doxygen::groupList);
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
  if (!Config_getBool("HAVE_DOT") || !Config_getBool("GENERATE_HTML")) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  generateGraphLegend(Config_getString("HTML_OUTPUT"));
  startFile(ol,"graph_legend",0,"Graph Legend");
  startTitle(ol,0);
  parseText(ol,theTranslator->trLegendTitle());
  endTitle(ol,0,0);
  bool oldStripCommentsState = Config_getBool("STRIP_CODE_COMMENTS");
  // temporarily disable the stripping of comments for our own code example!
  Config_getBool("STRIP_CODE_COMMENTS") = FALSE;
  parseDoc(ol,"graph_legend",1,0,0,theTranslator->trLegendDocs());
  Config_getBool("STRIP_CODE_COMMENTS") = oldStripCommentsState;
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------
/*!
 * write groups as hierarchial trees
 * \author KPW
 */

void writeGroupTreeNode(OutputList &ol, GroupDef *gd,bool subLevel)
{
  HtmlHelp *htmlHelp=0;
  FTVHelp  *ftvHelp = 0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
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
  if (!gd->visited && (!gd->isASubGroup() || subLevel))
  {
    //printf("gd->name()=%s #members=%d\n",gd->name().data(),gd->countMembers());
    // write group info
    bool hasSubGroups = gd->groupList->count()>0;
    bool hasSubPages = gd->pageDict->count()>0;
    int numSubItems = 0;
    if( Config_getBool("TOC_EXPAND"))
    {
      numSubItems += gd->docDefineMembers.count();
      numSubItems += gd->docTypedefMembers.count();
      numSubItems += gd->docEnumMembers.count();
      numSubItems += gd->docEnumValMembers.count();
      numSubItems += gd->docFuncMembers.count();
      numSubItems += gd->docVarMembers.count();
      numSubItems += gd->docProtoMembers.count();
      numSubItems += gd->namespaceList->count();
      numSubItems += gd->classSDict->count();
      numSubItems += gd->fileList->count();
      numSubItems += gd->exampleDict->count();
    }

    bool isDir = hasSubGroups || hasSubPages || numSubItems>0;
    //printf("gd=`%s': pageDict=%d\n",gd->name().data(),gd->pageDict->count());
    if(htmlHelp)
    {
        htmlHelp->addContentsItem(isDir,gd->groupTitle(),gd->getOutputFileBase()); 
        htmlHelp->incContentsDepth();
    }
    if(ftvHelp)
    {
        ftvHelp->addContentsItem(isDir,gd->getReference(),gd->getOutputFileBase(),
                                 0,gd->groupTitle()); 
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
      if (!pi->name.isEmpty()) si=Doxygen::sectionDict[pi->name];
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
        writeGroupTreeNode(ol,subgd,TRUE);
      }
      ol.endIndexList(); 
    }


    if (Config_getBool("TOC_EXPAND"))
    {
      // write members
      struct MemInfo
      {
        MemInfo(MemberList *l,const QCString &n) { list=l; name=n; }
        MemberList *list;
        QCString name;
      };
      MemInfo memberLists[] = 
      {
        MemInfo(&gd->docDefineMembers,  theTranslator->trDefines()),
        MemInfo(&gd->docTypedefMembers, theTranslator->trTypedefs()),
        MemInfo(&gd->docEnumMembers,    theTranslator->trEnumerations()),
        MemInfo(&gd->docEnumValMembers, theTranslator->trEnumerationValues()),
        MemInfo(&gd->docFuncMembers,    theTranslator->trFunctions()),
        MemInfo(&gd->docVarMembers,     theTranslator->trVariables()),
        MemInfo(&gd->docProtoMembers,   theTranslator->trFuncProtos()),
        MemInfo(0,0)
      };

      MemberList *members;
      MemInfo *pMemInfo;
      for (pMemInfo=&memberLists[0]; (members=pMemInfo->list) ; pMemInfo++)
      {
        if (members->count()>0)
        {
          if(htmlHelp)
          {
            htmlHelp->addContentsItem(TRUE, convertToHtml(pMemInfo->name), gd->getOutputFileBase(),0);
            htmlHelp->incContentsDepth();
          }

          if(ftvHelp)
          {

            ftvHelp->addContentsItem(TRUE, gd->getReference(), gd->getOutputFileBase(), 0, pMemInfo->name);
            ftvHelp->incContentsDepth();
          }

          MemberDef *md=members->first();
          while (md)
          {
            GroupDef *gd=md->getGroupDef();
            if(htmlHelp)
            {
              htmlHelp->addContentsItem(FALSE,md->name(),gd->getOutputFileBase(),md->anchor()); 
            }
            if(ftvHelp)
            {
              ftvHelp->addContentsItem(FALSE,gd->getReference(),gd->getOutputFileBase(),md->anchor(),md->name()); 
            }
            md=members->next();
          }

          if(htmlHelp) htmlHelp->decContentsDepth();
          if(ftvHelp)  ftvHelp->decContentsDepth();

        }
      }

      // write namespaces
      NamespaceList *namespaceList=gd->namespaceList;
      if (namespaceList->count()>0)
      {
        if(htmlHelp)
        {
          htmlHelp->addContentsItem(TRUE, convertToHtml(theTranslator->trNamespaces()), gd->getOutputFileBase(), 0);
          htmlHelp->incContentsDepth();
        }

        if(ftvHelp)
        {

          ftvHelp->addContentsItem(TRUE, gd->getReference(), gd->getOutputFileBase(), 0, theTranslator->trNamespaces());
          ftvHelp->incContentsDepth();
        }

        NamespaceDef *nsd=namespaceList->first();
        while (nsd)
        {
          if(htmlHelp)
          {
            htmlHelp->addContentsItem(FALSE, convertToHtml(nsd->name()), nsd->getOutputFileBase());
          }
          if(ftvHelp)
          {
            ftvHelp->addContentsItem(FALSE, nsd->getReference(), nsd->getOutputFileBase(), 0, convertToHtml(nsd->name()));
          }
          nsd=namespaceList->next();
        }
        if (htmlHelp) htmlHelp->decContentsDepth();
        if (ftvHelp)  ftvHelp->decContentsDepth();
      }

      // write classes
      if(gd->classSDict->count()>0)
      {
        if(htmlHelp)
        {
          htmlHelp->addContentsItem(TRUE, convertToHtml(theTranslator->trClasses()), gd->getOutputFileBase(), 0);
          htmlHelp->incContentsDepth();
        }

        if(ftvHelp)
        {

          ftvHelp->addContentsItem(TRUE, gd->getReference(), gd->getOutputFileBase(), 0, theTranslator->trClasses());
          ftvHelp->incContentsDepth();
        }

        writeClassTree(gd->classSDict);
        if(htmlHelp) htmlHelp->decContentsDepth();
        if(ftvHelp)  ftvHelp->decContentsDepth();
      }

      // write file list
      FileList *fileList=gd->fileList;
      if (fileList->count()>0)
      {
        if(htmlHelp)
        {
          htmlHelp->addContentsItem(TRUE, 
              convertToHtml(theTranslator->trFile(TRUE,FALSE)), 
              gd->getOutputFileBase(), 0);
          htmlHelp->incContentsDepth();
        }

        if(ftvHelp)
        {

          ftvHelp->addContentsItem(TRUE, gd->getReference(), 
              gd->getOutputFileBase(), 0, 
              theTranslator->trFile(TRUE,FALSE));
          ftvHelp->incContentsDepth();
        }

        FileDef *fd=fileList->first();
        while (fd)
        {
          if(htmlHelp)
            htmlHelp->addContentsItem(FALSE,convertToHtml(fd->name()),fd->getOutputFileBase());
          if(ftvHelp)
            ftvHelp->addContentsItem(FALSE, fd->getReference(), fd->getOutputFileBase(), 0, convertToHtml(fd->name()));
          fd=fileList->next();
        }
        if(htmlHelp)
          htmlHelp->decContentsDepth();
        if(ftvHelp)
          ftvHelp->decContentsDepth();
      }

      // write examples
      if (gd->exampleDict->count()>0)
      {
        if(htmlHelp)
        {
          htmlHelp->addContentsItem(TRUE, convertToHtml(theTranslator->trExamples()), gd->getOutputFileBase(), 0);
          htmlHelp->incContentsDepth();
        }

        if(ftvHelp)
        {
          ftvHelp->addContentsItem(TRUE, gd->getReference(), gd->getOutputFileBase(), 0, theTranslator->trExamples());
          ftvHelp->incContentsDepth();
        }

        PageSDictIterator eli(*(gd->exampleDict));
        PageInfo *pi=eli.toFirst();
        while (pi)
        {
          if(htmlHelp)
          {
            htmlHelp->addContentsItem(FALSE,convertToHtml(pi->name),convertNameToFile(pi->name+"-example")); 
          }
          if(ftvHelp)
          {
            ftvHelp->addContentsItem(FALSE,pi->getReference(),convertToHtml(pi->name+"-example"),0,convertNameToFile(pi->name)); 
          }
          pi=++eli;
        }

        if (htmlHelp) htmlHelp->decContentsDepth();
        if (ftvHelp)  ftvHelp->decContentsDepth();
      }
    }
    
    if (htmlHelp) htmlHelp->decContentsDepth();
    if (ftvHelp)  ftvHelp->decContentsDepth();
    
    gd->visited=TRUE;
  }
}

void writeGroupHierarchy(OutputList &ol)
{
  ol.startIndexList();
  GroupListIterator gli(Doxygen::groupList);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
  {
    writeGroupTreeNode(ol,gd,FALSE);
  }
  ol.endIndexList(); 
}

//----------------------------------------------------------------------------

#if 0
void writeGroupList(OutputList &ol)
{
  bool hasHtmlHelp = Config_getBool("GENERATE_HTML") && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = Config_getBool("GENERATE_HTML") && Config_getBool("GENERATE_TREEVIEW");
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp  = 0;
  if (hasHtmlHelp) htmlHelp = HtmlHelp::getInstance();
  if (hasFtvHelp) ftvHelp = FTVHelp::getInstance();
  ol.startDescription();
  GroupListIterator gli(Doxygen::groupList);
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
        if(!Config::instance()->get(""))
        {
          htmlHelp->addContentsItem(FALSE,gd->groupTitle(),docFile);
        }
      }
      if (hasFtvHelp)
      {
        ftvHelp = FTVHelp::getInstance();
        if(!Config::instance()->get(""))
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
  startFile(ol,"modules",0,"Module Index");
  startTitle(ol,0);
  QCString title = theTranslator->trModules();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  FTVHelp  *ftvHelp = 0;
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
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
    //if(!Config::instance()->get(""))
    //{
        htmlHelp->decContentsDepth();
    //}
  }
  if (hasFtvHelp)
  {
    //writeGroupTree(ol);   // KPW - modified to write hierarchial FTV Help
    //if(!Config::instance()->get(""))
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
  if (!Config_getString("PROJECT_NAME").isEmpty())
  {
    projPrefix=Config_getString("PROJECT_NAME")+" ";
  }

  //--------------------------------------------------------------------
  // write HTML index
  //--------------------------------------------------------------------
  ol.disableAllBut(OutputGenerator::Html);

  QCString defFileName = 
    Doxygen::mainPage ? Doxygen::mainPage->defFileName.data() : "<generated>";
  int defLine =
    Doxygen::mainPage ? Doxygen::mainPage->defLine : 1;

  QCString title;
  if (!Doxygen::mainPage || Doxygen::mainPage->title.isEmpty())
  {
    title = theTranslator->trMainPage();
  }
  else 
  {
    title = substitute(Doxygen::mainPage->title,"%","");
  }

  QCString indexName="index";
  if (Config_getBool("GENERATE_TREEVIEW")) indexName="main";
  ol.startFile(indexName,0,title,FALSE);
  
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
  if (hasHtmlHelp)
  {
    HtmlHelp::getInstance()->addContentsItem(FALSE,title,indexName); 
  }
  if (hasFtvHelp)
  {
    FTVHelp::getInstance()->addContentsItem(FALSE,0,indexName,0,title); 
  }

  if (!Config_getBool("DISABLE_INDEX")) writeQuickLinks(ol,TRUE);
  ol.startTitleHead(0);
  if (Doxygen::mainPage && !Doxygen::mainPage->title.isEmpty())
  {
    parseDoc(ol,defFileName,defLine,0,0,Doxygen::mainPage->title);
  }
  else
  {
    parseText(ol,projPrefix+theTranslator->trDocumentation());
  }
  ol.endTitleHead(0,0);
  ol.newParagraph();
  if (!Config_getString("PROJECT_NUMBER").isEmpty())
  {
    ol.startProjectNumber();
    parseDoc(ol,defFileName,defLine,0,0,Config_getString("PROJECT_NUMBER"));
    ol.endProjectNumber();
  }
  if (Config_getBool("DISABLE_INDEX") && Doxygen::mainPage==0) writeQuickLinks(ol,FALSE);

  if (Doxygen::mainPage)
  {
    parseDoc(ol,defFileName,defLine,0,0,Doxygen::mainPage->doc);

    if (!Config_getString("GENERATE_TAGFILE").isEmpty())
    {
       Doxygen::tagFile << "  <compound kind=\"page\">" << endl
                        << "    <filename>"
                        << convertToXML(Doxygen::mainPage->fileName)
                        << "</filename>"
                        << endl
                        << "    <title>"
                        << convertToXML(Doxygen::mainPage->title)
                        << "</title>"
                        << endl
                        << "    <name>"
                        << convertToXML(Doxygen::mainPage->name)
                        << "</name>"
                        << endl;

       Doxygen::mainPage->writeDocAnchorsToTagFile();
       Doxygen::tagFile << "  </compound>" << endl;
    }
  }
  
  endFile(ol);
  ol.disable(OutputGenerator::Html);
  
  //--------------------------------------------------------------------
  // write LaTeX/RTF index
  //--------------------------------------------------------------------
  ol.enable(OutputGenerator::Latex);
  ol.enable(OutputGenerator::RTF);

  ol.startFile("refman",0,0,FALSE);
  ol.startIndexSection(isTitlePageStart);
  if (!Config_getString("LATEX_HEADER").isEmpty()) 
  {
    ol.disable(OutputGenerator::Latex);
  }

  parseText(ol,projPrefix+theTranslator->trReferenceManual());
  if (!Config_getString("PROJECT_NUMBER").isEmpty())
  {
    ol.startProjectNumber(); 
    parseDoc(ol,defFileName,defLine,0,0,Config_getString("PROJECT_NUMBER"));
    ol.endProjectNumber();
  }
  ol.endIndexSection(isTitlePageStart);
  ol.startIndexSection(isTitlePageAuthor);
  parseText(ol,theTranslator->trGeneratedBy());
  ol.endIndexSection(isTitlePageAuthor);
  ol.enable(OutputGenerator::Latex);

  if (Doxygen::mainPage)
  {
    ol.startIndexSection(isMainPage);
    if (!Doxygen::mainPage->title.isEmpty())
    {
      parseDoc(ol,defFileName,defLine,0,0,Doxygen::mainPage->title);
    }
    else
    {
      parseText(ol,projPrefix+theTranslator->trMainPage());
    }
    ol.endIndexSection(isMainPage);
  }

  
  if (documentedPackages>0)
  {
    ol.startIndexSection(isPackageIndex);
    parseText(ol,projPrefix+theTranslator->trPackageList());
    ol.endIndexSection(isPackageIndex);
  }
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
  if (indexedPages>0)
  {
    ol.startIndexSection(isPageIndex);
    parseText(ol,projPrefix+theTranslator->trPageIndex());
    ol.endIndexSection(isPageIndex);
  }
  ol.lastIndexPage();
  if (documentedPackages>0)
  {
    ol.startIndexSection(isPackageDocumentation);
    parseText(ol,projPrefix+theTranslator->trPackageDocumentation());
    ol.endIndexSection(isPackageDocumentation);
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
  if (Doxygen::exampleSDict->count()>0)
  {
    ol.startIndexSection(isExampleDocumentation);
    parseText(ol,projPrefix+theTranslator->trExampleDocumentation());
    ol.endIndexSection(isExampleDocumentation);
  }
  if (documentedPages>0)
  {
    ol.startIndexSection(isPageDocumentation);
    parseText(ol,projPrefix+theTranslator->trPageDocumentation());
    ol.endIndexSection(isPageDocumentation);
  }
  ol.endIndexSection(isEndIndex);
  endFile(ol);

  if (Doxygen::mainPage)
  {
    ol.disable(OutputGenerator::Man);
    startFile(ol,Doxygen::mainPage->name,0,Doxygen::mainPage->title);
    //SectionInfo *si=0;
    //if (!Doxygen::mainPage->title.isEmpty() && !Doxygen::mainPage->name.isEmpty() &&
    //    (si=Doxygen::sectionDict[Doxygen::mainPage->name])!=0)
    //{
    //  ol.startSection(si->label,si->title,FALSE);
    //  ol.docify(si->title);
    //  ol.endSection(si->label,FALSE);
    //}
    ol.startTextBlock();
    parseDoc(ol,defFileName,defLine,0,0,Doxygen::mainPage->doc);
    ol.endTextBlock();
    endFile(ol);
    ol.enable(OutputGenerator::Man);
  }

  ol.popGeneratorState();
}

