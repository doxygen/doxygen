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

#include "qtbc.h"
#include <ctype.h>
#include "config.h"
#include "definition.h"
#include "doxygen.h"
#include "language.h"
#include "message.h"
#include "outputlist.h"
#include "scanner.h"
#include <qregexp.h>

Definition::Definition(const char *df,int dl,
                       const char *name,const char *b,const char *d)
{
  defFileName = df;
  defLine = dl;
  n=name; 
  brief=b; 
  doc=d; 
  sectionList=0, 
  startBodyLine=endBodyLine=-1, 
  bodyDef=0;
  sourceRefList=0;
  sourceRefDict=0;
}

Definition::~Definition()
{
  delete sectionList;
  delete sourceRefList;
  delete sourceRefDict;
}

QCString Definition::nameToFile(const char *name)
{
  return convertNameToFile(name);
#if 0
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
#endif
}

void Definition::addSectionsToDefinition(QList<QCString> *anchorList)
{
  if (!anchorList) return;
  QCString *s=anchorList->first();
  while (s)
  {
    SectionInfo *si=0;
    if (!s->isEmpty() && (si=sectionDict[*s]))
    {
      //printf("Add section `%s' to definition `%s'\n",
      //    si->label.data(),n.data());
      if (sectionList==0) sectionList = new SectionList;
      sectionList->append(si);
      si->definition = this;
    }
    s=anchorList->next();
  }
}

void Definition::setBriefDescription(const char *b) 
{ 
  brief=QCString(b).stripWhiteSpace(); 
  int bl=brief.length(); 
  if (bl>0) // add puntuation if needed
  {
    switch(brief.at(bl-1))
    {
      case '.': case '!': case '?': break;
      default: brief+='.'; break;
    }
  }
}

/*! Reads a fragment of code from file \a fileName starting at 
 * line \a startLine and ending at line \a endLine. The fragment is
 * stored in \a result. If FALSE is returned the code fragment could not be
 * found.
 *
 * The file is scanned for a opening bracket ('{') from \a startLine onward.
 * The line actually containing the bracket is returned via startLine.
 * The file is scanned for a closing bracket ('}') from \a endLine backward.
 * The line actually containing the bracket is returned via endLine.
 */
static bool readCodeFragment(const char *fileName,
                      int &startLine,int &endLine,QCString &result)
{
  //printf("readCodeFragment(%s,%d,%d)\n",fileName,startLine,endLine);
  if (fileName==0 || fileName[0]==0) return FALSE; // not a valid file name
  QFile f(fileName);
  if (f.open(IO_ReadOnly))
  {
    int c=0;
    int lineNr=1;
    // skip until the startLine has reached
    while (lineNr<startLine && !f.atEnd())
    {
      while ((c=f.getch())!='\n' && c!=-1) /* skip */;
      lineNr++; 
    }
    if (!f.atEnd())
    {
      // skip until the opening bracket is found
      while (lineNr<=endLine && !f.atEnd() && c!='{')
      {
        while ((c=f.getch())!='{' && c!=-1) if (c=='\n') lineNr++; 
      }
      if (c=='{') 
      {
        // copy until end of line
        result+=c;
        startLine=lineNr;
        const int maxLineLength=4096;
        char lineStr[maxLineLength];
        char *p=lineStr;
        while ((c=f.getch())!='\n' && c!=-1) *p++=c;
        //printf("First line str=`%s' atEnd=%d lineNr=%d endLine=%d\n",lineStr.data(),f.atEnd(),lineNr,endLine);
        *p++='\n';
        *p++='\0';
        while (lineNr<endLine && !f.atEnd())
        {
          //printf("adding line=`%s' lineNr=%d\n",lineStr.data(),lineNr);
          result+=lineStr;  
          f.readLine(lineStr,maxLineLength);
          lineNr++; 
        }
        p=lineStr+strlen(lineStr);
        while (--p>=lineStr && *p!='}') /* skip */;
        *(++p)='\n';
        *(++p)='\0';
        result+=lineStr;
        endLine=lineNr;
        return TRUE;
      }
    }
  }
  return FALSE;
}

