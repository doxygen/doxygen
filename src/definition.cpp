/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2007 by Dimitri van Heesch.
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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "config.h"
#include "definition.h"
#include "doxygen.h"
#include "language.h"
#include "message.h"
#include "outputlist.h"
#include "code.h"
#include "util.h"
#include "groupdef.h"
#include "pagedef.h"
#include "section.h"
#include "htags.h"
#include "parserintf.h"
#include "marshal.h"

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define popen _popen
#define pclose _pclose
#endif

#define START_MARKER 0x4445465B // DEF[
#define END_MARKER   0x4445465D // DEF]


class DefinitionImpl
{
  public:
    DefinitionImpl();
   ~DefinitionImpl();
    void init(const char *df,int dl,
         const char *n);

    SectionDict *sectionDict;  // dictionary of all sections, not accessible

    MemberSDict *sourceRefByDict;       
    MemberSDict *sourceRefsDict;        
    QList<ListItemInfo> *xrefListItems; 
    GroupList *partOfGroups;            

    DocInfo   *details; // not exported
    BriefInfo *brief;   // not exported
    BodyInfo  *body;    // not exported

    QCString localName;      // local (unqualified) name of the definition
                               // in the future m_name should become m_localName
    QCString qualifiedName;
    QCString ref;   // reference to external documentation

    bool hidden;

    Definition *outerScope;  // not owner

    // where the item was found
    QCString defFileName;
    int      defLine;
    QCString defFileExt;
};

DefinitionImpl::DefinitionImpl() 
  : sectionDict(0), sourceRefByDict(0), sourceRefsDict(0), 
    xrefListItems(0), partOfGroups(0),
    details(0), brief(0), body(0), 
    outerScope(0)
{
}

DefinitionImpl::~DefinitionImpl()
{
  delete sectionDict;
  delete sourceRefByDict;
  delete sourceRefsDict;
  delete partOfGroups;
  delete xrefListItems;
  delete brief;
  delete details;
  delete body;
}

void DefinitionImpl::init(const char *df,int dl,
                          const char *n)
{
  defFileName = df;
  int lastDot = defFileName.findRev('.');
  if (lastDot!=-1)
  {
    defFileExt = defFileName.mid(lastDot);
  }
  defLine = dl;
  QCString name = n;
  if (name!="<globalScope>") 
  {
    //extractNamespaceName(m_name,m_localName,ns);
    localName=stripScope(n);
  }
  else
  {
    localName=n;
  }
  //printf("m_localName=%s\n",m_localName.data());

  brief = 0;
  details = 0;
  body = 0;
  sourceRefByDict=0;
  sourceRefsDict=0;
  sectionDict=0, 
  outerScope=Doxygen::globalScope;
  partOfGroups=0;
  xrefListItems=0;
  hidden = FALSE;
}

//-----------------------------------------------------------------------------------------

static bool matchExcludedSymbols(const char *name)
{
  static QStrList &exclSyms = Config_getList("EXCLUDE_SYMBOLS");
  if (exclSyms.count()==0) return FALSE; // nothing specified
  const char *pat = exclSyms.first();
  QCString symName = name;
  while (pat)
  {
    QCString pattern = pat;
    bool forceStart=FALSE;
    bool forceEnd=FALSE;
    if (pattern.at(0)=='^') 
      pattern=pattern.mid(1),forceStart=TRUE;
    if (pattern.at(pattern.length()-1)=='$') 
      pattern=pattern.left(pattern.length()-1),forceEnd=TRUE;
    if (pattern.find('*')!=-1) // wildcard mode
    {
      QRegExp re(substitute(pattern,"*",".*"),TRUE);
      int i,pl;
      i = re.match(symName,0,&pl);
      //printf("  %d = re.match(%s) pattern=%s\n",i,symName.data(),pattern.data());
      if (i!=-1) // wildcard match
      {
        int sl=symName.length();
        // check if it is a whole word match
        if ((i==0     || pattern.at(0)=='*'    || (!isId(symName.at(i-1))  && !forceStart)) &&
            (i+pl==sl || pattern.at(i+pl)=='*' || (!isId(symName.at(i+pl)) && !forceEnd))
           )
        {
          //printf("--> name=%s pattern=%s match at %d\n",symName.data(),pattern.data(),i);
          return TRUE;
        }
      }
    }
    else if (!pattern.isEmpty()) // match words
    {
      int i = symName.find(pattern);
      if (i!=-1) // we have a match!
      {
        int pl=pattern.length();
        int sl=symName.length();
        // check if it is a whole word match
        if ((i==0     || (!isId(symName.at(i-1))  && !forceStart)) &&
            (i+pl==sl || (!isId(symName.at(i+pl)) && !forceEnd))
           )
        {
          //printf("--> name=%s pattern=%s match at %d\n",symName.data(),pattern.data(),i);
          return TRUE; 
        }
      }
    }
    pat = exclSyms.next();
  }
  //printf("--> name=%s: no match\n",name);
  return FALSE;
}

