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

#include "qtbc.h"
#include <ctype.h>
#include "config.h"
#include "definition.h"
#include "doxygen.h"
#include "language.h"
#include "message.h"
#include "outputlist.h"
#include "doc.h"
#include "code.h"
#include <qregexp.h>

Definition::Definition(const char *df,int dl,
                       const char *name,const char *b,const char *d)
{
  defFileName = df;
  defLine = dl;
  n=name; 
  brief=b; 
  doc=d; 
  sectionDict=0, 
  startBodyLine=endBodyLine=-1, 
  bodyDef=0;
  sourceRefList=0;
  sourceRefDict=0;
  m_todoId=0;
  m_testId=0;
  m_bugId=0;
}

Definition::~Definition()
{
  delete sectionDict;
  delete sourceRefList;
  delete sourceRefDict;
}

QCString Definition::nameToFile(const char *name,bool allowDots)
{
  return convertNameToFile(name,allowDots);
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
        if (Config::instance()->getBool("CASE_SENSE_NAMES"))
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
    if (!s->isEmpty() && (si=Doxygen::sectionDict[*s]))
    {
      //printf("Add section `%s' to definition `%s'\n",
      //    si->label.data(),n.data());
      if (sectionDict==0) 
      {
        sectionDict = new SectionDict(17);
      }
      if (sectionDict->find(*s)==0)
      {
        sectionDict->insert(*s,si);
      }
      si->definition = this;
    }
    s=anchorList->next();
  }
}

void Definition::writeDocAnchorsToTagFile()
{
  if (!Config::instance()->getString("GENERATE_TAGFILE").isEmpty() && sectionDict)
  {
    QDictIterator<SectionInfo> sdi(*sectionDict);
    SectionInfo *si;
    for (;(si=sdi.current());++sdi)
    {
      if (!si->generated)
      {
        if (definitionType()==TypeMember) Doxygen::tagFile << "  ";
        Doxygen::tagFile << "    <docanchor>" << si->label << "</docanchor>" << endl;
      }
    }
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
 * line \a startLine and ending at line \a endLine (inclusive). The fragment is
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
    int col=0;
    int lineNr=1;
    // skip until the startLine has reached
    while (lineNr<startLine && !f.atEnd())
    {
      while ((c=f.getch())!='\n' && c!=-1) /* skip */;
      lineNr++; 
    }
    if (!f.atEnd())
    {
      // skip until the opening bracket or lonely : is found
      bool found=FALSE;
      char cn=0;
      while (lineNr<=endLine && !f.atEnd() && !found)
      {
        while ((c=f.getch())!='{' && c!=':' && c!=-1) 
        {
          if (c=='\n') 
          {
            lineNr++,col=0; 
          }
          else if (c=='\t') 
          {
            col+=Config::instance()->getInt("TAB_SIZE") - (col%Config::instance()->getInt("TAB_SIZE"));
          }
          else
          {
            col++;
          }
        }
        if (c==':')
        {
          cn=f.getch();
          if (cn!=':') found=TRUE;
        }
        else if (c=='{')
        {
          found=TRUE;
        }
      }
      if (found) 
      {
        // For code with more than one line,
        // fill the line with spaces until we are at the right column
        // so that the opening brace lines up with the closing brace
        if (endLine!=startLine)
        {
          QCString spaces;
          spaces.fill(' ',col);
          result+=spaces;
        }
        // copy until end of line
        result+=c;
        if (c==':') result+=cn;
        startLine=lineNr;
        const int maxLineLength=4096;
        char lineStr[maxLineLength];
        do 
        {
          int size_read;
          do {
            // read up to maxLineLength-1 bytes, the last byte being zero
            size_read = f.readLine(lineStr, maxLineLength);
            result+=lineStr;
          } while (size_read == (maxLineLength-1));

          lineNr++; 
        } while (lineNr<=endLine && !f.atEnd());

        // strip stuff after closing bracket
        int newLineIndex = result.findRev('\n');
        int braceIndex   = result.findRev('}');
        if (braceIndex > newLineIndex) 
        {
          result.truncate(braceIndex+1);
        }
        endLine=lineNr-1;
        return TRUE;
      }
    }
  }
  return FALSE;
}

/*! Write a reference to the source code defining this definition */
void Definition::writeSourceDef(OutputList &ol,const char *)
{
  ol.pushGeneratorState();
  //printf("Definition::writeSourceRef %d %p\n",bodyLine,bodyDef);
  if (Config::instance()->getBool("SOURCE_BROWSER") && startBodyLine!=-1 && bodyDef)
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
  ol.popGeneratorState();
}

/*! Write code of this definition into the documentation */
void Definition::writeInlineCode(OutputList &ol,const char *scopeName)
{
  ol.pushGeneratorState();
  //printf("Source Fragment %s: %d-%d bodyDef=%p\n",name().data(),
  //        startBodyLine,endBodyLine,bodyDef);
  if (Config::instance()->getBool("INLINE_SOURCES") && startBodyLine!=-1 && 
      endBodyLine>=startBodyLine && bodyDef)
  {
    QCString codeFragment;
    int actualStart=startBodyLine,actualEnd=endBodyLine;
    if (readCodeFragment(bodyDef->absFilePath(),
          actualStart,actualEnd,codeFragment)
       )
    {
      initParseCodeContext();
      //printf("Read:\n`%s'\n\n",codeFragment.data());
      if (definitionType()==TypeMember) setParameterList((MemberDef *)this);
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
  if (Config::instance()->getBool("SOURCE_BROWSER") && sourceRefList)
  {
    ol.newParagraph();
    parseText(ol,theTranslator->trReferencedBy());
    ol.docify(" ");

    QCString ldefLine=theTranslator->trWriteList(sourceRefList->count());

    QRegExp marker("@[0-9]+");
    int index=0,newIndex,matchLen;
    // now replace all markers in inheritLine with links to the classes
    while ((newIndex=marker.match(ldefLine,index,&matchLen))!=-1)
    {
      bool ok;
      parseText(ol,ldefLine.mid(index,newIndex-index));
      uint entryIndex = ldefLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
      MemberDef *md=sourceRefList->at(entryIndex);
      if (ok && md)
      {
        QCString scope=md->getScopeString();
        QCString name=md->name();
        //printf("class=%p scope=%s scopeName=%s\n",md->getClassDef(),scope.data(),scopeName);
        if (!scope.isEmpty() && scope!=scopeName)
        {
          name.prepend(scope+"::");
        }
        if (md->getStartBodyLine()!=-1 && md->getBodyDef()) 
        {
          // for HTML write a real link
          ol.pushGeneratorState();
          ol.disableAllBut(OutputGenerator::Html);
          QCString lineStr,anchorStr;
          anchorStr.sprintf("l%05d",md->getStartBodyLine());
          ol.writeObjectLink(0,md->getBodyDef()->sourceName(),anchorStr,name);
          ol.popGeneratorState();

          // for the other output formats just mention the name
          ol.pushGeneratorState();
          ol.disable(OutputGenerator::Html);
          ol.docify(name);
          ol.popGeneratorState();
        }
        else
        {
          ol.docify(name);
        }
        ol.docify("()");
      }
      index=newIndex+matchLen;
    } 
    parseText(ol,ldefLine.right(ldefLine.length()-index));
    ol.writeString(".");
    ol.newParagraph();
  }
  ol.popGeneratorState();
}

bool Definition::hasDocumentation() 
{ 
  return !doc.isEmpty() ||             // has detailed docs
         !brief.isEmpty() ||           // has brief description
         Config::instance()->getBool("EXTRACT_ALL");       // extract everything
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