/*! Write a reference to the source code defining this definition */
void Definition::writeSourceDef(OutputList &ol,const char *scopeName)
{
  ol.pushGeneratorState();
  //printf("Definition::writeSourceRef %d %p\n",bodyLine,bodyDef);
  if (Config::sourceBrowseFlag && startBodyLine!=-1 && bodyDef)
  {
    //ol.disable(OutputGenerator::RTF);
    ol.newParagraph();
    //ol.enableAll();

    QCString refText = theTranslator->trDefinedAtLineInSourceFile();
    int lineMarkerPos = refText.find("@0");
    int fileMarkerPos = refText.find("@1");
    if (lineMarkerPos!=-1 && fileMarkerPos!=-1) // should always pass this.
    {
      QCString lineStr,anchorStr;
      lineStr.sprintf("%d",startBodyLine);
      anchorStr.sprintf("l%05d",startBodyLine);
      if (lineMarkerPos<fileMarkerPos) // line marker before file marker
      {
        // write text left from linePos marker
        parseText(ol,refText.left(lineMarkerPos)); 
        ol.disableAllBut(OutputGenerator::Html); 
        // write line link (HTML only)
        ol.writeObjectLink(0,bodyDef->sourceName(),
            anchorStr,lineStr);
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(lineStr);
        ol.enableAll();
        
        // write text between markers
        parseText(ol,refText.mid(lineMarkerPos+2,
              fileMarkerPos-lineMarkerPos-2));

        ol.disableAllBut(OutputGenerator::Html); 
        // write file link (HTML only)
        ol.writeObjectLink(0,bodyDef->sourceName(),
            0,bodyDef->name());
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(bodyDef->name());
        ol.enableAll();
        
        // write text right from file marker
        parseText(ol,refText.right(
              refText.length()-fileMarkerPos-2)); 
      }
      else // file marker before line marker
      {
        // write text left from file marker
        parseText(ol,refText.left(fileMarkerPos)); 
        ol.disableAllBut(OutputGenerator::Html); 
        // write file link (HTML only)
        ol.writeObjectLink(0,bodyDef->sourceName(),
            0,bodyDef->name());
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(bodyDef->name());
        ol.enableAll();
        
        // write text between markers
        parseText(ol,refText.mid(fileMarkerPos+2,
              lineMarkerPos-fileMarkerPos-2)); 

        ol.disableAllBut(OutputGenerator::Html); 
        // write line link (HTML only)
        ol.writeObjectLink(0,bodyDef->sourceName(),
            anchorStr,lineStr);
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(lineStr);
        ol.enableAll();

        // write text right from linePos marker
        parseText(ol,refText.right(
              refText.length()-lineMarkerPos-2)); 
      }
    }
    else
    {
      err("Error: translation error: invalid markers in trDefinedInSourceFile()\n");
    }

    ol.disableAllBut(OutputGenerator::RTF);
    ol.newParagraph();
    ol.enableAll();
  }
  if (Config::inlineSourceFlag && startBodyLine!=-1 && 
      endBodyLine>=startBodyLine && bodyDef)
  {
    //printf("Source Fragment %s: %d-%d\n",name().data(),
    //        startBodyLine,endBodyLine);
    QCString codeFragment;
    int actualStart=startBodyLine,actualEnd=endBodyLine;
    if (readCodeFragment(bodyDef->absFilePath(),
          actualStart,actualEnd,codeFragment)
       )
    {
      initParseCodeContext();
      //printf("Read:\n`%s'\n\n",codeFragment.data());
      ol.startCodeFragment();
      parseCode(ol,scopeName,codeFragment,FALSE,0,
          bodyDef,actualStart,actualEnd,TRUE);
      ol.endCodeFragment();
    }
  }
  ol.popGeneratorState();
}

/*! Write a reference to the source code fragments in which this 
 *  definition is used.
 */
void Definition::writeSourceRefs(OutputList &ol,const char *scopeName)
{
  ol.pushGeneratorState();
  if (Config::sourceBrowseFlag && sourceRefList)
  {
    ol.newParagraph();
    parseText(ol,theTranslator->trReferencedBy());
    ol.docify(" ");

    QCString defLine=theTranslator->trWriteList(sourceRefList->count());

    QRegExp marker("@[0-9]+");
    int index=0,newIndex,matchLen;
    // now replace all markers in inheritLine with links to the classes
    while ((newIndex=marker.match(defLine,index,&matchLen))!=-1)
    {
      bool ok;
      parseText(ol,defLine.mid(index,newIndex-index));
      uint entryIndex = defLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
      MemberDef *md=sourceRefList->at(entryIndex);
      if (ok && md)
      {
        QCString scope=md->getScopeString();
        QCString name=md->name();
        if (!scope.isEmpty() && scope!=scopeName)
        {
          name.prepend(scope+"::");
        }
        if (md->getStartBodyLine()!=-1 && md->getBodyDef()) 
        {
          QCString lineStr,anchorStr;
          anchorStr.sprintf("l%05d",md->getStartBodyLine());
          ol.writeObjectLink(0,md->getBodyDef()->sourceName(),
            anchorStr,name);
        }
        else
        {
          ol.docify(name);
        }
        ol.docify("()");
      }
      index=newIndex+matchLen;
    } 
    parseText(ol,defLine.right(defLine.length()-index));
    ol.writeString(".");
  }
  ol.popGeneratorState();
}



bool Definition::hasDocumentation() 
{ 
  return !doc.isEmpty() ||             // has detailed docs
         !brief.isEmpty() ||           // has brief description
         Config::extractAllFlag;       // extract everything
}

void Definition::addSourceReference(MemberDef *md)
{
  if (md)
  {
    QCString name=md->name();
    QCString scope=md->getScopeString();

    if (!scope.isEmpty())
    {
      name.prepend(scope+"::");
    }

    if (sourceRefList==0)
    {
      sourceRefDict = new MemberDict(53);
      sourceRefList = new MemberList;
    }
    if (sourceRefDict->find(name)==0)
    {
      sourceRefDict->insert(name,md);
      sourceRefList->inSort(md);
    }
  }
}