void Definition::addToMap(const char *name,Definition *d)
{
  QCString symbolName = name;
  int index=computeQualifiedIndex(symbolName);
  if (index!=-1) symbolName=symbolName.mid(index+2);
  if (!symbolName.isEmpty()) 
  {
    //printf("******* adding symbol `%s' (%p)\n",symbolName.data(),d);
    DefinitionIntf *di=Doxygen::symbolMap->find(symbolName);
    //printf("  addToMap(%p): looking for symbol %s: %p\n",d,symbolName.data(),di);
    if (di==0) // new Symbol
    {
      //printf("  new symbol!\n");
      Doxygen::symbolMap->insert(symbolName,d);
    }
    else // existing symbol
    {
      //printf("  existing symbol: ");
      if (di->definitionType()==DefinitionIntf::TypeSymbolList) // already multiple symbols
      {
        //printf("adding to exiting list\n");
        DefinitionList *dl = (DefinitionList*)di;
        dl->append(d);
      }
      else // going from one to two symbols
      {
        Doxygen::symbolMap->take(symbolName);
        DefinitionList *dl = new DefinitionList;
        //printf("replacing symbol by list %p with elements %p and %p\n",dl,di,d);
        dl->append((Definition*)di);
        dl->append(d);
        Doxygen::symbolMap->insert(symbolName,dl);
      }
    }

    // auto resize if needed
    static int sizeIndex=9;
    if (Doxygen::symbolMap->size()>SDict_primes[sizeIndex])
    {
      Doxygen::symbolMap->resize(SDict_primes[++sizeIndex]);
    }

    d->_setSymbolName(symbolName);
  }
}

void Definition::removeFromMap(Definition *d)
{
  QCString symbolName = d->symbolName();
  int index=computeQualifiedIndex(symbolName);
  if (index!=-1) symbolName=symbolName.mid(index+2);
  if (!symbolName.isEmpty()) 
  {
    //printf("******* removing symbol `%s' (%p)\n",symbolName.data(),d);
    DefinitionIntf *di=Doxygen::symbolMap->find(symbolName);
    if (di)
    {
      ASSERT(di!=0);
      if (di!=d) // symbolName not unique
      {
        //printf("  removing from list: %p!\n",di);
        DefinitionList *dl = (DefinitionList*)di;
        bool b = dl->removeRef(d);
        ASSERT(b==TRUE);
        if (dl->isEmpty())
        {
          Doxygen::symbolMap->take(symbolName);
        }
      }
      else // symbolName unique
      {
        //printf("  removing symbol %p\n",di);
        Doxygen::symbolMap->take(symbolName);
      }
    }
  }
}

Definition::Definition(const char *df,int dl,
                       const char *name,const char *b,
                       const char *d,bool isSymbol)
{
  m_name = name;
  m_impl = new DefinitionImpl;
  m_impl->init(df,dl,name);
  m_isSymbol = isSymbol;
  if (isSymbol) addToMap(name,this);
  _setBriefDescription(b,df,dl);
  _setDocumentation(d,df,dl,TRUE);
  if (matchExcludedSymbols(name)) m_impl->hidden = TRUE;
}

Definition::~Definition()
{
  if (m_isSymbol) 
  {
    removeFromMap(this);
  }
  if (m_impl)
  {
    delete m_impl;
    m_impl=0;
  }
}

void Definition::addSectionsToDefinition(QList<SectionInfo> *anchorList)
{
  if (!anchorList) return;
  makeResident();
  //printf("%s: addSectionsToDefinition(%d)\n",name().data(),anchorList->count());
  SectionInfo *si=anchorList->first();
  while (si)
  {
    //printf("Add section `%s' to definition `%s'\n",
    //    si->label.data(),name().data());
    SectionInfo *gsi=Doxygen::sectionDict.find(si->label);
    if (gsi==0)
    {
      gsi = new SectionInfo(*si);
      Doxygen::sectionDict.insert(si->label,gsi);
    }
    if (m_impl->sectionDict==0) 
    {
      m_impl->sectionDict = new SectionDict(17);
    }
    if (m_impl->sectionDict->find(gsi->label)==0)
    {
      m_impl->sectionDict->insert(gsi->label,gsi);
      gsi->definition = this;
    }
    si=anchorList->next();
  }
}

