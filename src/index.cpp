/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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
#include <qregexp.h>

#include "message.h"
#include "index.h"
#include "doxygen.h"
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
#include "pagedef.h"

int annotatedClasses;
int hierarchyClasses;
int documentedFiles;
int documentedGroups;
int documentedNamespaces;
int indexedPages;
int documentedClassMembers[CMHL_Total];
int documentedFileMembers[FMHL_Total];
int documentedNamespaceMembers[NMHL_Total];
int documentedHtmlFiles;
int documentedPages;

int countClassHierarchy();
int countClassMembers(int filter=CMHL_All);
int countFileMembers(int filter=FMHL_All);
void countFiles(int &htmlFiles,int &files);
int countGroups();
int countNamespaces();
int countAnnotatedClasses();
int countNamespaceMembers(int filter=NMHL_All);
int countIncludeFiles();
void countRelatedPages(int &docPages,int &indexPages);

void countDataStructures()
{
  annotatedClasses           = countAnnotatedClasses();
  hierarchyClasses           = countClassHierarchy();
  countFiles(documentedHtmlFiles,documentedFiles);
  countRelatedPages(documentedPages,indexedPages);
  documentedGroups           = countGroups();
  documentedNamespaces       = countNamespaces();
  int i;
  for (i=0;i<(int)CMHL_Total;i++)
  {
    documentedClassMembers[i] = countClassMembers(i);
  }
  for (i=0;i<(int)FMHL_Total;i++)
  {
    documentedFileMembers[i] = countFileMembers(i);
  }
  for (i=0;i<(int)NMHL_Total;i++)
  {
    documentedNamespaceMembers[i] = countNamespaceMembers(i);
  }
}

static void startIndexHierarchy(OutputList &ol,int level)
{
  // UGLY HACK!
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  ol.disable(OutputGenerator::Html);
  if (level<6) ol.startIndexList();
  ol.enableAll();
  ol.disable(OutputGenerator::Latex);
  ol.disable(OutputGenerator::RTF);
  ol.startItemList();
  ol.popGeneratorState();
}

static void endIndexHierarchy(OutputList &ol,int level)
{
  // UGLY HACK!
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  ol.disable(OutputGenerator::Html);
  if (level<6) ol.endIndexList();
  ol.enableAll();
  ol.disable(OutputGenerator::Latex);
  ol.disable(OutputGenerator::RTF);
  ol.endItemList();
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

static bool g_memberIndexLetterUsed[CMHL_Total][256];
static bool g_fileIndexLetterUsed[FMHL_Total][256];
static bool g_namespaceIndexLetterUsed[NMHL_Total][256];

const int maxItemsBeforeQuickIndex = 30;

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
  QCString scopelessName=name;
  int i=scopelessName.findRev("::");
  if (i!=-1) scopelessName=scopelessName.mid(i);
  QCString result=s;
  result=result.stripWhiteSpace();
  // strip trailing .
  if (!result.isEmpty() && result.at(result.length()-1)=='.') 
    result=result.left(result.length()-1);

  // strip any predefined prefix
  QStrList &briefDescAbbrev = Config_getList("ABBREVIATE_BRIEF");
  const char *p = briefDescAbbrev.first();
  while (p)
  {
    QCString s = p;
    s.replace(QRegExp("\\$name"), scopelessName);  // replace $name with entity name
    s += " ";
    stripWord(result,s);
    p = briefDescAbbrev.next();
  }

  // capitalize first word
  if (!result.isEmpty())
  {
    int c=result[0];
    if (c>='a' && c<='z') c+='A'-'a';
    result[0]=c;
  }
  return result;
}

//----------------------------------------------------------------------------

static void startQuickIndexItem(OutputList &ol,const char *l,
                                bool hl,bool compact,bool &first)
{
  if (!first && compact) ol.writeString(" | ");
  first=FALSE;
  if (!compact) ol.writeString("<li>");
  if (hl && compact)
  {
    ol.writeString("<a class=\"qindexHL\" ");
  }
  else
  {
    ol.writeString("<a class=\"qindex\" ");
  }
  ol.writeString("href=\""); 
  ol.writeString(l);
  ol.writeString("\">");
}

static void endQuickIndexItem(OutputList &ol)
{
  ol.writeString("</a>");
}


static QCString fixSpaces(const QCString &s)
{
  return substitute(s," ","&nbsp;");
}

//void writeQuickLinks(OutputList &ol,bool compact,HighlightedItem hli,bool ext=FALSE)
//{
//  bool first=TRUE;
//  ol.pushGeneratorState();
//  ol.disableAllBut(OutputGenerator::Html);
//  QCString extLink;
//  if (ext) { extLink="_doc"; }
//  if (compact) 
//  {
//    ol.writeString("<div class=\"qindex\">"); 
//  }
//  else 
//  {
//    ol.startItemList();
//  }
//
//  if (Config_getBool("SEARCHENGINE"))
//  {
//    ol.writeString("  <form class=\"search\" action=\"search.php\" method=\"get\">\n");
//  }
//
//  if (Config_getBool("GENERATE_TREEVIEW"))
//  {
//    startQuickIndexItem(ol,extLink,"main"+Doxygen::htmlFileExtension,
//                        hli==HLI_Main,compact,first);
//  }
//  else
//  {
//    startQuickIndexItem(ol,extLink,"index"+Doxygen::htmlFileExtension,
//                        hli==HLI_Main,compact,first);
//  }
//  ol.parseText(fixSpaces(theTranslator->trMainPage()));
//  endQuickIndexItem(ol);
//
//  if (documentedGroups>0)
//  {
//    startQuickIndexItem(ol,extLink,"modules"+Doxygen::htmlFileExtension,
//                        hli==HLI_Modules,compact,first);
//    ol.parseText(fixSpaces(theTranslator->trModules()));
//    endQuickIndexItem(ol);
//  } 
//  if (documentedNamespaces>0)
//  {
//    startQuickIndexItem(ol,extLink,"namespaces"+Doxygen::htmlFileExtension,
//                        hli==HLI_Namespaces,compact,first);
//    if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
//    {
//      ol.parseText(fixSpaces(theTranslator->trPackages()));
//    }
//    else
//    {
//      ol.parseText(theTranslator->trNamespaceList());
//    }
//    endQuickIndexItem(ol);
//  }
//  if (hierarchyClasses>0)
//  {
//    startQuickIndexItem(ol,extLink,"hierarchy"+Doxygen::htmlFileExtension,
//                           hli==HLI_Hierarchy,compact,first);
//    ol.parseText(fixSpaces(theTranslator->trClassHierarchy()));
//    endQuickIndexItem(ol);
//  } 
//  if (annotatedClasses>0)
//  {
//    if (Config_getBool("ALPHABETICAL_INDEX"))
//    {
//      startQuickIndexItem(ol,extLink,"classes"+Doxygen::htmlFileExtension,
//                          hli==HLI_Classes,compact,first);
//      ol.parseText(fixSpaces(theTranslator->trAlphabeticalList()));
//      endQuickIndexItem(ol);
//    }
//    if (!compact) ol.writeListItem();
//    startQuickIndexItem(ol,extLink,"annotated"+Doxygen::htmlFileExtension,
//                        hli==HLI_Annotated,compact,first);
//    ol.parseText(fixSpaces(theTranslator->trCompoundList()));
//    endQuickIndexItem(ol);
//  } 
//  if (documentedHtmlFiles>0)
//  {
//    startQuickIndexItem(ol,extLink,"files"+Doxygen::htmlFileExtension,
//                        hli==HLI_Files,compact,first);
//    ol.parseText(fixSpaces(theTranslator->trFileList()));
//    endQuickIndexItem(ol);
//  } 
//  if (documentedNamespaceMembers[NMHL_All]>0)
//  {
//    startQuickIndexItem(ol,extLink,"namespacemembers"+Doxygen::htmlFileExtension,
//                        hli==HLI_NamespaceMembers,compact,first);
//    ol.parseText(fixSpaces(theTranslator->trNamespaceMembers()));
//    endQuickIndexItem(ol);
//  }
//  if (documentedClassMembers[CMHL_All]>0)
//  {
//    startQuickIndexItem(ol,extLink,"functions"+Doxygen::htmlFileExtension,
//                        hli==HLI_Functions,compact,first);
//    ol.parseText(fixSpaces(theTranslator->trCompoundMembers()));
//    endQuickIndexItem(ol);
//  } 
//  if (documentedFileMembers[FMHL_All]>0)
//  {
//    startQuickIndexItem(ol,extLink,"globals"+Doxygen::htmlFileExtension,
//                        hli==HLI_Globals,compact,first);
//    ol.parseText(fixSpaces(theTranslator->trFileMembers()));
//    endQuickIndexItem(ol);
//  } 
//  if (indexedPages>0)
//  {
//    startQuickIndexItem(ol,extLink,"pages"+Doxygen::htmlFileExtension,
//                        hli==HLI_Pages,compact,first);
//    ol.parseText(fixSpaces(theTranslator->trRelatedPages()));
//    endQuickIndexItem(ol);
//  } 
//  if (Doxygen::exampleSDict->count()>0)
//  {
//    startQuickIndexItem(ol,extLink,"examples"+Doxygen::htmlFileExtension,
//                        hli==HLI_Examples,compact,first);
//    ol.parseText(fixSpaces(theTranslator->trExamples()));
//    endQuickIndexItem(ol);
//  } 
//  if (Config_getBool("SEARCHENGINE"))
//  {
//    //startQuickIndexItem(ol,"_cgi","",hli==HLI_Search,compact,first);
//    //ol.parseText(fixSpaces(theTranslator->trSearch()));
//    //endQuickIndexItem(ol);
//    ol.writeString("  | <span class=\"search\"><u>S</u>earch for "
//                   "<input class=\"search\" type=\"text\" name=\"query\" value=\"\" size=\"30\" accesskey=\"s\"/>"
//                   "</span>");
//  } 
//  if (compact) 
//  {
//    ol.writeString("</div>\n");
//  }
//  else 
//  {
//    ol.endItemList();
//  }
//  ol.popGeneratorState();
//}

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
               const char *title,HighlightedItem hli)
{
  ol.startFile(name,manName,title);
  if (!Config_getBool("DISABLE_INDEX")) ol.writeQuickLinks(TRUE,hli);
}

