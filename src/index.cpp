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
#include "scanner.h"
#include "code.h"
#include "config.h"
#include "filedef.h"
#include "outputlist.h"
#include "util.h"
#include "groupdef.h"
#include "language.h"
#include "htmlhelp.h"
#include "dot.h"

//----------------------------------------------------------------------------

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
        ol.writeIndexItem(cd->getReference(),cd->getOutputFileBase(),cd->displayName());
        if (cd->isReference()) 
        { 
          ol.startTypewriter(); 
          ol.docify(" [external]");
          ol.endTypewriter();
        }
        if (hasHtmlHelp)
        {
          htmlHelp->addContentsItem(cd->name(),cd->getOutputFileBase());
        }
      }
      else
      {
        ol.writeIndexItem(0,0,cd->name());
        //if (hasHtmlHelp) htmlHelp->addContentsItem(cd->name(),"nodoc");
        if (hasHtmlHelp)
        {
          htmlHelp->addContentsItem(cd->name(),0);
        }
      }
      if (!cd->visited && !hideSuper && cd->superClasses()->count()>0)
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
          started=TRUE;
        }
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
            htmlHelp->addContentsItem(cd->name(),cd->getOutputFileBase());
            //cd->writeMembersToContents();
          }
        }
        else
        {
          ol.writeIndexItem(0,0,cd->displayName());
          //if (hasHtmlHelp) htmlHelp->addContentsItem(cd->name(),"nodoc");
          if (hasHtmlHelp)
          {
            htmlHelp->addContentsItem(cd->name(),0);
          }
        }
        if (!cd->visited && cd->superClasses()->count()>0) 
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
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"hierarchy"); 
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
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"inherits"); 
  }
  ol.startTextLink("hierarchy",0);
  parseText(ol,theTranslator->trGotoTextualHierarchy());
  ol.endTextLink();
  ol.newParagraph();
  parseText(ol,theTranslator->trClassHierarchyDescription());
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
      bool src = fd->generateSource() || Config::sourceBrowseFlag;
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
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"files"); 
    htmlHelp->incContentsDepth();
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
      bool src = fd->generateSource() || Config::sourceBrowseFlag;
      if ((doc || src) && !fd->isReference())
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
            htmlHelp->addContentsItem(fd->name(),fd->getOutputFileBase());
          }
        }
        else
        {
          ol.startBold();
          ol.docify(fd->name());
          ol.endBold();
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
  endFile(ol);
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------
#if 0
void writeSourceIndex(OutputList &ol)
{
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,"sources","Source Index");
  startTitle(ol,0);
  QCString title = theTranslator->trSources();
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"sources"); 
    htmlHelp->incContentsDepth();
  }
  //ol.newParagraph();
  //parseText(ol,theTranslator->trFileListDescription(Config::extractAllFlag));
  //ol.newParagraph();
  ol.endTextBlock();

  //ol.startIndexList();
  bool started=FALSE;
  FileName *fn=inputNameList.first();
  while (fn)
  {
    FileDef *fd=fn->first();
    while (fd)
    {
      if (!fd->isReference())
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
        ol.writeObjectLink(0,fd->sourceName(),0,fd->name());
        ol.writeString("\n");
        if (Config::generateHtml && Config::htmlHelpFlag)
        {
          HtmlHelp::getInstance()->addContentsItem(
              fd->name(),fd->sourceName());
        }
      }
      fd=fn->next();
    }
    fn=inputNameList.next();
  }
  if (started) ol.endItemList();
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  endFile(ol);
  ol.enable(OutputGenerator::Man);
}
#endif

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
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"namespaces","Namespace Index");
  startTitle(ol,0);
  QCString title = theTranslator->trNamespaceList();
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"namespaces"); 
    htmlHelp->incContentsDepth();
  }
  //ol.newParagraph();
  parseText(ol,theTranslator->trNamespaceListDescription(Config::extractAllFlag));
  //ol.newParagraph();
  ol.endTextBlock();

  ol.startIndexList();
  NamespaceDef *nd=namespaceList.first();
  while (nd)
  {
    if (nd->isLinkableInProject())
    {
      ol.writeStartAnnoItem("namespace",nd->getOutputFileBase(),0,nd->name());
      ol.docify(" (");
      if (!nd->briefDescription().isEmpty())
      {
        OutputList briefOutput(&ol);
        parseDoc(briefOutput,
                 nd->getDefFileName(),nd->getDefLine(),
                 nd->name(),0,
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
      ol.docify(" (");
      if (!cd->briefDescription().isEmpty())
      {
        OutputList briefOutput(&ol);
        parseDoc(briefOutput,
                 cd->getDefFileName(),cd->getDefLine(),
                 cd->name(),0,
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
          NamespaceDef *nd = namespaceDict[namesp];
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
  if (annotatedClasses==0) return;
  
  //if (classList.count()==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"annotated","Annotated Index");
  startTitle(ol,0);
  QCString title = theTranslator->trCompoundList();
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"annotated"); 
    htmlHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trCompoundListDescription());
  //ol.newParagraph();
  ol.endTextBlock();
  writeAnnotatedClassList(ol);
  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    htmlHelp->decContentsDepth();
  }
  
  endFile(ol);
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
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
      ol.writeListItem();
      ol.docify(substituteClassNames(mn->memberName()));
      if (isFunc) ol.docify("()");
      ol.writeString("\n");

      int count=0;
      md=mn->last();
      QCString prevName;
      while (md)
      {
        ClassDef *cd=md->getClassDef();
        if (//cd && (md->isFriend() || md->protection()!=Private || Config::extractPrivateFlag) && 
            //!md->isReference() && md->hasDocumentation() && 
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
          //md->name()[0]!='@' && (cd=md->getClassDef()) && cd->isLinkable()) 
          md->isLinkableInProject() && 
          !md->isRelated() &&
          (cd=md->getClassDef()) && 
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
      //    //md->name()[0]!='@' && (cd=md->getClassDef()) && cd->isLinkable()
      //    md->isLinkableInProject() &&
      //    md->isRelated() &&
      //    (cd=md->getClassDef()) &&
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
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  //ol.disable(OutputGenerator::Man);
  //ol.disable(OutputGenerator::Latex);
  startFile(ol,"functions","Compound Member Index");
  startTitle(ol,0);
  parseText(ol,Config::projectName+" "+theTranslator->trCompoundMembers());
  endTitle(ol,0,0);
  parseText(ol,theTranslator->trCompoundMembersDescription(Config::extractAllFlag));
  writeMemberList(ol);
  endFile(ol);
  //ol.enable(OutputGenerator::Latex);
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
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
      FileDef *fd=md->getFileDef();
      bool hasDocs = md->getFileDef() && 
                     md->getFileDef()->isLinkableInProject();
      
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
        FileDef *fd=md->getFileDef();
        bool hasDocs = md->getFileDef() && 
                       md->getFileDef()->isLinkableInProject();
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
      NamespaceDef *nd=md->getNamespaceDef();
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
          (fd=md->getFileDef()) && 
          fd->isLinkableInProject()
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
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  startFile(ol,"globals","File Member Index");
  startTitle(ol,0);
  parseText(ol,Config::projectName+" "+theTranslator->trFileMembers());
  endTitle(ol,0,0);
  parseText(ol,theTranslator->trFileMembersDescription(Config::extractAllFlag));
  writeFileMemberList(ol);
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeNamespaceMemberIndex(OutputList &ol)
{
  if (documentedNamespaceMembers==0) return;
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);
  //ol.disable(OutputGenerator::Man);
  //ol.disable(OutputGenerator::Latex);
  startFile(ol,"namespacemembers","Namespace Member Index");
  startTitle(ol,0);
  parseText(ol,Config::projectName+" "+theTranslator->trNamespaceMembers());
  endTitle(ol,0,0);
  parseText(ol,theTranslator->trNamespaceMemberDescription(Config::extractAllFlag));
  writeNamespaceMemberList(ol);
  endFile(ol);
  //ol.enable(OutputGenerator::Latex);
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

//int countIncludeFiles()
//{
//  int count=0;
//  FileDef *fd=includeFiles.first();
//  while (fd)
//  {
//    //if (fd->isLinkableInProject())
//    //{
//      count++;
//    //}
//    fd=includeFiles.next();
//  }  
//  return count;
//}
//
////----------------------------------------------------------------------------
//
//void writeHeaderFileList(OutputList &ol)
//{
//  bool started=FALSE;
//  FileDef *fd=includeFiles.first();
//  while (fd)
//  {
//    /*
//    if (fd->isLinkableInProject())
//    {
//    */
//      if (!started)
//      {
//        started=TRUE;
//        ol.startItemList();
//      }
//      ol.writeListItem();
//      QCString path;
//      if (Config::fullPathNameFlag) 
//      {
//        path=stripFromPath(fd->getPath().copy());
//      }
//      if (!path.isEmpty()) ol.docify(path);
//      ol.writeObjectLink(0,fd->includeName(),0,fd->name());
//      ol.writeString("\n");
//      if (Config::generateHtml && Config::htmlHelpFlag)
//      {
//        HtmlHelp::getInstance()->addContentsItem(
//            fd->name(),fd->includeName());
//      }
//    /*
//    }
//    */
//    fd=includeFiles.next();
//  }  
//  if (started) ol.endItemList();
//}
//
////----------------------------------------------------------------------------
//
//void writeHeaderIndex(OutputList &ol)
//{
//  if (documentedIncludeFiles==0) return;
//  ol.disable(OutputGenerator::Man);
//  ol.disable(OutputGenerator::Latex);
//  startFile(ol,"headers","Header File Index");
//  startTitle(ol,0);
//  QCString title = Config::projectName+" "+theTranslator->trHeaderFiles();
//  parseText(ol,title);
//  endTitle(ol,0,0);
//  HtmlHelp *htmlHelp = 0;
//  if (Config::generateHtml && Config::htmlHelpFlag)
//  {
//    htmlHelp = HtmlHelp::getInstance();
//    htmlHelp->addContentsItem(title,"headers"); 
//    htmlHelp->incContentsDepth();
//  }
//  parseText(ol,theTranslator->trHeaderFilesDescription());
//  writeHeaderFileList(ol);
//  if (Config::generateHtml && Config::htmlHelpFlag)
//  {
//    htmlHelp->decContentsDepth();
//  }
//  endFile(ol);
//  ol.enable(OutputGenerator::Latex);
//  ol.enable(OutputGenerator::Man);
//}

//----------------------------------------------------------------------------

void writeExampleIndex(OutputList &ol)
{
  if (exampleList.count()==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"examples","Example Index");
  startTitle(ol,0);
  QCString title = theTranslator->trExamples();
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"examples"); 
    htmlHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trExamplesDescription());
  //ol.newParagraph();
  ol.endTextBlock();
  ol.startIndexList();
  PageInfo *pi=exampleList.first();
  while (pi)
  {
    ol.writeListItem();
    QCString n=convertFileName(pi->name)+"-example";
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
  //ol.enable(OutputGenerator::Man);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writePageIndex(OutputList &ol)
{
  if (pageList.count()==0) return;
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Man);
  startFile(ol,"pages","Page Index");
  startTitle(ol,0);
  QCString title = theTranslator->trRelatedPages();
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"pages"); 
    htmlHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trRelatedPagesDescription());
  //ol.newParagraph();
  ol.endTextBlock();
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
    
    //ol.writeListItem();
    ol.writeStartAnnoItem("pages",pageName,0,pageTitle);
    //ol.writeObjectLink(0,pageName,0,pageTitle);
    ol.writeEndAnnoItem(pageName);
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
    if (gd->countMembers()>0) count++;
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
    //printf("gd->name()=%s #members=%d\n",gd->name().data(),gd->countMembers());
    if (gd->countMembers()>0)
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
    }
  }
  ol.endDescription();
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
  parseDoc(ol,"graph_legend",1,0,0,theTranslator->trLegendDocs());
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeGroupIndex(OutputList &ol)
{
  if (documentedGroups==0) return; 
  ol.pushGeneratorState(); 
  ol.disable(OutputGenerator::Man);
  startFile(ol,"modules","Module Index");
  startTitle(ol,0);
  QCString title = theTranslator->trModules();
  if (!Config::projectName.isEmpty()) title.prepend(Config::projectName+" ");
  parseText(ol,title);
  endTitle(ol,0,0);
  ol.startTextBlock();
  HtmlHelp *htmlHelp = 0;
  bool hasHtmlHelp = Config::generateHtml && Config::htmlHelpFlag;
  if (hasHtmlHelp)
  {
    htmlHelp = HtmlHelp::getInstance();
    htmlHelp->addContentsItem(title,"modules"); 
    htmlHelp->incContentsDepth();
  }
  parseText(ol,theTranslator->trModulesDescription());
  ol.endTextBlock();
  writeGroupList(ol);
  if (hasHtmlHelp)
  {
    htmlHelp->decContentsDepth();
  }
  endFile(ol);
  ol.popGeneratorState();
}

//----------------------------------------------------------------------------

void writeIndex(OutputList &ol)
{
  // save old generator state
  //bool manEnabled = ol.isEnabled(OutputGenerator::Man);
  //bool texEnabled = ol.isEnabled(OutputGenerator::Latex);
  //bool htmEnabled = ol.isEnabled(OutputGenerator::Html);
  ol.pushGeneratorState();

  QCString projPrefix;
  if (!Config::projectName.isEmpty())
  {
    projPrefix=Config::projectName+" ";
  }
  // write HTML index
  //ol.disable(OutputGenerator::Man);
  //ol.disable(OutputGenerator::Latex);
  ol.disableAllBut(OutputGenerator::Html);

  QCString defFileName = 
    mainPage ? mainPage->defFileName.data() : "<generated>";
  int defLine =
    mainPage ? mainPage->defLine : 1;

  QCString title;
  if (!mainPage || mainPage->title.isEmpty())
  {
    title = "Main Index";
  }
  else 
  {
    title = mainPage->title;
  }
  ol.startFile("index",title,FALSE);

  if (Config::generateHtml && Config::htmlHelpFlag)
  {
    HtmlHelp::getInstance()->addContentsItem(title,"index"); 
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
  
  // write LaTeX/RTF index
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
  if (pageList.count()>0)
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

  if (mainPage)
  {
    ol.disable(OutputGenerator::Man);
    startFile(ol,mainPage->name,mainPage->title);
    SectionInfo *si=0;
    if (!mainPage->title.isEmpty() && !mainPage->name.isEmpty() &&
        (si=sectionDict[mainPage->name])!=0)
    {
      ol.writeSection(si->label,si->title,FALSE);
    }
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