void Definition::writeDocAnchorsToTagFile()
{
  makeResident();
  if (!Config_getString("GENERATE_TAGFILE").isEmpty() && m_impl->sectionDict)
  {
    //printf("%s: writeDocAnchorsToTagFile(%d)\n",name().data(),m_sectionDict->count());
    QDictIterator<SectionInfo> sdi(*m_impl->sectionDict);
    SectionInfo *si;
    for (;(si=sdi.current());++sdi)
    {
      if (!si->generated)
      {
        //printf("write an entry!\n");
        if (definitionType()==TypeMember) Doxygen::tagFile << "  ";
        Doxygen::tagFile << "    <docanchor file=\"" 
                         << si->fileName << "\">" << si->label 
                         << "</docanchor>" << endl;
      }
    }
  }
}

void Definition::_setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace) 
{ 
  if (d==0) return;
  //printf("Definition::setDocumentation(%s,%s,%d,%d)\n",d,docFile,docLine,stripWhiteSpace);
  QCString doc = d;
  if (stripWhiteSpace)
  {
    doc = stripLeadingAndTrailingEmptyLines(doc);
  }
  else // don't strip whitespace
  {
    doc=d;
  }
  //printf("setting docs for %s: `%s'\n",name().data(),m_doc.data());
  if (m_impl->details==0)
  {
    m_impl->details = new DocInfo;
  }
  m_impl->details->doc  = doc;
  m_impl->details->file = docFile;
  m_impl->details->line = docLine;
}

void Definition::setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace)
{
  if (d==0) return;
  makeResident();
  _setDocumentation(d,docFile,docLine,stripWhiteSpace);
}

#define uni_isupper(c) (QChar(c).category()==QChar::Letter_Uppercase)

void Definition::_setBriefDescription(const char *b,const char *briefFile,int briefLine)
{
  if (b==0) return;
  static QCString outputLanguage = Config_getEnum("OUTPUT_LANGUAGE");
  static bool needsDot = outputLanguage!="Japanese" && 
                         outputLanguage!="Chinese" &&
                         outputLanguage!="Korean";
  //fprintf(stderr,"Definition::setBriefDescription(%s,%s,%d)\n",b,briefFile,briefLine);
  if (m_impl->brief==0)
  {
    m_impl->brief = new BriefInfo;
  }
  m_impl->brief->doc=QCString(b).stripWhiteSpace();
  int bl=m_impl->brief->doc.length(); 
  if (bl>0 && needsDot) // add punctuation if needed
  {
    switch(m_impl->brief->doc.at(bl-1))
    {
      case '.': case '!': case '?': break;
      default: 
        if (uni_isupper(m_impl->brief->doc.at(0))) m_impl->brief->doc+='.'; 
        break;
    }
  }
  m_impl->brief->file = briefFile;
  m_impl->brief->line = briefLine;
  m_impl->brief->tooltip = parseCommentAsText(m_impl->brief->doc,briefFile,briefLine);
}

void Definition::setBriefDescription(const char *b,const char *briefFile,int briefLine) 
{ 
  if (b==0) return;
  makeResident();
  _setBriefDescription(b,briefFile,briefLine);
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
  QCString cmd=getFileFilter(fileName)+" \""+fileName+"\"";
  FILE *f = Config_getBool("FILTER_SOURCE_FILES") ? popen(cmd,"r") : fopen(fileName,"r");
  bool found=FALSE;
  if (f)
  {
    int c=0;
    int col=0;
    int lineNr=1;
    // skip until the startLine has reached
    while (lineNr<startLine && !feof(f))
    {
      while ((c=fgetc(f))!='\n' && c!=EOF) /* skip */;
      lineNr++; 
    }
    if (!feof(f))
    {
      // skip until the opening bracket or lonely : is found
      char cn=0;
      while (lineNr<=endLine && !feof(f) && !found)
      {
        while ((c=fgetc(f))!='{' && c!=':' && c!=EOF) 
        {
          //printf("parsing char `%c'\n",c);
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
          cn=fgetc(f);
          if (cn!=':') found=TRUE;
        }
        else if (c=='{')
        {
          found=TRUE;
        }
      }
      //printf(" -> readCodeFragment(%s,%d,%d) lineNr=%d\n",fileName,startLine,endLine,lineNr);
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
          //printf("reading line %d in range %d-%d\n",lineNr,startLine,endLine);
          int size_read;
          do 
          {
            // read up to maxLineLength-1 bytes, the last byte being zero
            char *p = fgets(lineStr, maxLineLength,f);
            //printf("  read %s",p);
            if (p) 
            {
              size_read=qstrlen(p); 
            }
            else 
            {
              size_read=-1;
            }
            result+=lineStr;
          } while (size_read == (maxLineLength-1));

          lineNr++; 
        } while (lineNr<=endLine && !feof(f));

        // strip stuff after closing bracket
        int newLineIndex = result.findRev('\n');
        int braceIndex   = result.findRev('}');
        if (braceIndex > newLineIndex) 
        {
          result.truncate(braceIndex+1);
        }
        endLine=lineNr-1;
      }
    }
    if (Config_getBool("FILTER_SOURCE_FILES")) pclose(f); else fclose(f);
  }
  result = transcodeCharacterStringToUTF8(result);
  return found;
}