void endFile(OutputList &ol,bool)
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  ol.writeFooter(); // write the footer
  //if (Config_getString("HTML_FOOTER").isEmpty())
  //{
  //  ol.parseText(theTranslator->trGeneratedAt(
  //            dateToString(TRUE),
  //            Config_getString("PROJECT_NAME")
  //           ));
  //}
  //ol.writeFooter(1,external); // write the link to the picture
  //if (Config_getString("HTML_FOOTER").isEmpty())
  //{
  //  parseText(ol,theTranslator->trWrittenBy());
  //}
  //ol.writeFooter(2,external); // end the footer
  ol.popGeneratorState();
  ol.endFile();
}

//----------------------------------------------------------------------------

static bool classHasVisibleChildren(ClassDef *cd)
{
  if (cd->subClasses()->count()==0) return FALSE;
  BaseClassList *bcl=cd->subClasses();
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

void writeClassTree(OutputList &ol,BaseClassList *bcl,bool hideSuper,int level)
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
        startIndexHierarchy(ol,level);
        if (hasHtmlHelp) htmlHelp->incContentsDepth();
        if (hasFtvHelp)  ftvHelp->incContentsDepth();
        started=TRUE;
      }
      //printf("Passed...\n");
      bool hasChildren = !cd->visited && !hideSuper && classHasVisibleChildren(cd);
      //printf("tree4: Has children %s: %d\n",cd->name().data(),hasChildren);
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
          htmlHelp->addContentsItem(hasChildren,cd->displayName(),cd->getOutputFileBase());
        }
        if (hasFtvHelp)
        {
          ftvHelp->addContentsItem(hasChildren,cd->getReference(),cd->getOutputFileBase(),0,cd->displayName());
        }
      }
      else
      {
        ol.writeIndexItem(0,0,cd->name());
        if (hasHtmlHelp)
        {
          htmlHelp->addContentsItem(hasChildren,cd->displayName(),0);
        }
        if (hasFtvHelp)
        {
          ftvHelp->addContentsItem(hasChildren,0,0,0,cd->displayName());
        }
      }
      if (hasChildren)
      {
        //printf("Class %s at %p visited=%d\n",cd->name().data(),cd,cd->visited);
        bool wasVisited=cd->visited;
        cd->visited=TRUE;
        writeClassTree(ol,cd->subClasses(),wasVisited,level+1);
      }
    }
  }
  if (started) 
  {
    endIndexHierarchy(ol,level);
    if (hasHtmlHelp) htmlHelp->decContentsDepth();
    if (hasFtvHelp)  ftvHelp->decContentsDepth();
  }
}


//----------------------------------------------------------------------------
/*! Generates HTML Help tree of classes */

