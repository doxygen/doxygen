/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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
#include <qregexp.h>
#include "config.h"
#include "definition.h"
#include "doxygen.h"
#include "language.h"
#include "message.h"
#include "outputlist.h"
#include "doc.h"
#include "code.h"
#include "util.h"
#include "groupdef.h"

Definition::Definition(const char *df,int dl,
                       const char *name,const char *b,const char *d)
{
  m_defFileName = df;
  m_defLine = dl;
  m_name=name; 
  if (m_name!="<globalScope>") 
    m_localName=stripScope(name);
  else
    m_localName=name;
  //printf("m_localName=%s\n",m_localName.data());
  m_brief=b; 
  m_doc=d; 
  m_sectionDict=0, 
  m_startBodyLine=m_endBodyLine=-1, 
  m_bodyDef=0;
  m_sourceRefByDict=0;
  m_sourceRefsDict=0;
  m_todoId=0;
  m_testId=0;
  m_bugId=0;
  m_deprecatedId=0;
  m_outerScope=Doxygen::globalScope;
  m_partOfGroups=0;
  m_specialListItems=0;
  m_briefLine=1;
  m_docFile=1;
}

Definition::~Definition()
{
  delete m_sectionDict;
  delete m_sourceRefByDict;
  delete m_sourceRefsDict;
  delete m_partOfGroups;
  delete m_specialListItems;
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
      if (m_sectionDict==0) 
      {
        m_sectionDict = new SectionDict(17);
      }
      if (m_sectionDict->find(*s)==0)
      {
        m_sectionDict->insert(*s,si);
      }
      si->definition = this;
    }
    s=anchorList->next();
  }
}