/*! Write a reference to the source code defining this definition */
void Definition::writeSourceDef(OutputList &ol,const char *)
{
  makeResident();
  ol.pushGeneratorState();
  //printf("Definition::writeSourceRef %d %p\n",bodyLine,bodyDef);
  if (Config_getBool("SOURCE_BROWSER") && 
      m_impl->body && m_impl->body->startLine!=-1 && m_impl->body->fileDef)
  {
    QCString refText = theTranslator->trDefinedAtLineInSourceFile();
    int lineMarkerPos = refText.find("@0");
    int fileMarkerPos = refText.find("@1");
    if (lineMarkerPos!=-1 && fileMarkerPos!=-1) // should always pass this.
    {
      QCString lineStr,anchorStr;
      lineStr.sprintf("%d",m_impl->body->startLine);
      anchorStr.sprintf(Htags::useHtags ? "L%d" : "l%05d",m_impl->body->startLine);
      ol.newParagraph();
      if (lineMarkerPos<fileMarkerPos) // line marker before file marker
      {
        // write text left from linePos marker
        ol.parseText(refText.left(lineMarkerPos)); 
        ol.disableAllBut(OutputGenerator::Html); 
        // write line link (HTML only)
        ol.writeObjectLink(0,m_impl->body->fileDef->getSourceFileBase(),
            anchorStr,lineStr);
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(lineStr);
        ol.enableAll();
        
        // write text between markers
        ol.parseText(refText.mid(lineMarkerPos+2,
              fileMarkerPos-lineMarkerPos-2));

        ol.disableAllBut(OutputGenerator::Html); 
        // write file link (HTML only)
        ol.writeObjectLink(0,m_impl->body->fileDef->getSourceFileBase(),
            0,m_impl->body->fileDef->name());
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(m_impl->body->fileDef->name());
        ol.enableAll();
        
        // write text right from file marker
        ol.parseText(refText.right(
              refText.length()-fileMarkerPos-2)); 
      }
      else // file marker before line marker
      {
        // write text left from file marker
        ol.parseText(refText.left(fileMarkerPos)); 
        ol.disableAllBut(OutputGenerator::Html); 
        // write file link (HTML only)
        ol.writeObjectLink(0,m_impl->body->fileDef->getSourceFileBase(),
            0,m_impl->body->fileDef->name());
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(m_impl->body->fileDef->name());
        ol.enableAll();
        
        // write text between markers
        ol.parseText(refText.mid(fileMarkerPos+2,
              lineMarkerPos-fileMarkerPos-2)); 

        ol.disableAllBut(OutputGenerator::Html); 
        // write line link (HTML only)
        ol.writeObjectLink(0,m_impl->body->fileDef->getSourceFileBase(),
            anchorStr,lineStr);
        ol.enableAll();
        ol.disable(OutputGenerator::Html);
        // write normal text (Latex/Man only)
        ol.docify(lineStr);
        ol.enableAll();

        // write text right from linePos marker
        ol.parseText(refText.right(
              refText.length()-lineMarkerPos-2)); 
      }
    }
    else
    {
      err("Error: translation error: invalid markers in trDefinedInSourceFile()\n");
    }
  }
  ol.popGeneratorState();
}

void Definition::setBodySegment(int bls,int ble) 
{
  makeResident();
  //printf("setBodySegment(%d,%d) for %s\n",bls,ble,name().data());
  if (m_impl->body==0) m_impl->body = new BodyInfo;
  m_impl->body->startLine=bls; 
  m_impl->body->endLine=ble; 
}