void writeClassTree(BaseClassList *cl,int level)
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
    if (cd->isVisibleInHierarchy() && hasVisibleRoot(cd->baseClasses()))
    //if (cd->isVisibleInHierarchy() && !cd->visited)
    {
      if (!started)
      {
        if (hasHtmlHelp) htmlHelp->incContentsDepth();
        if (hasFtvHelp)  ftvHelp->incContentsDepth();
        started=TRUE;
      }
      bool hasChildren = !cd->visited && classHasVisibleChildren(cd);
      //printf("tree2: Has children %s: %d\n",cd->name().data(),hasChildren);
      if (cd->isLinkable())
      {
        if (hasHtmlHelp)
        {
            htmlHelp->addContentsItem(hasChildren,cd->displayName(),cd->getOutputFileBase());
        }
        if (hasFtvHelp)
        {
            ftvHelp->addContentsItem(hasChildren,cd->getReference(),cd->getOutputFileBase(),0,cd->displayName());
        }
      }
      if (hasChildren)
      {
        writeClassTree(cd->subClasses(),level+1);
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

void writeClassTreeNode(ClassDef *cd,bool hasHtmlHelp,bool hasFtvHelp,bool &started,int level)
{
    if (cd->isVisibleInHierarchy() && !cd->visited)
    {
      if (!started)
      {
        started=TRUE;
      }
      bool hasChildren = classHasVisibleChildren(cd);
      //printf("node: Has children %s: %d\n",cd->name().data(),hasChildren);
      if (cd->isLinkable())
      {
        if (hasHtmlHelp)
        {
          HtmlHelp::getInstance()->addContentsItem(hasChildren,cd->displayName(),cd->getOutputFileBase());
        }
        if (hasFtvHelp)
        {
          FTVHelp::getInstance()->addContentsItem(hasChildren,cd->getReference(),cd->getOutputFileBase(),0,cd->displayName());
        }
      }
      if (hasChildren)
      {
        writeClassTree(cd->subClasses(),level+1);
      }
      cd->visited=TRUE;
    }
}

void writeClassTree(ClassList *cl,int level)
{
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
  ClassListIterator cli(*cl);
  bool started=FALSE;
  for ( ; cli.current() ; ++cli)
  {
    writeClassTreeNode(cli.current(),hasHtmlHelp,hasFtvHelp,started,level);
  }
}

void writeClassTree(ClassSDict *d,int level)
{
  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
  ClassSDict::Iterator cli(*d);
  bool started=FALSE;
  for ( ; cli.current() ; ++cli)
  {
    writeClassTreeNode(cli.current(),hasHtmlHelp,hasFtvHelp,started,level);
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
          startIndexHierarchy(ol,0);
          if (hasHtmlHelp) htmlHelp->incContentsDepth();
          if (hasFtvHelp)  ftvHelp->incContentsDepth();
          started=TRUE;
        }
        bool hasChildren = !cd->visited && classHasVisibleChildren(cd); 
        //printf("list: Has children %s: %d\n",cd->name().data(),hasChildren);
        if (cd->isLinkable())
        {
          //printf("Writing class %s isLinkable()=%d isLinkableInProject()=%d cd->templateMaster()=%p\n",
          //    cd->displayName().data(),cd->isLinkable(),cd->isLinkableInProject(),cd->templateMaster());
          ol.writeIndexItem(cd->getReference(),cd->getOutputFileBase(),cd->displayName());
          if (cd->isReference()) 
          {
            ol.startTypewriter(); 
            ol.docify(" [external]");
            ol.endTypewriter();
          }
          if (hasHtmlHelp)
          {
            htmlHelp->addContentsItem(hasChildren,cd->displayName(),cd->getOutputFileBase());
          }
          if (hasFtvHelp)
          {
            ftvHelp->addContentsItem(hasChildren,cd->getReference(),cd->getOutputFileBase(),0,cd->displayName());
          }
        }
        else
        {
          ol.writeIndexItem(0,0,cd->displayName());
          if (hasHtmlHelp)
          {
            htmlHelp->addContentsItem(hasChildren,cd->displayName(),0);
          }
          if (hasFtvHelp)
          {
            ftvHelp->addContentsItem(hasChildren,0,0,0,cd->displayName());
          }
        }
        if (hasChildren) 
        {
          writeClassTree(ol,cd->subClasses(),cd->visited,1);
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
    endIndexHierarchy(ol,0);
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
  startFile(ol,"hierarchy",0,"Hierarchical Index",HLI_Hierarchy);
  startTitle(ol,0);
  QCString title = theTranslator->trClassHierarchy();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) 
  {
    title.prepend(Config_getString("PROJECT_NAME")+" ");
  }
  ol.parseText(title);
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
    ol.parseText(theTranslator->trGotoGraphicalHierarchy());
    ol.endTextLink();
    ol.newParagraph();
    ol.enable(OutputGenerator::Latex);
    ol.enable(OutputGenerator::RTF);
  }
  ol.parseText(theTranslator->trClassHierarchyDescription());
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
  ol.parseText(title);
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
  ol.parseText(theTranslator->trGotoTextualHierarchy());
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
  startFile(ol,"files",0,"File Index",HLI_Files);
  startTitle(ol,0);
  QCString title = theTranslator->trFileList();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  ol.parseText(title);
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
  ol.parseText(theTranslator->trFileListDescription(Config_getBool("EXTRACT_ALL")));
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
        if (!path.isEmpty()) 
        {
          if (path.at(path.length()-1)!='/') fullName.prepend("/");
          fullName.prepend(path);
        }

        ol.startIndexKey();
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
            htmlHelp->addContentsItem(FALSE,fullName,0);
          }
          if (hasFtvHelp)
          {
            ftvHelp->addContentsItem(FALSE,0,0,0,fullName);
          }
        }
        if (src)
        {
          ol.pushGeneratorState();
          ol.disableAllBut(OutputGenerator::Html);
          ol.docify(" ");
          ol.startTextLink(fd->includeName(),0);
          ol.docify("[");
          ol.parseText(theTranslator->trCode());
          ol.docify("]");
          ol.endTextLink();
          ol.popGeneratorState();
        }
        ol.endIndexKey();
        bool hasBrief = !fd->briefDescription().isEmpty();
        ol.startIndexValue(hasBrief);
        if (hasBrief)
        {
          //ol.docify(" (");
          ol.parseDoc(
              fd->briefFile(),fd->briefLine(),
              fd,0,
              abbreviate(fd->briefDescription(),fd->name()),
              FALSE, // index words
              FALSE, // isExample
              0,     // example name
              TRUE   // single line
             );
          //ol.docify(")");
        }
        ol.endIndexValue(fd->getOutputFileBase(),hasBrief);
        //ol.popGeneratorState();
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
  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
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
  QCString title;
  if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
  {
    startFile(ol,"namespaces",0,"Package Index",HLI_Namespaces);
    title = theTranslator->trPackageList();
  }
  else
  {
    startFile(ol,"namespaces",0,"Namespace Index",HLI_Namespaces);
    title = theTranslator->trNamespaceList();
  }
  startTitle(ol,0);
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  ol.parseText(title);
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
  if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
  {
    ol.parseText(theTranslator->trPackageListDescription());
  }
  else
  {
    ol.parseText(theTranslator->trNamespaceListDescription(Config_getBool("EXTRACT_ALL")));
  }
  //ol.newParagraph();
  ol.endTextBlock();

  bool first=TRUE;

  NamespaceSDict::Iterator nli(Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    if (nd->isLinkableInProject())
    {
      if (first)
      {
        ol.startIndexList();
        first=FALSE;
      }
      //ol.writeStartAnnoItem("namespace",nd->getOutputFileBase(),0,nd->name());
      ol.startIndexKey();
      ol.writeObjectLink(0,nd->getOutputFileBase(),0,nd->displayName());
      ol.endIndexKey();
      bool hasBrief = !nd->briefDescription().isEmpty();
      ol.startIndexValue(hasBrief);
      if (hasBrief)
      {
        //ol.docify(" (");
        ol.parseDoc(
                 nd->briefFile(),nd->briefLine(),
                 nd,0,
                 abbreviate(nd->briefDescription(),nd->displayName()),
                 FALSE, // index words
                 FALSE  // isExample
                );
        //ol.docify(")");
      }
      ol.endIndexValue(nd->getOutputFileBase(),hasBrief);
      //ol.writeEndAnnoItem(nd->getOutputFileBase());
      if (hasHtmlHelp)
      {
        htmlHelp->addContentsItem(FALSE,nd->displayName(),nd->getOutputFileBase());
      }
      if (hasFtvHelp)
      {
        ftvHelp->addContentsItem(FALSE,nd->getReference(),nd->getOutputFileBase(),0,nd->displayName());
      }
    }
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
      //ol.writeStartAnnoItem(type,cd->getOutputFileBase(),0,cd->displayName());
      ol.startIndexKey();
      ol.writeObjectLink(0,cd->getOutputFileBase(),0,cd->displayName());
      ol.endIndexKey();
      bool hasBrief = !cd->briefDescription().isEmpty();
      ol.startIndexValue(hasBrief);
      if (hasBrief)
      {
        ol.parseDoc(
                 cd->briefFile(),cd->briefLine(),
                 cd,0,
                 abbreviate(cd->briefDescription(),cd->name()),
                 FALSE,  // indexWords
                 FALSE   // isExample
                );
      }
      ol.endIndexValue(cd->getOutputFileBase(),hasBrief);
      //ol.writeEndAnnoItem(cd->getOutputFileBase());
      if (hasHtmlHelp)
      {
        HtmlHelp::getInstance()->addContentsItem(FALSE,cd->displayName(),cd->getOutputFileBase());
      }
      if (hasFtvHelp)
      {
        FTVHelp::getInstance()->addContentsItem(FALSE,cd->getReference(),cd->getOutputFileBase(),0,cd->displayName());
      }
    }
  }
  ol.endIndexList();
}

//----------------------------------------------------------------------------

// OBSOLETE
//void writePackageList(OutputList &ol)
//{
//  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
//  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
//  bool hasFtvHelp =  generateHtml && Config_getBool("GENERATE_TREEVIEW");
//  ol.startIndexList(); 
//  PackageSDict::Iterator pdi(Doxygen::packageDict);
//  PackageDef *pd;
//  for (;(pd=pdi.current());++pdi)
//  {
//    if (!pd->isReference())
//    {
//      //ol.writeStartAnnoItem("package",pd->getOutputFileBase(),0,pd->name());
//      ol.startIndexKey();
//      ol.writeObjectLink(0,pd->getOutputFileBase(),0,pd->name());
//      ol.endIndexKey();
//      bool hasBrief = !pd->briefDescription().isEmpty();
//      ol.startIndexValue(hasBrief);
//      if (hasBrief)
//      {
//        //ol.docify(" (");
//        parseDoc(ol,
//            pd->getDefFileName(),pd->getDefLine(),
//            pd,0,
//            abbreviate(pd->briefDescription(),pd->name()));
//        //ol.docify(")");
//      }
//      ol.endIndexValue(pd->getOutputFileBase(),hasBrief);
//      
//      //ol.writeEndAnnoItem(pd->getOutputFileBase());
//      if (hasHtmlHelp)
//      {
//        HtmlHelp::getInstance()->addContentsItem(FALSE,pd->name(),pd->getOutputFileBase());
//      }
//      if (hasFtvHelp)
//      {
//        FTVHelp::getInstance()->addContentsItem(FALSE,pd->getReference(),pd->getOutputFileBase(),0,pd->name());
//      }
//    }
//  }
//  ol.endIndexList();
//}

//----------------------------------------------------------------------------