void Definition::writeDocAnchorsToTagFile()
{
  if (!Config_getString("GENERATE_TAGFILE").isEmpty() && m_sectionDict)
  {
    QDictIterator<SectionInfo> sdi(*m_sectionDict);
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

void Definition::setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace) 
{ 
  if (d==0) return;
  //printf("Definition::setDocumentation(%s,%s,%d)\n",d,docFile,docLine);
  if (stripWhiteSpace)
    m_doc=((QCString)d).stripWhiteSpace();
  else
    m_doc=d;
  m_docFile = docFile;
  m_docLine = docLine;
}

void Definition::setBriefDescription(const char *b,const char *briefFile,int briefLine) 
{ 
  if (b==0) return;
  //printf("Definition::setBriefDescription(%s,%s,%d)\n",b,briefFile,briefLine);
  m_brief=QCString(b).stripWhiteSpace(); 
  int bl=m_brief.length(); 
  if (bl>0) // add puntuation if needed
  {
    switch(m_brief.at(bl-1))
    {
      case '.': case '!': case '?': break;
      default: 
        if (isupper(m_brief.at(0))) m_brief+='.'; 
        break;
    }
  }
  m_briefFile = briefFile;
  m_briefLine = briefLine;
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
            col+=Config_getInt("TAB_SIZE") - (col%Config_getInt("TAB_SIZE"));
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
  if (Config_getBool("SOURCE_BROWSER") && m_startBodyLine!=-1 && m_bodyDef)
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
      lineStr.sprintf("%d",m_startBodyLine);
      anchorStr.sprintf("l%05d",m_startBodyLine);
      if (lineMarkerPos<fileMarkerPos) // line marker before file marker
      {
        // write text left from linePos marker
        parseText(ol,refText.left(lineMarkerPos)); 
        ol.disableAllBut(OutputGenerator::Html); 
        // write line link (HTML only)
        ol.writeObjectLink(0,m_bodyDef->getSourceFileBase(),
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
        ol.writeObjectLink(0,m_bodyDef->getSourceFileBase(),
            0,m_bodyDef->name());
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(m_bodyDef->name());
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
        ol.writeObjectLink(0,m_bodyDef->getSourceFileBase(),
            0,m_bodyDef->name());
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(m_bodyDef->name());
        ol.enableAll();
        
        // write text between markers
        parseText(ol,refText.mid(fileMarkerPos+2,
              lineMarkerPos-fileMarkerPos-2)); 

        ol.disableAllBut(OutputGenerator::Html); 
        // write line link (HTML only)
        ol.writeObjectLink(0,m_bodyDef->getSourceFileBase(),
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
  if (Config_getBool("INLINE_SOURCES") && m_startBodyLine!=-1 && 
      m_endBodyLine>=m_startBodyLine && m_bodyDef)
  {
    QCString codeFragment;
    int actualStart=m_startBodyLine,actualEnd=m_endBodyLine;
    if (readCodeFragment(m_bodyDef->absFilePath(),
          actualStart,actualEnd,codeFragment)
       )
    {
      initParseCodeContext();
      //printf("Read:\n`%s'\n\n",codeFragment.data());
      if (definitionType()==TypeMember) setParameterList((MemberDef *)this);
      ol.newParagraph();
      ol.startCodeFragment();
      parseCode(ol,scopeName,codeFragment,FALSE,0,
          m_bodyDef,actualStart,actualEnd,TRUE);
      ol.endCodeFragment();
    }
  }
  ol.popGeneratorState();
}

/*! Write a reference to the source code fragments in which this 
 *  definition is used.
 */
void Definition::writeSourceRefList(OutputList &ol,const char *scopeName,
    const QCString &text,MemberSDict *members,bool /*funcOnly*/)
{
  ol.pushGeneratorState();
  if (Config_getBool("SOURCE_BROWSER") && members)
  {
    ol.newParagraph();
    parseText(ol,text);
    ol.docify(" ");

    QCString ldefLine=theTranslator->trWriteList(members->count());

    QRegExp marker("@[0-9]+");
    int index=0,newIndex,matchLen;
    // now replace all markers in inheritLine with links to the classes
    while ((newIndex=marker.match(ldefLine,index,&matchLen))!=-1)
    {
      bool ok;
      parseText(ol,ldefLine.mid(index,newIndex-index));
      uint entryIndex = ldefLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
      MemberDef *md=members->at(entryIndex);
      if (ok && md)
      {
        QCString scope=md->getScopeString();
        QCString name=md->name();
        //printf("class=%p scope=%s scopeName=%s\n",md->getClassDef(),scope.data(),scopeName);
        if (!scope.isEmpty() && scope!=scopeName)
        {
          if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
          {
            name.prepend(scope+".");
          }
          else
          {
            name.prepend(scope+"::");
          }
        }
        if (md->isFunction() || md->isSlot() || 
            md->isPrototype() || md->isSignal()
           ) name+="()";
        Definition *d = md->getOuterScope();
        if (d==Doxygen::globalScope) d=md->getBodyDef();
        if (md->getStartBodyLine()!=-1 && md->getBodyDef()) 
        {
          //printf("md->getBodyDef()=%p global=%p\n",md->getBodyDef(),Doxygen::globalScope); 
          // for HTML write a real link
          ol.pushGeneratorState();
          ol.disableAllBut(OutputGenerator::Html);
          QCString lineStr,anchorStr;
          anchorStr.sprintf("l%05d",md->getStartBodyLine());
          //printf("Write object link to %s\n",md->getBodyDef()->getSourceFileBase().data());
          ol.writeObjectLink(0,md->getBodyDef()->getSourceFileBase(),anchorStr,name);
          ol.popGeneratorState();

          // for the other output formats just mention the name
          ol.pushGeneratorState();
          ol.disable(OutputGenerator::Html);
          ol.docify(name);
          ol.popGeneratorState();
        }
        else if (md->isLinkable() && d && d->isLinkable())
        {
          // for HTML write a real link
          ol.pushGeneratorState();
          ol.disableAllBut(OutputGenerator::Html);
          ol.writeObjectLink(d->getReference(),d->getOutputFileBase(),
                             md->anchor(),name);
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
      }
      index=newIndex+matchLen;
    } 
    parseText(ol,ldefLine.right(ldefLine.length()-index));
    ol.writeString(".");
  }
  ol.popGeneratorState();
}

void Definition::writeSourceReffedBy(OutputList &ol,const char *scopeName)
{
  writeSourceRefList(ol,scopeName,theTranslator->trReferencedBy(),m_sourceRefByDict,FALSE);
}

void Definition::writeSourceRefs(OutputList &ol,const char *scopeName)
{
  writeSourceRefList(ol,scopeName,theTranslator->trReferences(),m_sourceRefsDict,TRUE);
}

bool Definition::hasDocumentation() const
{ 
  return !m_doc.isEmpty() ||             // has detailed docs
         !m_brief.isEmpty() ||           // has brief description
         Config_getBool("EXTRACT_ALL");       // extract everything
}

void Definition::addSourceReferencedBy(MemberDef *md)
{
  if (md)
  {
    QCString name  = md->name();
    QCString scope = md->getScopeString();

    if (!scope.isEmpty())
    {
      name.prepend(scope+"::");
    }

    if (m_sourceRefByDict==0)
    {
      m_sourceRefByDict = new MemberSDict;
    }
    if (m_sourceRefByDict->find(name)==0)
    {
      m_sourceRefByDict->inSort(name,md);
    }
  }
}

void Definition::addSourceReferences(MemberDef *md)
{
  if (md)
  {
    QCString name  = md->name();
    QCString scope = md->getScopeString();

    if (!scope.isEmpty())
    {
      name.prepend(scope+"::");
    }

    if (m_sourceRefsDict==0)
    {
      m_sourceRefsDict = new MemberSDict;
    }
    if (m_sourceRefsDict->find(name)==0)
    {
      m_sourceRefsDict->inSort(name,md);
    }
  }
}

Definition *Definition::findInnerCompound(const char *)
{
  return 0;
}

void Definition::addInnerCompound(Definition *)
{
  err("Error: Definition::addInnerCompound() called\n");
}

QCString Definition::qualifiedName() const
{
  //printf("start Definition::qualifiedName() localName=%s\n",m_localName.data());
  if (m_outerScope==0) 
  {
    if (m_localName=="<globalScope>") return "";
    else return m_localName; 
  }

  QCString qualifiedName;
  if (m_outerScope->name()=="<globalScope>")
  {
    qualifiedName = m_localName.copy();
  }
  else
  {
    qualifiedName = m_outerScope->qualifiedName()+"::"+m_localName;
  }
  //printf("end Definition::qualifiedName()=%s\n",qualifiedName.data());
  return qualifiedName;
};

QCString Definition::localName() const
{
  return m_localName;
}

void Definition::setBodySegment(int bls,int ble) 
{
  //printf("setBodySegment(%d,%d) for %s\n",bls,ble,name().data());
  m_startBodyLine=bls; 
  m_endBodyLine=ble; 
}

void Definition::makePartOfGroup(GroupDef *gd)
{
  if (m_partOfGroups==0) m_partOfGroups = new GroupList;
  m_partOfGroups->append(gd);
}

void Definition::setRefItems(const QList<ListItemInfo> *sli)
{
  if (sli)
  {
    // deep copy the list
    if (m_specialListItems==0) 
    {
      m_specialListItems=new QList<ListItemInfo>;
      m_specialListItems->setAutoDelete(TRUE);
    }
    QListIterator<ListItemInfo> slii(*sli);
    ListItemInfo *lii;
    for (slii.toFirst();(lii=slii.current());++slii)
    {
      m_specialListItems->append(new ListItemInfo(*lii));
    } 
  }
}

int Definition::getSpecialListId(const char *listName) const
{
  if (m_specialListItems)
  {
    QListIterator<ListItemInfo> slii(*m_specialListItems);
    ListItemInfo *lii;
    for (slii.toFirst();(lii=slii.current());++slii)
    {
      if (strcmp(lii->type,listName)==0)
      {
        return lii->itemId;
      }
    }
  }
  return -1;
}

const QList<ListItemInfo> *Definition::specialListItems() const
{
  return m_specialListItems;
}