void Definition::setBodyDef(FileDef *fd)         
{
  makeResident();
  if (m_impl->body==0) m_impl->body = new BodyInfo;
  m_impl->body->fileDef=fd; 
}

/*! Write code of this definition into the documentation */
void Definition::writeInlineCode(OutputList &ol,const char *scopeName)
{
  makeResident();
  ol.pushGeneratorState();
  //printf("Source Fragment %s: %d-%d bodyDef=%p\n",name().data(),
  //        m_startBodyLine,m_endBodyLine,m_bodyDef);
  if (Config_getBool("INLINE_SOURCES") && 
      m_impl->body && m_impl->body->startLine!=-1 && 
      m_impl->body->endLine>=m_impl->body->startLine && m_impl->body->fileDef)
  {
    QCString codeFragment;
    int actualStart=m_impl->body->startLine,actualEnd=m_impl->body->endLine;
    if (readCodeFragment(m_impl->body->fileDef->absFilePath(),
          actualStart,actualEnd,codeFragment)
       )
    {
      ParserInterface *pIntf = Doxygen::parserManager->getParser(m_impl->defFileExt);
      pIntf->resetCodeParserState();
      //printf("Read:\n`%s'\n\n",codeFragment.data());
      MemberDef *thisMd = 0;
      if (definitionType()==TypeMember) thisMd = (MemberDef *)this;
      ol.startCodeFragment();
      pIntf->parseCode(ol,               // codeOutIntf
                       scopeName,        // scope
                       codeFragment,     // input
                       FALSE,            // isExample
                       0,                // exampleName
                       m_impl->body->fileDef,  // fileDef
                       actualStart,      // startLine
                       actualEnd,        // endLine
                       TRUE,             // inlineFragment
                       thisMd            // memberDef
                      );
      ol.endCodeFragment();
      ol.newParagraph();
    }
  }
  ol.popGeneratorState();
}

/*! Write a reference to the source code fragments in which this 
 *  definition is used.
 */