// write an alphabetical index of all class with a header for each letter
void writeAlphabeticalClassList(OutputList &ol)
{
  //ol.startAlphabeticalIndexList(); 

  // first count the number of headers
  ClassSDict::Iterator cli(Doxygen::classSDict);
  ClassDef *cd;
  char startLetter=0;
  int headerItems=0;
  QCString alphaLinks = "<p><div class=\"qindex\">";
  for (;(cd=cli.current());++cli)
  {
    if (cd->isLinkableInProject() && cd->templateMaster()==0)
    {
      int index = getPrefixIndex(cd->className());
      //printf("name=%s index=%d\n",cd->className().data(),index);
      if (toupper(cd->className().at(index))!=startLetter) // new begin letter => new header
      {
        startLetter=toupper(cd->className().at(index));
        if (headerItems) alphaLinks += "&nbsp;|&nbsp;";
        headerItems++;
	alphaLinks += (QCString)"<a class=\"qindex\" href=\"#letter_" + 
                      startLetter + "\">" + 
                      startLetter + "</a>";
      }
    }
  }
  alphaLinks += "</div><p>\n";
  ol.writeString(alphaLinks);

  ol.writeString("<table align=center width=\"95%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");

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
      int index = getPrefixIndex(cd->className());
      if (toupper(cd->className().at(index))!=startLetter)
      {
        // insert a new header using a dummy class pointer.
        startLetter=toupper(cd->className().at(index));
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
    //ol.nextTableRow();
    ol.writeString("<tr>");
    // the last column may contain less items then the others
    int colsInRow = (i<rows-1) ? columns : itemsInLastRow; 
    //printf("row [%d]\n",i);
    for (j=0;j<colsInRow;j++) // foreach table column
    {
      ol.writeString("<td>");
      ClassDef *cd = colIterators[j]->current();
      //printf("columns [%d] cd=%p\n",j,cd);
      if (cd==(ClassDef *)8) // the class pointer is really a header
      {
        cd=++(*colIterators[j]); // get the next item
        if (cd)
        {
          //printf("head ClassDef=%p %s\n",cd,cd ? cd->name().data() : "<none>");
          int index = getPrefixIndex(cd->className());
          startLetter=toupper(cd->className().at(index));
          char s[2]; s[0]=startLetter; s[1]=0;
          //ol.writeIndexHeading(s);
          ol.writeString("<a name=\"letter_");
          ol.writeString(s);
          ol.writeString("\"></a>");
          ol.writeString("<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\">"
                           "<tr>"
                             "<td><div class=\"ah\">&nbsp;&nbsp;"); 
          ol.writeString(s);
          ol.writeString(         "&nbsp;&nbsp;</div>"
                             "</td>"
                           "</tr>"
                         "</table>\n");

        }
      }
      else if (cd) // a real class, insert a link
      {
        QCString namesp,cname;
        //if (cd->getNamespaceDef()) namesp=cd->getNamespaceDef()->displayName();
        //QCString cname=cd->className();
        extractNamespaceName(cd->name(),cname,namesp);
        QCString nsDispName;
        if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
        {
          nsDispName=substitute(namesp,"::",".");
        }
        else
        {
          nsDispName=namesp.copy();
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
                           nd->getOutputFileBase(),0,nsDispName);
          }
          else
          {
            ol.docify(nsDispName);
          }
          ol.docify(")");
        }
        ol.writeNonBreakableSpace(3);
        //printf("item ClassDef=%p %s\n",cd,cd ? cd->name().data() : "<none>");
        ++(*colIterators[j]);
      }
      //ol.endTableColumn();
      ol.writeString("</td>");
      //if (j<colsInRow-1) ol.nextTableColumn();
    }
    //ol.endTableRow();
    ol.writeString("</tr>");
  }
  //ol.endAlphabeticalIndexList();
  ol.writeString("</table>");
  
  ol.writeString(alphaLinks);

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
  startFile(ol,"classes"+Doxygen::htmlFileExtension,0,"Alphabetical index",HLI_Classes);
  startTitle(ol,0);
  ol.parseText(Config_getString("PROJECT_NAME")+" "+theTranslator->trCompoundIndex());
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
  startFile(ol,"annotated",0,"Annotated Index",HLI_Annotated);
  startTitle(ol,0);
  QCString title = theTranslator->trCompoundList();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle =  title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  ol.parseText(title);
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
  ol.parseText(theTranslator->trCompoundListDescription());
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

//void writePackageIndex(OutputList &ol)
//{
//  bool &generateHtml = Config_getBool("GENERATE_HTML") ;
//  bool hasHtmlHelp = generateHtml && Config_getBool("GENERATE_HTMLHELP");
//  bool hasFtvHelp  = generateHtml && Config_getBool("GENERATE_TREEVIEW");
//
//  if (documentedPackages==0) return;
//  
//  ol.pushGeneratorState();
//  ol.disable(OutputGenerator::Man);
//  startFile(ol,"packages",0,"Package Index");
//  startTitle(ol,0);
//  QCString title = theTranslator->trPackageList();
//  QCString htmlHelpTitle = title;
//  QCString ftvHelpTitle =  title;
//  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
//  parseText(ol,title);
//  endTitle(ol,0,0);
//  ol.startTextBlock();
//  HtmlHelp *htmlHelp = 0;
//  FTVHelp  *ftvHelp = 0;
//  if (hasHtmlHelp)
//  {
//    htmlHelp = HtmlHelp::getInstance();
//    htmlHelp->addContentsItem(TRUE,htmlHelpTitle,"packages"); 
//    htmlHelp->incContentsDepth();
//  }
//  if (hasFtvHelp)
//  {
//    ftvHelp = FTVHelp::getInstance();
//    ftvHelp->addContentsItem(TRUE,0,"packages",0,ftvHelpTitle); 
//    ftvHelp->incContentsDepth();
//  }
//  parseText(ol,theTranslator->trPackageListDescription());
//  ol.endTextBlock();
//  writePackageList(ol);
//  if (hasHtmlHelp)
//  {
//    htmlHelp->decContentsDepth();
//  }
//  if (hasFtvHelp)
//  {
//    ftvHelp->decContentsDepth();
//  }
//  
//  endFile(ol);
//  ol.popGeneratorState();
//}

//----------------------------------------------------------------------------

void writeMemberList(OutputList &ol,bool useSections,ClassMemberHighlight filter)
{
  bool first = TRUE;
  char lastChar = 0;

  MemberNameSDict::Iterator mnli(Doxygen::memberNameSDict);
  MemberName *mn=0;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    bool isFunc=FALSE;
    while (md && !found)
    {
      ClassDef *cd;
      bool isFriendToHide = Config_getBool("HIDE_FRIEND_COMPOUNDS") &&
                            (QCString(md->typeString())=="friend class" || 
                             QCString(md->typeString())=="friend struct" ||
                             QCString(md->typeString())=="friend union");
      if (
           md->isLinkableInProject() &&
           (cd=md->getClassDef()) &&
           cd->isLinkableInProject() && cd->templateMaster()==0 &&
           ( filter==CMHL_All        && !(md->isFriend() && isFriendToHide) ||
            (filter==CMHL_Functions  && (md->isFunction() || md->isSlot() || md->isSignal()))  ||
            (filter==CMHL_Variables  && md->isVariable())  ||
            (filter==CMHL_Typedefs   && md->isTypedef())   ||
            (filter==CMHL_Enums      && md->isEnumerate()) ||
            (filter==CMHL_EnumValues && md->isEnumValue()) ||
            (filter==CMHL_Properties && md->isProperty())  ||
            (filter==CMHL_Events     && md->isEvent())     ||
            (filter==CMHL_Related    && (md->isRelated() || (md->isFriend() && !isFriendToHide)))
           )
         ) 
      { 
        found=TRUE; 
        isFunc=!md->isObjCMethod() && 
                (md->isFunction() || md->isSlot() || md->isSignal()); 
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
          ol.startSection(anchor,title,SectionInfo::Subsection);
          ol.docify(title);
          ol.endSection(anchor,SectionInfo::Subsection);
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
            cd->templateMaster()==0
           )
        {
          if (count==0) 
            ol.docify(": ");
          else 
            ol.docify(", ");
          ol.writeObjectLink(md->getReference(),md->getOutputFileBase(),md->anchor(),
                            cd->displayName());
          count++;
          prevName=cd->displayName();
        }
        md=mn->prev();
      }
    }
  }
  ol.endItemList();
}

//----------------------------------------------------------------------------

int countClassMembers(int filter)
{
  int i=0;for (i=0;i<256;i++) g_memberIndexLetterUsed[filter][i]=FALSE;
  int count=0;
  MemberNameSDict::Iterator mnli(Doxygen::memberNameSDict);
  MemberName *mn=0;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    ClassDef *cd;
    while (md && !found)
    {
      bool isFriendToHide = Config_getBool("HIDE_FRIEND_COMPOUNDS") &&
                            (QCString(md->typeString())=="friend class" || 
                             QCString(md->typeString())=="friend struct" ||
                             QCString(md->typeString())=="friend union");
      if (
          md->isLinkableInProject() && 
          (cd=md->getClassDef()) && 
          cd->isLinkableInProject() &&
          ( filter==CMHL_All        && !(md->isFriend() && isFriendToHide) ||
           (filter==CMHL_Functions  && (md->isFunction() || md->isSlot() || md->isSignal()))  ||
           (filter==CMHL_Variables  && md->isVariable())  ||
           (filter==CMHL_Typedefs   && md->isTypedef())   ||
           (filter==CMHL_Enums      && md->isEnumerate()) ||
           (filter==CMHL_EnumValues && md->isEnumValue()) ||
           (filter==CMHL_Properties && md->isProperty())  ||
           (filter==CMHL_Events     && md->isEvent())     ||
           (filter==CMHL_Related    && (md->isRelated() || (md->isFriend() && !isFriendToHide)))
          )
         )
      {
        found=TRUE;
      }
      md=mn->next();
    }
    if (found)
    {
      QCString n = mn->memberName();
      if (!n.isEmpty()) g_memberIndexLetterUsed[filter][tolower(n.at(0))]=TRUE;
      count++;
    }
  }
  return count;
}

//----------------------------------------------------------------------------

void writeQuickMemberIndex(OutputList &ol,bool *charUsed)
{
  bool first=TRUE;
  int i;
  ol.writeString("<div class=\"qindex\">"); 
  for (i=33;i<127;i++)
  {
    QCString anchor="#index_";
    char is[2];is[0]=(char)i;is[1]='\0';
    if (charUsed[i])
    {
      startQuickIndexItem(ol,anchor+is,FALSE,TRUE,first);
      ol.writeString(is);
      endQuickIndexItem(ol);
      first=FALSE;
    }
  }
  ol.writeString("</div>\n");
  ol.newParagraph();
}

//----------------------------------------------------------------------------

static void writeMemberIndexFiltered(OutputList &ol,
                       const char *fileName,ClassMemberHighlight hl)
{
  if (documentedClassMembers[hl]==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,fileName,0,"Compound Member Index",HLI_Functions);
  QCString title = theTranslator->trCompoundMembers();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle =  title;
  //if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  //startTitle(ol,0);
  //ol.parseText(title);
  //endTitle(ol,0,0);
  
  ol.writeString("<div class=\"qindex\">"); 

  bool first=TRUE;
  startQuickIndexItem(ol,
       "functions"+Doxygen::htmlFileExtension,hl==CMHL_All,TRUE,first);
  ol.writeString(fixSpaces(theTranslator->trAll()));
  endQuickIndexItem(ol);
  
  if (documentedClassMembers[CMHL_Functions]>0)
  {
    startQuickIndexItem(ol,
        "functions_func"+Doxygen::htmlFileExtension,hl==CMHL_Functions,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trFunctions()));
    endQuickIndexItem(ol);
  }
  if (documentedClassMembers[CMHL_Variables]>0)
  {
    startQuickIndexItem(ol,
        "functions_vars"+Doxygen::htmlFileExtension,hl==CMHL_Variables,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trVariables()));
    endQuickIndexItem(ol);
  }
  if (documentedClassMembers[CMHL_Typedefs]>0)
  {
    startQuickIndexItem(ol,
        "functions_type"+Doxygen::htmlFileExtension,hl==CMHL_Typedefs,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trTypedefs()));
    endQuickIndexItem(ol);
  }
  if (documentedClassMembers[CMHL_Enums]>0)
  {
    startQuickIndexItem(ol,
        "functions_enum"+Doxygen::htmlFileExtension,hl==CMHL_Enums,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trEnumerations()));
    endQuickIndexItem(ol);
  }
  if (documentedClassMembers[CMHL_EnumValues]>0)
  {
    startQuickIndexItem(ol,
        "functions_eval"+Doxygen::htmlFileExtension,hl==CMHL_EnumValues,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trEnumerationValues()));
    endQuickIndexItem(ol);
  }
  if (documentedClassMembers[CMHL_Properties]>0)
  {
    startQuickIndexItem(ol,
        "functions_prop"+Doxygen::htmlFileExtension,hl==CMHL_Properties,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trProperties()));
    endQuickIndexItem(ol);
  }
  if (documentedClassMembers[CMHL_Events]>0)
  {
    startQuickIndexItem(ol,
        "functions_evnt"+Doxygen::htmlFileExtension,hl==CMHL_Events,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trEvents()));
    endQuickIndexItem(ol);
  }
  if (documentedClassMembers[CMHL_Related]>0)
  {
    startQuickIndexItem(ol,
        "functions_rela"+Doxygen::htmlFileExtension,hl==CMHL_Related,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trRelatedFunctions()));
    endQuickIndexItem(ol);
  }
  ol.writeString("</div>\n");
  
  bool quickIndex = documentedClassMembers[hl]>maxItemsBeforeQuickIndex;
  if (quickIndex)
  {
    writeQuickMemberIndex(ol,g_memberIndexLetterUsed[hl]);
  }
  ol.newParagraph();
  if (hl==CMHL_All) 
  {
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
    ol.parseText(theTranslator->trCompoundMembersDescription(Config_getBool("EXTRACT_ALL")));
  }
  writeMemberList(ol,quickIndex,hl);
  endFile(ol);
  ol.popGeneratorState();
}

void writeMemberIndex(OutputList &ol)
{
  QCString ext=Doxygen::htmlFileExtension;
  writeMemberIndexFiltered(ol,"functions"+ext,CMHL_All);
  writeMemberIndexFiltered(ol,"functions_func"+ext,CMHL_Functions);
  writeMemberIndexFiltered(ol,"functions_vars"+ext,CMHL_Variables);
  writeMemberIndexFiltered(ol,"functions_type"+ext,CMHL_Typedefs);
  writeMemberIndexFiltered(ol,"functions_enum"+ext,CMHL_Enums);
  writeMemberIndexFiltered(ol,"functions_eval"+ext,CMHL_EnumValues);
  writeMemberIndexFiltered(ol,"functions_prop"+ext,CMHL_Properties);
  writeMemberIndexFiltered(ol,"functions_evnt"+ext,CMHL_Events);
  writeMemberIndexFiltered(ol,"functions_rela"+ext,CMHL_Related);
}

//----------------------------------------------------------------------------

static void writeFileMemberList(OutputList &ol,
                                bool useSections,
                                FileMemberHighlight filter)
{
  char lastChar=0;
  bool first=TRUE;
  MemberNameSDict::Iterator mnli(Doxygen::functionNameSDict);
  MemberName *mn=0;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    while (md && !found)
    {
      FileDef *fd=md->getFileDef();
      bool hasDocs = md->getFileDef() && 
                     md->getFileDef()->isLinkableInProject();
      
      if (fd && hasDocs && 
          md->isLinkableInProject() &&
          ( filter==FMHL_All ||
           (filter==FMHL_Functions  && md->isFunction())  ||
           (filter==FMHL_Variables  && md->isVariable())  ||
           (filter==FMHL_Typedefs   && md->isTypedef())   ||
           (filter==FMHL_Enums      && md->isEnumerate()) ||
           (filter==FMHL_EnumValues && md->isEnumValue()) ||
           (filter==FMHL_Defines    && md->isDefine())
          )
         ) 
      {
        found=TRUE;
      }
      else
      {
        md=mn->next();
      }
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
          ol.startSection(anchor,title,SectionInfo::Subsection);
          ol.docify(title);
          ol.endSection(anchor,SectionInfo::Subsection);
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
        if (fd && fd->isLinkableInProject() && 
            md->isLinkableInProject() &&
            prevName!=fd->name())
        {
          if (count==0) 
            ol.docify(": ");
          else 
            ol.docify(", ");
          QCString baseName=fd->name();
          ol.writeObjectLink(md->getReference(),
              md->getOutputFileBase(),md->anchor(), baseName);
          count++;
          prevName=fd->name();
        }
        md=mn->prev();
      }
    }
  }
  ol.endItemList();
}

//----------------------------------------------------------------------------

void writeNamespaceMemberList(OutputList &ol,bool useSections,
                              NamespaceMemberHighlight filter)
{
  char lastChar=0;
  bool first=TRUE;
  MemberNameSDict::Iterator mnli(Doxygen::functionNameSDict);
  MemberName *mn=0;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    while (md && !found)
    {
      NamespaceDef *nd=md->getNamespaceDef();
      if (nd && nd->isLinkableInProject() && md->isLinkableInProject() &&
          ( filter==NMHL_All ||
           (filter==NMHL_Functions  && md->isFunction())  ||
           (filter==NMHL_Variables  && md->isVariable())  ||
           (filter==NMHL_Typedefs   && md->isTypedef())   ||
           (filter==NMHL_Enums      && md->isEnumerate()) ||
           (filter==NMHL_EnumValues && md->isEnumValue())
          )
         ) 
      {
        found=TRUE;
      }
      else
      {
        md=mn->next();
      }
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
          ol.startSection(anchor,title,SectionInfo::Subsection);
          ol.docify(title);
          ol.endSection(anchor,SectionInfo::Subsection);
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
          ol.writeObjectLink(md->getReference(),md->getOutputFileBase(),
                             md->anchor(),nd->name());
          count++;
          prevName=nd->name();
        }
        md=mn->prev();
      }
    }
  }
  if (!first) ol.endItemList();
}

//----------------------------------------------------------------------------