void Definition::_writeSourceRefList(OutputList &ol,const char *scopeName,
    const QCString &text,MemberSDict *members,bool /*funcOnly*/)
{
  ol.pushGeneratorState();
  if (Config_getBool("SOURCE_BROWSER") && members)
  {
    ol.newParagraph();
    ol.parseText(text);
    ol.docify(" ");

    QCString ldefLine=theTranslator->trWriteList(members->count());

    QRegExp marker("@[0-9]+");
    int index=0,newIndex,matchLen;
    // now replace all markers in inheritLine with links to the classes
    while ((newIndex=marker.match(ldefLine,index,&matchLen))!=-1)
    {
      bool ok;
      ol.parseText(ldefLine.mid(index,newIndex-index));
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
        if (!md->isObjCMethod() &&
            (md->isFunction() || md->isSlot() || 
             md->isPrototype() || md->isSignal()
            )
           ) name+="()";
        //Definition *d = md->getOutputFileBase();
        //if (d==Doxygen::globalScope) d=md->getBodyDef();
        if (!(md->isLinkable() && !Config_getBool("REFERENCES_LINK_SOURCE")) && md->getStartBodyLine()!=-1 && md->getBodyDef()) 
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
        else if (md->isLinkable() /*&& d && d->isLinkable()*/)
        {
          // for HTML write a real link
          ol.pushGeneratorState();
          ol.disableAllBut(OutputGenerator::Html);
          ol.writeObjectLink(md->getReference(),
                             md->getOutputFileBase(),
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
    ol.parseText(ldefLine.right(ldefLine.length()-index));
    ol.writeString(".");
  }
  ol.popGeneratorState();
}

void Definition::writeSourceReffedBy(OutputList &ol,const char *scopeName)
{
  makeResident();
  if (Config_getBool("REFERENCED_BY_RELATION"))
  {
    _writeSourceRefList(ol,scopeName,theTranslator->trReferencedBy(),m_impl->sourceRefByDict,FALSE);
  }
}

void Definition::writeSourceRefs(OutputList &ol,const char *scopeName)
{
  makeResident();
  if (Config_getBool("REFERENCES_RELATION"))
  {
    _writeSourceRefList(ol,scopeName,theTranslator->trReferences(),m_impl->sourceRefsDict,TRUE);
  }
}

bool Definition::hasDocumentation() const
{ 
  static bool extractAll    = Config_getBool("EXTRACT_ALL"); 
  static bool sourceBrowser = Config_getBool("SOURCE_BROWSER");
  makeResident();
  bool hasDocs = 
         (m_impl->details && !m_impl->details->doc.isEmpty()) || // has detailed docs
         (m_impl->brief && !m_impl->brief->doc.isEmpty())     || // has brief description
         extractAll ||                   // extract everything
         (sourceBrowser && m_impl->body && 
          m_impl->body->startLine!=-1 && m_impl->body->fileDef); // link to definition
  return hasDocs;
}

bool Definition::hasUserDocumentation() const
{
  makeResident();
  bool hasDocs = 
         (m_impl->details && !m_impl->details->doc.isEmpty()) ||
         (m_impl->brief   && !m_impl->brief->doc.isEmpty());
  return hasDocs;
}

void Definition::addSourceReferencedBy(MemberDef *md)
{
  if (md)
  {
    makeResident();
    QCString name  = md->name();
    QCString scope = md->getScopeString();

    if (!scope.isEmpty())
    {
      name.prepend(scope+"::");
    }

    if (m_impl->sourceRefByDict==0)
    {
      m_impl->sourceRefByDict = new MemberSDict;
    }
    if (m_impl->sourceRefByDict->find(name)==0)
    {
      m_impl->sourceRefByDict->inSort(name,md);
    }
  }
}

void Definition::addSourceReferences(MemberDef *md)
{
  if (md)
  {
    QCString name  = md->name();
    QCString scope = md->getScopeString();
    makeResident();

    if (!scope.isEmpty())
    {
      name.prepend(scope+"::");
    }

    if (m_impl->sourceRefsDict==0)
    {
      m_impl->sourceRefsDict = new MemberSDict;
    }
    if (m_impl->sourceRefsDict->find(name)==0)
    {
      //printf("Adding reference %s->%s\n",md->name().data(),name.data());
      m_impl->sourceRefsDict->inSort(name,md);
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

QCString Definition::qualifiedName() 
{
  makeResident();
  if (!m_impl->qualifiedName.isEmpty()) return m_impl->qualifiedName;
  
  //printf("start Definition::qualifiedName() localName=%s\n",m_localName.data());
  if (m_impl->outerScope==0) 
  {
    if (m_impl->localName=="<globalScope>") return "";
    else return m_impl->localName; 
  }

  if (m_impl->outerScope->name()=="<globalScope>")
  {
    m_impl->qualifiedName = m_impl->localName.copy();
  }
  else
  {
    m_impl->qualifiedName = m_impl->outerScope->qualifiedName()+"::"+m_impl->localName;
  }
  //printf("end Definition::qualifiedName()=%s\n",m_qualifiedName.data());
  return m_impl->qualifiedName;
};

void Definition::setOuterScope(Definition *d) 
{
  makeResident();
  if (m_impl->outerScope!=d)
  { 
    m_impl->qualifiedName.resize(0); // flush cached scope name
    m_impl->outerScope = d; 
  }
  m_impl->hidden = m_impl->hidden || d->isHidden();
}

QCString Definition::localName() const
{
  makeResident();
  return m_impl->localName;
}

void Definition::makePartOfGroup(GroupDef *gd)
{
  makeResident();
  if (m_impl->partOfGroups==0) m_impl->partOfGroups = new GroupList;
  m_impl->partOfGroups->append(gd);
}

void Definition::setRefItems(const QList<ListItemInfo> *sli)
{
  if (sli)
  {
    makeResident();
    // deep copy the list
    if (m_impl->xrefListItems==0) 
    {
      m_impl->xrefListItems=new QList<ListItemInfo>;
      m_impl->xrefListItems->setAutoDelete(TRUE);
    }
    QListIterator<ListItemInfo> slii(*sli);
    ListItemInfo *lii;
    for (slii.toFirst();(lii=slii.current());++slii)
    {
      m_impl->xrefListItems->append(new ListItemInfo(*lii));
    } 
  }
}

void Definition::mergeRefItems(Definition *d)
{
  LockingPtr< QList<ListItemInfo> > xrefList = d->xrefListItems();
  if (xrefList!=0)
  {
    makeResident();
    // deep copy the list
    if (m_impl->xrefListItems==0) 
    {
      m_impl->xrefListItems=new QList<ListItemInfo>;
      m_impl->xrefListItems->setAutoDelete(TRUE);
    }
    QListIterator<ListItemInfo> slii(*xrefList);
    ListItemInfo *lii;
    for (slii.toFirst();(lii=slii.current());++slii)
    {
      if (_getXRefListId(lii->type)==-1)
      {
        m_impl->xrefListItems->append(new ListItemInfo(*lii));
      }
    } 
  }
}

int Definition::_getXRefListId(const char *listName) const
{
  makeResident();
  if (m_impl->xrefListItems)
  {
    QListIterator<ListItemInfo> slii(*m_impl->xrefListItems);
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

LockingPtr< QList<ListItemInfo> > Definition::xrefListItems() const
{
  makeResident();
  return LockingPtr< QList<ListItemInfo> >(this,m_impl->xrefListItems);
}


QCString Definition::convertNameToFile(const char *name,bool allowDots) const
{
  makeResident();
  if (!m_impl->ref.isEmpty())
  {
    return name;
  }
  else
  {
    return ::convertNameToFile(name,allowDots);
  }
}

void Definition::writePathFragment(OutputList &ol) const
{
  makeResident();
  if (m_impl->outerScope && m_impl->outerScope!=Doxygen::globalScope)
  {
    m_impl->outerScope->writePathFragment(ol);
    if (m_impl->outerScope->definitionType()==Definition::TypeClass ||
        m_impl->outerScope->definitionType()==Definition::TypeNamespace)
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
      {
        ol.writeString(".");
      }
      else
      {
        ol.writeString("::");
      }
    }
    else
    {
      ol.writeString("&nbsp;");
      ol.writeString("&raquo");
      ol.writeString("&nbsp;");
    }
  }
  if (isLinkable())
  {
    if (definitionType()==Definition::TypeGroup && ((const GroupDef*)this)->groupTitle())
    {
      ol.writeObjectLink(getReference(),getOutputFileBase(),0,((const GroupDef*)this)->groupTitle());
    }
    else if (definitionType()==Definition::TypePage && !((const PageDef*)this)->title().isEmpty())
    {
      ol.writeObjectLink(getReference(),getOutputFileBase(),0,((const PageDef*)this)->title());
    }
    else
    {
      ol.writeObjectLink(getReference(),getOutputFileBase(),0,m_impl->localName);
    }
  }
  else
  {
    ol.startBold();
    ol.docify(m_impl->localName);
    ol.endBold();
  }
}

void Definition::writeNavigationPath(OutputList &ol) const
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputGenerator::Html);

  ol.writeString("<div class=\"nav\">\n");
  writePathFragment(ol);
  ol.writeString("</div>\n");

  ol.popGeneratorState();
}

QCString Definition::symbolName() const 
{ 
  return m_symbolName; 
}

QCString Definition::documentation() const 
{ 
  makeResident();
  return m_impl->details ? m_impl->details->doc : QCString(""); 
}

int Definition::docLine() const 
{ 
  makeResident();
  return m_impl->details ? m_impl->details->line : 1; 
}

QCString Definition::docFile() const 
{ 
  makeResident();
  return m_impl->details ? m_impl->details->file : QCString("<"+m_name+">"); 
}

QCString Definition::briefDescription() const 
{ 
  makeResident();
  return m_impl->brief ? m_impl->brief->doc : QCString(""); 
}

QCString Definition::briefDescriptionAsTooltip() const
{
  makeResident();
  return m_impl->brief ? m_impl->brief->tooltip : QCString(""); 
}

int Definition::briefLine() const 
{ 
  makeResident();
  return m_impl->brief ? m_impl->brief->line : 1; 
}

QCString Definition::briefFile() const 
{ 
  makeResident();
  return m_impl->brief ? m_impl->brief->file : QCString("<"+m_name+">"); 
}

QCString Definition::getDefFileName() const 
{ 
  makeResident();
  return m_impl->defFileName; 
}

QCString Definition::getDefFileExtension() const 
{ 
  makeResident();
  return m_impl->defFileExt; 
}

int Definition::getDefLine() const 
{ 
  makeResident();
  return m_impl->defLine; 
}

bool Definition::isHidden() const
{
  makeResident();
  return m_impl->hidden;
}

bool Definition::isVisibleInProject() const 
{ 
  return isLinkableInProject() && !m_impl->hidden; 
}

bool Definition::isVisible() const
{ 
  return isLinkable() && !m_impl->hidden; 
}

QCString Definition::getReference() const 
{ 
  makeResident();
  return m_impl->ref; 
}

bool Definition::isReference() const 
{ 
  makeResident();
  return !m_impl->ref.isEmpty(); 
}

int Definition::getStartBodyLine() const         
{ 
  makeResident();
  return m_impl->body ? m_impl->body->startLine : -1; 
}

int Definition::getEndBodyLine() const           
{ 
  makeResident();
  return m_impl->body ? m_impl->body->endLine : -1; 
}

FileDef *Definition::getBodyDef()                
{ 
  makeResident();
  return m_impl->body ? m_impl->body->fileDef : 0; 
}

LockingPtr<GroupList> Definition::partOfGroups() const 
{ 
  makeResident();
  return LockingPtr<GroupList>(this,m_impl->partOfGroups); 
}

Definition *Definition::getOuterScope() const 
{ 
  makeResident();
  return m_impl->outerScope; 
}

LockingPtr<MemberSDict> Definition::getReferencesMembers() const 
{ 
  makeResident();
  return LockingPtr<MemberSDict>(this,m_impl->sourceRefsDict); 
}

LockingPtr<MemberSDict> Definition::getReferencedByMembers() const 
{ 
  makeResident();
  return LockingPtr<MemberSDict>(this,m_impl->sourceRefByDict); 
}

void Definition::setReference(const char *r) 
{ 
  makeResident();
  m_impl->ref=r; 
}

void Definition::_setSymbolName(const QCString &name) 
{ 
  m_symbolName=name; 
}

void Definition::setHidden(bool b) 
{ 
  makeResident();
  m_impl->hidden = m_impl->hidden || b; 
}

void Definition::setLocalName(const QCString name) 
{ 
  makeResident();
  m_impl->localName=name; 
}

void Definition::makeResident() const
{
}


void Definition::flushToDisk() const
{
  //printf("%p: Definition::flushToDisk()\n",this);
  Definition *that = (Definition *)this;
  //printf("Definition::flushToDisk(): pos=%d\n",(int)m_storagePos); 
  marshalUInt(Doxygen::symbolStorage,START_MARKER);
  marshalSectionDict  (Doxygen::symbolStorage,m_impl->sectionDict);
  marshalMemberSDict  (Doxygen::symbolStorage,m_impl->sourceRefByDict);
  marshalMemberSDict  (Doxygen::symbolStorage,m_impl->sourceRefsDict);
  marshalItemInfoList (Doxygen::symbolStorage,m_impl->xrefListItems);
  marshalGroupList    (Doxygen::symbolStorage,m_impl->partOfGroups);
  marshalDocInfo      (Doxygen::symbolStorage,m_impl->details);
  marshalBriefInfo    (Doxygen::symbolStorage,m_impl->brief);
  marshalBodyInfo     (Doxygen::symbolStorage,m_impl->body);
  marshalQCString     (Doxygen::symbolStorage,m_impl->localName);
  marshalQCString     (Doxygen::symbolStorage,m_impl->qualifiedName);
  marshalQCString     (Doxygen::symbolStorage,m_impl->ref);
  marshalBool         (Doxygen::symbolStorage,m_impl->hidden);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->outerScope);
  marshalQCString     (Doxygen::symbolStorage,m_impl->defFileName);
  marshalInt          (Doxygen::symbolStorage,m_impl->defLine);
  marshalQCString     (Doxygen::symbolStorage,m_impl->defFileExt);
  marshalUInt(Doxygen::symbolStorage,END_MARKER);
  delete that->m_impl;
  that->m_impl = 0;
}

void Definition::loadFromDisk() const
{
  //printf("%p: Definition::loadFromDisk()\n",this);
  Definition *that = (Definition *)this;
  assert(m_impl==0);
  that->m_impl = new DefinitionImpl;
  uint marker = unmarshalUInt(Doxygen::symbolStorage);
  assert(marker==START_MARKER);
  m_impl->sectionDict     = unmarshalSectionDict  (Doxygen::symbolStorage);
  m_impl->sourceRefByDict = unmarshalMemberSDict  (Doxygen::symbolStorage);
  m_impl->sourceRefsDict  = unmarshalMemberSDict  (Doxygen::symbolStorage);
  m_impl->xrefListItems   = unmarshalItemInfoList (Doxygen::symbolStorage);
  m_impl->partOfGroups    = unmarshalGroupList    (Doxygen::symbolStorage);
  m_impl->details         = unmarshalDocInfo      (Doxygen::symbolStorage);
  m_impl->brief           = unmarshalBriefInfo    (Doxygen::symbolStorage);
  m_impl->body            = unmarshalBodyInfo     (Doxygen::symbolStorage);
  m_impl->localName       = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->qualifiedName   = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->ref             = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->hidden          = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->outerScope      = (Definition *)unmarshalObjPointer   (Doxygen::symbolStorage);
  m_impl->defFileName     = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->defLine         = unmarshalInt          (Doxygen::symbolStorage);
  m_impl->defFileExt      = unmarshalQCString     (Doxygen::symbolStorage);
  marker = unmarshalUInt(Doxygen::symbolStorage);
  assert(marker==END_MARKER);
}