int countNamespaceMembers(int filter)
{
  int i=0;for (i=0;i<256;i++) g_namespaceIndexLetterUsed[filter][i]=FALSE;
  int count=0;
  MemberNameSDict::Iterator mnli(Doxygen::functionNameSDict);
  MemberName *mn=0;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberDef *md=mn->first();
    bool found=FALSE;
    while (md && !found)
    {
      NamespaceDef *nd=md->getNamespaceDef();
      if (nd && nd->isLinkableInProject() && md->isLinkableInProject() &&
          ( filter==NMHL_All ||
           (filter==NMHL_Functions  && md->isFunction())  ||
           (filter==NMHL_Variables  && md->isVariable())  ||
           (filter==NMHL_Typedefs   && md->isTypedef())   ||
           (filter==NMHL_Enums      && md->isEnumerate()) ||
           (filter==NMHL_EnumValues && md->isEnumValue())
          )
         )
      {
        QCString n = mn->memberName();
        if (!n.isEmpty()) g_namespaceIndexLetterUsed[filter][tolower(n.at(0))]=TRUE;
        found=TRUE;
      }
      else
        md=mn->next();
    }
    if (found) count++;
  }
  return count;
}

//----------------------------------------------------------------------------

int countFileMembers(int filter)
{
  int i=0;for (i=0;i<256;i++) g_fileIndexLetterUsed[filter][i]=FALSE;
  int count=0;
  MemberNameSDict::Iterator mnli(Doxygen::functionNameSDict);
  MemberName *mn=0;
  for (mnli.toFirst();(mn=mnli.current());++mnli)
  {
    MemberDef *md=mn->first();
    FileDef *fd;
    bool found=FALSE;
    while (md && !found)
    {
      if (md->isLinkableInProject() && 
          (fd=md->getFileDef()) && 
          fd->isLinkableInProject() &&
          ( filter==FMHL_All ||
           (filter==FMHL_Functions  && md->isFunction())  ||
           (filter==FMHL_Variables  && md->isVariable())  ||
           (filter==FMHL_Typedefs   && md->isTypedef())   ||
           (filter==FMHL_Enums      && md->isEnumerate()) ||
           (filter==FMHL_EnumValues && md->isEnumValue()) ||
           (filter==FMHL_Defines    && md->isDefine())
          )
         ) 
      {
        QCString n = mn->memberName();
        if (!n.isEmpty()) 
        {
          g_fileIndexLetterUsed[filter][tolower(n.at(0))]=TRUE;
        }
        found=TRUE;
      }
      else
        md=mn->next();
    }
    if (found) count++;
  }
  return count;
}

//----------------------------------------------------------------------------

static void writeFileMemberIndexFiltered(OutputList &ol,
    const char *fileName,FileMemberHighlight hl)
{
  if (documentedFileMembers[hl]==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,fileName,0,"File Member Index",HLI_Globals);
  QCString title = theTranslator->trFileMembers();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle =  title;

  ol.writeString("<div class=\"qindex\">"); 

  bool first=TRUE;
  startQuickIndexItem(ol,
       "globals"+Doxygen::htmlFileExtension,hl==FMHL_All,TRUE,first);
  ol.writeString(fixSpaces(theTranslator->trAll()));
  endQuickIndexItem(ol);
  
  if (documentedFileMembers[FMHL_Functions]>0)
  {
    startQuickIndexItem(ol,
        "globals_func"+Doxygen::htmlFileExtension,hl==FMHL_Functions,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trFunctions()));
    endQuickIndexItem(ol);
  }
  if (documentedFileMembers[FMHL_Variables]>0)
  {
    startQuickIndexItem(ol,
        "globals_vars"+Doxygen::htmlFileExtension,hl==FMHL_Variables,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trVariables()));
    endQuickIndexItem(ol);
  }
  if (documentedFileMembers[FMHL_Typedefs]>0)
  {
    startQuickIndexItem(ol,
        "globals_type"+Doxygen::htmlFileExtension,hl==FMHL_Typedefs,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trTypedefs()));
    endQuickIndexItem(ol);
  }
  if (documentedFileMembers[FMHL_Enums]>0)
  {
    startQuickIndexItem(ol,
        "globals_enum"+Doxygen::htmlFileExtension,hl==FMHL_Enums,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trEnumerations()));
    endQuickIndexItem(ol);
  }
  if (documentedFileMembers[FMHL_EnumValues]>0)
  {
    startQuickIndexItem(ol,
        "globals_eval"+Doxygen::htmlFileExtension,hl==FMHL_EnumValues,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trEnumerationValues()));
    endQuickIndexItem(ol);
  }
  if (documentedFileMembers[FMHL_Defines]>0)
  {
    startQuickIndexItem(ol,
        "globals_defs"+Doxygen::htmlFileExtension,hl==FMHL_Defines,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trDefines()));
    endQuickIndexItem(ol);
  }
  ol.writeString("</div>\n");
  

  bool quickIndex = documentedFileMembers[hl]>maxItemsBeforeQuickIndex;
  if (quickIndex)
  {
    writeQuickMemberIndex(ol,g_fileIndexLetterUsed[hl]);
  }
  ol.newParagraph();
  if (hl==FMHL_All) 
  {
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
    ol.parseText(theTranslator->trFileMembersDescription(Config_getBool("EXTRACT_ALL")));
  }
  writeFileMemberList(ol,quickIndex,hl);
  endFile(ol);
  ol.popGeneratorState();
}

void writeFileMemberIndex(OutputList &ol)
{
  QCString ext=Doxygen::htmlFileExtension;
  writeFileMemberIndexFiltered(ol,"globals"+ext,FMHL_All);
  writeFileMemberIndexFiltered(ol,"globals_func"+ext,FMHL_Functions);
  writeFileMemberIndexFiltered(ol,"globals_vars"+ext,FMHL_Variables);
  writeFileMemberIndexFiltered(ol,"globals_type"+ext,FMHL_Typedefs);
  writeFileMemberIndexFiltered(ol,"globals_enum"+ext,FMHL_Enums);
  writeFileMemberIndexFiltered(ol,"globals_eval"+ext,FMHL_EnumValues);
  writeFileMemberIndexFiltered(ol,"globals_defs"+ext,FMHL_Defines);
}


//----------------------------------------------------------------------------

static void writeNamespaceMemberIndexFiltered(OutputList &ol,
                                        const char *fileName,
                                        NamespaceMemberHighlight hl)
{
  if (documentedNamespaceMembers[hl]==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,fileName,0,"Namespace Member Index",HLI_NamespaceMembers);
  QCString title = theTranslator->trNamespaceMembers();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle =  title;
  //if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  //startTitle(ol,0);
  //ol.parseText(title);
  //endTitle(ol,0,0);
 
  ol.writeString("<div class=\"qindex\">"); 

  bool first=TRUE;
  startQuickIndexItem(ol,
       "namespacemembers"+Doxygen::htmlFileExtension,hl==NMHL_All,TRUE,first);
  ol.writeString(fixSpaces(theTranslator->trAll()));
  endQuickIndexItem(ol);
  
  if (documentedNamespaceMembers[NMHL_Functions]>0)
  {
    startQuickIndexItem(ol,
        "namespacemembers_func"+Doxygen::htmlFileExtension,hl==NMHL_Functions,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trFunctions()));
    endQuickIndexItem(ol);
  }
  if (documentedNamespaceMembers[NMHL_Variables]>0)
  {
    startQuickIndexItem(ol,
        "namespacemembers_vars"+Doxygen::htmlFileExtension,hl==NMHL_Variables,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trVariables()));
    endQuickIndexItem(ol);
  }
  if (documentedNamespaceMembers[NMHL_Typedefs]>0)
  {
    startQuickIndexItem(ol,
        "namespacemembers_type"+Doxygen::htmlFileExtension,hl==NMHL_Typedefs,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trTypedefs()));
    endQuickIndexItem(ol);
  }
  if (documentedNamespaceMembers[NMHL_Enums]>0)
  {
    startQuickIndexItem(ol,
        "namespacemembers_enum"+Doxygen::htmlFileExtension,hl==NMHL_Enums,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trEnumerations()));
    endQuickIndexItem(ol);
  }
  if (documentedNamespaceMembers[NMHL_EnumValues]>0)
  {
    startQuickIndexItem(ol,
        "namespacemembers_eval"+Doxygen::htmlFileExtension,hl==NMHL_EnumValues,TRUE,first);
    ol.writeString(fixSpaces(theTranslator->trEnumerationValues()));
    endQuickIndexItem(ol);
  }
  ol.writeString("</div>\n");
  
  bool quickIndex = documentedNamespaceMembers[hl]>maxItemsBeforeQuickIndex;
  if (quickIndex)
  {
    writeQuickMemberIndex(ol,g_namespaceIndexLetterUsed[hl]);
  }
  ol.newParagraph();
  if (hl==NMHL_All) 
  {
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
    ol.parseText(theTranslator->trNamespaceMemberDescription(Config_getBool("EXTRACT_ALL")));
  }
  writeNamespaceMemberList(ol,quickIndex,hl);
  endFile(ol);
  ol.popGeneratorState();
}

void writeNamespaceMemberIndex(OutputList &ol)
{
  QCString ext=Doxygen::htmlFileExtension;
  writeNamespaceMemberIndexFiltered(ol,"namespacemembers"+ext,NMHL_All);
  writeNamespaceMemberIndexFiltered(ol,"namespacemembers_func"+ext,NMHL_Functions);
  writeNamespaceMemberIndexFiltered(ol,"namespacemembers_vars"+ext,NMHL_Variables);
  writeNamespaceMemberIndexFiltered(ol,"namespacemembers_type"+ext,NMHL_Typedefs);
  writeNamespaceMemberIndexFiltered(ol,"namespacemembers_enum"+ext,NMHL_Enums);
  writeNamespaceMemberIndexFiltered(ol,"namespacemembers_eval"+ext,NMHL_EnumValues);
}

//----------------------------------------------------------------------------

void writeExampleIndex(OutputList &ol)
{
  if (Doxygen::exampleSDict->count()==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"examples",0,"Example Index",HLI_Examples);
  startTitle(ol,0);
  QCString title = theTranslator->trExamples();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  ol.parseText(title);
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
  ol.parseText(theTranslator->trExamplesDescription());
  //ol.newParagraph();
  ol.endTextBlock();
  ol.startItemList();
  PageSDict::Iterator pdi(*Doxygen::exampleSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    ol.writeListItem();
    QCString n=pd->getOutputFileBase();
    if (!pd->title().isEmpty())
    {
      ol.writeObjectLink(0,n,0,pd->title());
      if (hasHtmlHelp) htmlHelp->addContentsItem(FALSE,pd->title(),n);
      if (hasFtvHelp)  ftvHelp->addContentsItem(FALSE,pd->getReference(),n,0,pd->title());
    }
    else
    {
      ol.writeObjectLink(0,n,0,pd->name());
      if (hasHtmlHelp) htmlHelp->addContentsItem(FALSE,pd->name(),n);
      if (hasFtvHelp)  ftvHelp->addContentsItem(FALSE,pd->getReference(),n,0,pd->name());
    }
    ol.writeString("\n");
  }
  ol.endItemList();
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
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    if (!pd->getGroupDef() && (!pd->isReference() || Config_getBool("ALLEXTERNALS")))
    {
      indexPages++;
      if (!pd->isReference()) docPages++;
    }
  }
}

//----------------------------------------------------------------------------


void writePageIndex(OutputList &ol)
{
  if (indexedPages==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"pages",0,"Page Index",HLI_Pages);
  startTitle(ol,0);
  QCString title = theTranslator->trRelatedPages();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  ol.parseText(title);
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
  ol.parseText(theTranslator->trRelatedPagesDescription());
  ol.endTextBlock();
  startIndexHierarchy(ol,0);
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd=0;
  for (pdi.toFirst();(pd=pdi.current());++pdi)
  {
    if (!pd->getGroupDef() && (!pd->isReference() || Config_getBool("ALLEXTERNALS")))
    {
      QCString pageTitle;

      if (pd->title().isEmpty())
        pageTitle=pd->name();
      else
        pageTitle=pd->title();

      ol.writeIndexItem(pd->getReference(),pd->getOutputFileBase(),pageTitle);
      if (pd->isReference()) 
      { 
        ol.startTypewriter(); 
        ol.docify(" [external]");
        ol.endTypewriter();
      }
      ol.writeString("\n");
      if (hasHtmlHelp) htmlHelp->addContentsItem(FALSE,pageTitle,pd->getOutputFileBase());
      if (hasFtvHelp)  ftvHelp->addContentsItem(FALSE,pd->getReference(),pd->getOutputFileBase(),0,pageTitle);
    }
  }
  endIndexHierarchy(ol,0);
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
  GroupSDict::Iterator gli(Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
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
  ol.parseText(theTranslator->trLegendTitle());
  endTitle(ol,0,0);
  bool oldStripCommentsState = Config_getBool("STRIP_CODE_COMMENTS");
  // temporarily disable the stripping of comments for our own code example!
  Config_getBool("STRIP_CODE_COMMENTS") = FALSE;
  ol.parseDoc("graph_legend",1,0,0,theTranslator->trLegendDocs(),FALSE,FALSE);
  Config_getBool("STRIP_CODE_COMMENTS") = oldStripCommentsState;
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------
/*!
 * write groups as hierarchial trees
 * \author KPW
 */

void writeGroupTreeNode(OutputList &ol, GroupDef *gd,int level)
{
  HtmlHelp *htmlHelp=0;
  FTVHelp  *ftvHelp = 0;
  if (level>20)
  {
    warn(gd->getDefFileName(),gd->getDefLine(),
        "Warning: maximum nesting level exceeded for group %s: check for possible recursive group relation!\n",gd->name().data()
        );
    return;
  }
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

  /* Some groups should appear twice under different parent-groups.
   * That is why we should not check if it was visited 
   */
  if (/*!gd->visited &&*/ (!gd->isASubGroup() || level>0) &&
      (!gd->isReference() || Config_getBool("EXTERNAL_GROUPS")) // hide external groups by default
     )
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
    if (ftvHelp)
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
    PageSDict::Iterator pli(*gd->pageDict);
    PageDef *pd = 0;
    for (pli.toFirst();(pd=pli.current());++pli)
    {
      SectionInfo *si=0;
      if (!pd->name().isEmpty()) si=Doxygen::sectionDict[pd->name()];
      if(htmlHelp) htmlHelp->addContentsItem(FALSE,
                                   convertToHtml(pd->title()),
                                   gd->getOutputFileBase(),
                                   si ? si->label.data() : 0
                                  ); 
      if(ftvHelp)  ftvHelp->addContentsItem(FALSE,
                                   gd->getReference(),
                                   gd->getOutputFileBase(),
                                   si ? si->label.data() : 0,
                                   convertToHtml(pd->title())
                                  ); 
    }

    // write subgroups
    if (hasSubGroups)
    {
      startIndexHierarchy(ol,level+1);
      QListIterator<GroupDef> gli(*gd->groupList);
      GroupDef *subgd = 0;
      for (gli.toFirst();(subgd=gli.current());++gli)
      {
        writeGroupTreeNode(ol,subgd,level+1);
      }
      endIndexHierarchy(ol,level+1); 
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
          bool first=TRUE;
          MemberDef *md=members->first();
          while (md)
          {
            if (md->isDetailedSectionVisible(TRUE))
            {
              if (first)
              {
                first=FALSE;
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
              }
              if(htmlHelp)
              {
                htmlHelp->addContentsItem(FALSE,md->name(),md->getOutputFileBase(),md->anchor()); 
              }
              if(ftvHelp)
              {
                ftvHelp->addContentsItem(FALSE,md->getReference(),md->getOutputFileBase(),md->anchor(),md->name()); 
              }
            }
            md=members->next();
          }

          if(htmlHelp && !first) htmlHelp->decContentsDepth();
          if(ftvHelp && !first)  ftvHelp->decContentsDepth();

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
      if (gd->classSDict->count()>0)
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

        writeClassTree(gd->classSDict,1);
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

        PageSDict::Iterator eli(*(gd->exampleDict));
        PageDef *pd=eli.toFirst();
        while (pd)
        {
          if(htmlHelp)
          {
            htmlHelp->addContentsItem(FALSE,pd->getReference(),pd->getOutputFileBase()); 
          }
          if(ftvHelp)
          {
            ftvHelp->addContentsItem(FALSE,pd->getReference(),pd->getOutputFileBase(),0,pd->name()); 
          }
          pd=++eli;
        }

        if (htmlHelp) htmlHelp->decContentsDepth();
        if (ftvHelp)  ftvHelp->decContentsDepth();
      }
    }
    
    if (htmlHelp) htmlHelp->decContentsDepth();
    if (ftvHelp)  ftvHelp->decContentsDepth();
    
    //gd->visited=TRUE;
  }
}

void writeGroupHierarchy(OutputList &ol)
{
  startIndexHierarchy(ol,0);
  GroupSDict::Iterator gli(Doxygen::groupSDict);
  GroupDef *gd;
  for (gli.toFirst();(gd=gli.current());++gli)
  {
    writeGroupTreeNode(ol,gd,0);
  }
  endIndexHierarchy(ol,0); 
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
          gd->briefFile(),gd->briefLine(),
          gd,0,gd->briefDescription());
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
  startFile(ol,"modules",0,"Module Index",HLI_Modules);
  startTitle(ol,0);
  QCString title = theTranslator->trModules();
  QCString htmlHelpTitle = title;
  QCString ftvHelpTitle  = title;
  if (!Config_getString("PROJECT_NAME").isEmpty()) title.prepend(Config_getString("PROJECT_NAME")+" ");
  ol.parseText(title);
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
  ol.parseText(theTranslator->trModulesDescription());
  ol.endTextBlock();
  writeGroupHierarchy(ol);
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

static bool mainPageHasTitle()
{
  if (Doxygen::mainPage==0) return FALSE;
  if (Doxygen::mainPage->title().isEmpty()) return FALSE;
  if (Doxygen::mainPage->title().lower()=="notitle") return FALSE;
  return TRUE;
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
    Doxygen::mainPage ? Doxygen::mainPage->getDefFileName().data() : "<generated>";
  int defLine =
    Doxygen::mainPage ? Doxygen::mainPage->getDefLine() : -1;

  QCString title;
  if (!mainPageHasTitle())
  {
    title = theTranslator->trMainPage();
  }
  else 
  {
    title = substitute(Doxygen::mainPage->title(),"%","");
  }

  QCString indexName="index";
  if (Config_getBool("GENERATE_TREEVIEW")) indexName="main";
  ol.startFile(indexName,0,title);
  
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

  if (!Config_getBool("DISABLE_INDEX")) ol.writeQuickLinks(TRUE,HLI_Main);
  ol.startTitleHead(0);
  if (Doxygen::mainPage && !Doxygen::mainPage->title().isEmpty())
  {
    if (Doxygen::mainPage->title().lower()!="notitle")
    {
      ol.parseDoc(defFileName,defLine,Doxygen::mainPage,0,Doxygen::mainPage->title(),TRUE,FALSE);
    }
  }
  else
  {
    if (!Config_getString("PROJECT_NAME").isEmpty())
    {
      ol.parseText(projPrefix+theTranslator->trDocumentation());
    }
  }
  ol.endTitleHead(0,0);
  ol.newParagraph();
  if (!Config_getString("PROJECT_NUMBER").isEmpty())
  {
    ol.startProjectNumber();
    ol.parseDoc(defFileName,defLine,Doxygen::mainPage,0,Config_getString("PROJECT_NUMBER"),TRUE,FALSE);
    ol.endProjectNumber();
  }
  if (Config_getBool("DISABLE_INDEX") && Doxygen::mainPage==0) ol.writeQuickLinks(FALSE,HLI_Main);

  if (Doxygen::mainPage)
  {
    Doxygen::insideMainPage=TRUE;
    ol.parseDoc(defFileName,defLine,Doxygen::mainPage,0,
                Doxygen::mainPage->documentation(),TRUE,FALSE
                /*,Doxygen::mainPage->sectionDict*/);

    if (!Config_getString("GENERATE_TAGFILE").isEmpty())
    {
       Doxygen::tagFile << "  <compound kind=\"page\">" << endl
                        << "    <name>"
                        << convertToXML(Doxygen::mainPage->name())
                        << "</name>" << endl
                        << "    <title>"
                        << convertToXML(Doxygen::mainPage->title())
                        << "</title>" << endl
                        << "    <filename>"
                        << convertToXML(Doxygen::mainPage->getOutputFileBase())
                        << "</filename>" << endl;

       Doxygen::mainPage->writeDocAnchorsToTagFile();
       Doxygen::tagFile << "  </compound>" << endl;
    }
    Doxygen::insideMainPage=FALSE;
  }
  
  endFile(ol);
  ol.disable(OutputGenerator::Html);
  
  //--------------------------------------------------------------------
  // write LaTeX/RTF index
  //--------------------------------------------------------------------
  ol.enable(OutputGenerator::Latex);
  ol.enable(OutputGenerator::RTF);

  ol.startFile("refman",0,0);
  ol.startIndexSection(isTitlePageStart);
  if (!Config_getString("LATEX_HEADER").isEmpty()) 
  {
    ol.disable(OutputGenerator::Latex);
  }

  ol.parseText(projPrefix+theTranslator->trReferenceManual());
  if (!Config_getString("PROJECT_NUMBER").isEmpty())
  {
    ol.startProjectNumber(); 
    ol.parseDoc(defFileName,defLine,Doxygen::mainPage,0,Config_getString("PROJECT_NUMBER"),FALSE,FALSE);
    ol.endProjectNumber();
  }
  ol.endIndexSection(isTitlePageStart);
  ol.startIndexSection(isTitlePageAuthor);
  ol.parseText(theTranslator->trGeneratedBy());
  ol.endIndexSection(isTitlePageAuthor);
  ol.enable(OutputGenerator::Latex);

  if (Doxygen::mainPage)
  {
    ol.startIndexSection(isMainPage);
    if (mainPageHasTitle())
    {
      ol.parseDoc(defFileName,defLine,Doxygen::mainPage,0,Doxygen::mainPage->title(),FALSE,FALSE);
    }
    else
    {
      ol.parseText(projPrefix+theTranslator->trMainPage());
    }
    ol.endIndexSection(isMainPage);
  }

  if (!Config_getBool("LATEX_HIDE_INDICES"))
  {
    if (documentedGroups>0)
    {
      ol.startIndexSection(isModuleIndex);
      ol.parseText(projPrefix+theTranslator->trModuleIndex());
      ol.endIndexSection(isModuleIndex);
    }
    if (documentedNamespaces>0)
    {
      ol.startIndexSection(isNamespaceIndex);
      ol.parseText(projPrefix+theTranslator->trNamespaceIndex());
      ol.endIndexSection(isNamespaceIndex);
    }
    if (hierarchyClasses>0)
    {
      ol.startIndexSection(isClassHierarchyIndex);
      ol.parseText(projPrefix+theTranslator->trHierarchicalIndex());
      ol.endIndexSection(isClassHierarchyIndex);
    }
    if (annotatedClasses>0)
    {
      ol.startIndexSection(isCompoundIndex);
      ol.parseText(projPrefix+theTranslator->trCompoundIndex());
      ol.endIndexSection(isCompoundIndex);
    }
    if (documentedFiles>0)
    {
      ol.startIndexSection(isFileIndex);
      ol.parseText(projPrefix+theTranslator->trFileIndex());
      ol.endIndexSection(isFileIndex);
    }
    if (indexedPages>0)
    {
      ol.startIndexSection(isPageIndex);
      ol.parseText(projPrefix+theTranslator->trPageIndex());
      ol.endIndexSection(isPageIndex);
    }
  }
  ol.lastIndexPage();
  if (documentedGroups>0)
  {
    ol.startIndexSection(isModuleDocumentation);
    ol.parseText(projPrefix+theTranslator->trModuleDocumentation());
    ol.endIndexSection(isModuleDocumentation);
  }
  if (documentedNamespaces>0)
  {
    ol.startIndexSection(isNamespaceDocumentation);
    ol.parseText(projPrefix+theTranslator->trNamespaceDocumentation());
    ol.endIndexSection(isNamespaceDocumentation);
  }
  if (annotatedClasses>0)
  {
    ol.startIndexSection(isClassDocumentation);
    ol.parseText(projPrefix+theTranslator->trClassDocumentation());
    ol.endIndexSection(isClassDocumentation);
  }
  if (documentedFiles>0)
  {
    ol.startIndexSection(isFileDocumentation);
    ol.parseText(projPrefix+theTranslator->trFileDocumentation());
    ol.endIndexSection(isFileDocumentation);
  }
  if (Doxygen::exampleSDict->count()>0)
  {
    ol.startIndexSection(isExampleDocumentation);
    ol.parseText(projPrefix+theTranslator->trExampleDocumentation());
    ol.endIndexSection(isExampleDocumentation);
  }
  if (documentedPages>0)
  {
    ol.startIndexSection(isPageDocumentation);
    ol.parseText(projPrefix+theTranslator->trPageDocumentation());
    ol.endIndexSection(isPageDocumentation);
  }
  ol.endIndexSection(isEndIndex);
  endFile(ol);

  if (Doxygen::mainPage)
  {
    Doxygen::insideMainPage=TRUE;
    ol.disable(OutputGenerator::Man);
    startFile(ol,Doxygen::mainPage->name(),0,Doxygen::mainPage->title());
    ol.startTextBlock();
    ol.parseDoc(defFileName,defLine,Doxygen::mainPage,0,
                Doxygen::mainPage->documentation(),FALSE,FALSE
                /*,Doxygen::mainPage->sectionDict*/);
    ol.endTextBlock();
    endFile(ol);
    ol.enable(OutputGenerator::Man);
    Doxygen::insideMainPage=FALSE;
  }

  ol.popGeneratorState();
}



