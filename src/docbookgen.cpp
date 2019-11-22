/******************************************************************************
*
*
*
* Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qintdict.h>
#include <qregexp.h>
#include "docbookgen.h"
#include "doxygen.h"
#include "message.h"
#include "config.h"
#include "classlist.h"
#include "classdef.h"
#include "diagram.h"
#include "util.h"
#include "defargs.h"
#include "outputgen.h"
#include "dot.h"
#include "dotcallgraph.h"
#include "dotclassgraph.h"
#include "dotdirdeps.h"
#include "dotgroupcollaboration.h"
#include "dotincldepgraph.h"
#include "pagedef.h"
#include "filename.h"
#include "version.h"
#include "docbookvisitor.h"
#include "docparser.h"
#include "language.h"
#include "parserintf.h"
#include "arguments.h"
#include "memberlist.h"
#include "groupdef.h"
#include "memberdef.h"
#include "namespacedef.h"
#include "membername.h"
#include "membergroup.h"
#include "dirdef.h"
#include "section.h"

// no debug info
#define Docbook_DB(x) do {} while(0)
// debug to stdout
//#define Docbook_DB(x) printf x
// debug inside output
//#define Docbook_DB(x) QCString __t;__t.sprintf x;m_t << __t

#if 0
#define DB_GEN_C DB_GEN_C1(t)
#define DB_GEN_C1(x) x << "<!-- DB_GEN_C " << __LINE__ << " -->\n";
#define DB_GEN_C2(y) DB_GEN_C2a(t,y)
#define DB_GEN_C2a(x,y) x << "<!-- DB_GEN_C " << __LINE__ << " " << y << " -->\n";
#else
#define DB_GEN_C
#define DB_GEN_C1(x)
#define DB_GEN_C2(y)
#define DB_GEN_C2a(x,y)
#endif
//------------------

inline void writeDocbookString(FTextStream &t,const char *s)
{
  t << convertToDocBook(s);
}

inline void writeDocbookCodeString(FTextStream &t,const char *s, int &col)
{
  char c;
  while ((c=*s++))
  {
    switch(c)
    {
      case '\t':
        {
          static int tabSize = Config_getInt(TAB_SIZE);
          int spacesToNextTabStop = tabSize - (col%tabSize);
          col+=spacesToNextTabStop;
          while (spacesToNextTabStop--) t << "&#32;";
          break;
        }
      case ' ':  t << "&#32;"; col++;  break;
      case '<':  t << "&lt;"; col++;   break;
      case '>':  t << "&gt;"; col++;   break;
      case '&':  t << "&amp;"; col++;  break;
      case '\'': t << "&apos;"; col++; break;
      case '"':  t << "&quot;"; col++; break;
      case '\007':  t << "^G"; col++; break; // bell
      case '\014':  t << "^L"; col++; break; // form feed
      default:   t << c; col++;        break;
    }
  }
}

static void addIndexTerm(FTextStream &t, QCString prim, QCString sec = "")
{
  t << "<indexterm><primary>";
  t << convertToDocBook(prim);
  t << "</primary>";
  if (!sec.isEmpty())
  {
    t << "<secondary>";
    t << convertToDocBook(sec);
    t << "</secondary>";
  }
  t << "</indexterm>" << endl;
}
void writeDocbookLink(FTextStream &t,const char * /*extRef*/,const char *compoundId,
    const char *anchorId,const char * text,const char * /*tooltip*/)
{
  t << "<link linkend=\"_" << stripPath(compoundId);
  if (anchorId) t << "_1" << anchorId;
  t << "\"";
  t << ">";
  writeDocbookString(t,text);
  t << "</link>";
}

DocbookCodeGenerator::DocbookCodeGenerator(FTextStream &t) : m_lineNumber(-1), m_col(0),
  m_insideCodeLine(FALSE), m_insideSpecialHL(FALSE)
{
  m_prettyCode=Config_getBool(DOCBOOK_PROGRAMLISTING);
  setTextStream(t);
}

DocbookCodeGenerator::DocbookCodeGenerator() : m_lineNumber(-1), m_col(0),
  m_insideCodeLine(FALSE), m_insideSpecialHL(FALSE),  m_streamSet(FALSE)
{
  m_prettyCode=Config_getBool(DOCBOOK_PROGRAMLISTING);
}

DocbookCodeGenerator::~DocbookCodeGenerator() {}

void DocbookCodeGenerator::codify(const char *text)
{
  Docbook_DB(("(codify \"%s\")\n",text));
  writeDocbookCodeString(m_t,text,m_col);
}
void DocbookCodeGenerator::writeCodeLink(const char *ref,const char *file,
    const char *anchor,const char *name,
    const char *tooltip)
{
  Docbook_DB(("(writeCodeLink)\n"));
  writeDocbookLink(m_t,ref,file,anchor,name,tooltip);
  m_col+=(int)strlen(name);
}
void DocbookCodeGenerator::writeCodeLinkLine(const char *ref,const char *file,
    const char *anchor,const char *name,
    const char *tooltip)
{
  Docbook_DB(("(writeCodeLinkLine)\n"));
  m_t << "<anchor xml:id=\"_" << stripExtensionGeneral(stripPath(file),".xml");
  m_t << "_1l";
  writeDocbookString(m_t,name);
  m_t << "\"/>";
  m_col+=(int)strlen(name);
}
void DocbookCodeGenerator::writeTooltip(const char *, const DocLinkInfo &, const char *,
                  const char *, const SourceLinkInfo &, const SourceLinkInfo &
                 )
{
  Docbook_DB(("(writeToolTip)\n"));
}
void DocbookCodeGenerator::startCodeLine(bool)
{
  Docbook_DB(("(startCodeLine)\n"));
  m_insideCodeLine=TRUE;
  m_col=0;
}
void DocbookCodeGenerator::endCodeLine()
{
  if (m_insideCodeLine) m_t << endl;
  Docbook_DB(("(endCodeLine)\n"));
  m_lineNumber = -1;
  m_refId.resize(0);
  m_external.resize(0);
  m_insideCodeLine=FALSE;
}
void DocbookCodeGenerator::startFontClass(const char *colorClass)
{
  Docbook_DB(("(startFontClass)\n"));
  m_t << "<emphasis role=\"" << colorClass << "\">";
  m_insideSpecialHL=TRUE;
}
void DocbookCodeGenerator::endFontClass()
{
  Docbook_DB(("(endFontClass)\n"));
  m_t << "</emphasis>"; // non DocBook
  m_insideSpecialHL=FALSE;
}
void DocbookCodeGenerator::writeCodeAnchor(const char *)
{
  Docbook_DB(("(writeCodeAnchor)\n"));
}
void DocbookCodeGenerator::writeLineNumber(const char *ref,const char *fileName,
    const char *anchor,int l)
{
  Docbook_DB(("(writeLineNumber)\n"));
  m_insideCodeLine = TRUE;
  if (m_prettyCode)
  {
    QCString lineNumber;
    lineNumber.sprintf("%05d",l);

    if (fileName && !m_sourceFileName.isEmpty())
    {
      writeCodeLinkLine(ref,m_sourceFileName,anchor,lineNumber,0);
      writeCodeLink(ref,fileName,anchor,lineNumber,0);
    }
    else
    {
      codify(lineNumber);
    }
    m_t << " ";
  }
  else
  {
    m_t << l << " ";
  }
  m_col=0;
}
void DocbookCodeGenerator::setCurrentDoc(const Definition *,const char *,bool)
{
}
void DocbookCodeGenerator::addWord(const char *,bool)
{
}
void DocbookCodeGenerator::finish()
{
  endCodeLine();
}
void DocbookCodeGenerator::startCodeFragment()
{
  m_t << "<literallayout><computeroutput>" << endl;
}
void DocbookCodeGenerator::endCodeFragment()
{
  //endCodeLine checks is there is still an open code line, if so closes it.
  endCodeLine();

  m_t << "</computeroutput></literallayout>" << endl;
}

DocbookGenerator::DocbookGenerator() : OutputGenerator()
{
DB_GEN_C
  dir=Config_getString(DOCBOOK_OUTPUT);
  //insideTabbing=FALSE;
  //firstDescItem=TRUE;
  //disableLinks=FALSE;
  //m_indent=0;
  //templateMemberItem = FALSE;
  m_prettyCode=Config_getBool(DOCBOOK_PROGRAMLISTING);
  m_denseText = FALSE;
  m_inGroup = FALSE;
  m_inDetail = FALSE;
  m_levelListItem = 0;
  m_descTable = FALSE;
  m_inLevel = -1;
  m_firstMember = FALSE;
  for (int i = 0 ; i < sizeof(m_inListItem) / sizeof(*m_inListItem) ; i++) m_inListItem[i] = FALSE;
  for (int i = 0 ; i < sizeof(m_inSimpleSect) / sizeof(*m_inSimpleSect) ; i++) m_inSimpleSect[i] = FALSE;
}

DocbookGenerator::~DocbookGenerator()
{
DB_GEN_C
}

void DocbookGenerator::init()
{
  QCString dir=Config_getString(DOCBOOK_OUTPUT);
  QDir d(dir);
  if (!d.exists() && !d.mkdir(dir))
  {
    term(1,"Could not create output directory %s\n",dir.data());
  }

  createSubDirs(d);
}

void DocbookGenerator::startFile(const char *name,const char *,const char *)
{
DB_GEN_C
  QCString fileName=name;
  QCString pageName;
  QCString fileType="section";
  if (fileName == "refman")
  {
    fileName="index";
    fileType="book";
  }
  else if (fileName == "index")
  {
    fileName="mainpage";
    fileType="chapter";
  }
  pageName = fileName;
  relPath = relativePathToRoot(fileName);
  if (fileName.right(4)!=".xml") fileName+=".xml";
  startPlainFile(fileName);
  m_codeGen.setTextStream(t);
  m_codeGen.setRelativePath(relPath);
  m_codeGen.setSourceFileName(stripPath(fileName));

  t << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>" << endl;;
  t << "<" << fileType << " xmlns=\"http://docbook.org/ns/docbook\" version=\"5.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"";
  if (!pageName.isEmpty()) t << " xml:id=\"_" <<  stripPath(pageName) << "\"";
  t << ">" << endl;
}

void DocbookGenerator::endFile()
{
DB_GEN_C
  if (m_inDetail) t << "</section>" << endl;
  m_inDetail = FALSE;
  while (m_inLevel != -1)
  {
    t << "</section>" << endl;
    m_inLevel--;
  }
  if (m_inGroup) t << "</section>" << endl;
  m_inGroup = FALSE;

  QCString fileType="section";
  QCString fileName= m_codeGen.sourceFileName();
  if (fileName == "index.xml")
  {
    fileType="book";
  }
  else if (fileName == "mainpage.xml")
  {
    fileType="chapter";
  }
  t << "</" << fileType << ">" << endl;
  endPlainFile();
  m_codeGen.setSourceFileName("");
}

void DocbookGenerator::startIndexSection(IndexSections is)
{
DB_GEN_C2("IndexSections " << is)
  switch (is)
  {
    case isTitlePageStart:
      {
        QCString dbk_projectName = Config_getString(PROJECT_NAME);
        t << "    <info>" << endl;
        t << "    <title>" << convertToDocBook(dbk_projectName) << "</title>" << endl;
        t << "    </info>" << endl;
      }
      break;
    case isTitlePageAuthor:
      break;
    case isMainPage:
      t << "<chapter>" << endl;
      t << "    <title>";
      break;
    case isModuleIndex:
      //Module Index}\n"
      break;
    case isDirIndex:
      //Directory Index}\n"
      break;
    case isNamespaceIndex:
      //Namespace Index}\n"
      break;
    case isClassHierarchyIndex:
      //Hierarchical Index}\n"
      break;
    case isCompoundIndex:
      //t << "{"; //Class Index}\n"
      break;
    case isFileIndex:
      //Annotated File Index}\n"
      break;
    case isPageIndex:
      //Annotated Page Index}\n"
      break;
    case isModuleDocumentation:
      t << "<chapter>\n";
      t << "    <title>";
      break;
    case isDirDocumentation:
      t << "<chapter>\n";
      t << "    <title>";
      break;
    case isNamespaceDocumentation:
      t << "<chapter>\n";
      t << "    <title>";
      break;
    case isClassDocumentation:
      t << "<chapter>\n";
      t << "    <title>";
      break;
    case isFileDocumentation:
      t << "<chapter>\n";
      t << "    <title>";
      break;
    case isExampleDocumentation:
      t << "<chapter>\n";
      t << "    <title>";
      break;
    case isPageDocumentation:
      break;
    case isPageDocumentation2:
      break;
    case isEndIndex:
      break;
  }
}

void DocbookGenerator::endIndexSection(IndexSections is)
{
DB_GEN_C2("IndexSections " << is)
  static bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  switch (is)
  {
    case isTitlePageStart:
      break;
    case isTitlePageAuthor:
      break;
    case isMainPage:
      t << "</title>" << endl;
      t << "    <xi:include href=\"mainpage.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
      t << "</chapter>" << endl;
      break;
    case isModuleIndex:
      //t << "</chapter>" << endl;
      break;
    case isDirIndex:
      //t << "<xi:include href=\"dirs.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //t << "</chapter>" << endl;
      break;
    case isNamespaceIndex:
      //t << "<xi:include href=\"namespaces.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //t << "</chapter>" << endl;
      break;
    case isClassHierarchyIndex:
      //t << "<xi:include href=\"hierarchy.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //t << "</chapter>" << endl;
      break;
    case isCompoundIndex:
      //t << "</chapter>" << endl;
      break;
    case isFileIndex:
      //t << "<xi:include href=\"files.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //t << "</chapter>" << endl;
      break;
    case isPageIndex:
      //t << "<xi:include href=\"pages.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //t << "</chapter>" << endl;
      break;
    case isModuleDocumentation:
      {
        t << "</title>" << endl;
        GroupSDict::Iterator gli(*Doxygen::groupSDict);
        GroupDef *gd;
        bool found=FALSE;
        for (gli.toFirst();(gd=gli.current()) && !found;++gli)
        {
          if (!gd->isReference())
          {
            t << "    <xi:include href=\"" << gd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
            found=TRUE;
          }
        }
        for (;(gd=gli.current());++gli)
        {
          if (!gd->isReference())
          {
            t << "    <xi:include href=\"" << gd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
          }
        }
      }
      t << "</chapter>\n";
      break;
    case isDirDocumentation:
      {
        t << "</title>" << endl;
        SDict<DirDef>::Iterator dli(*Doxygen::directories);
        DirDef *dd;
        bool found=FALSE;
        for (dli.toFirst();(dd=dli.current()) && !found;++dli)
        {
          if (dd->isLinkableInProject())
          {
            t << "<    xi:include href=\"" << dd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
            found=TRUE;
          }
        }
        for (;(dd=dli.current());++dli)
        {
          if (dd->isLinkableInProject())
          {
            t << "    <xi:include href=\"" << dd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
          }
        }
      }
      t << "</chapter>\n";
      break;
    case isNamespaceDocumentation:
      {
        t << "</title>" << endl;
        NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
        NamespaceDef *nd;
        bool found=FALSE;
        for (nli.toFirst();(nd=nli.current()) && !found;++nli)
        {
          if (nd->isLinkableInProject())
          {
            t << "<xi:include href=\"" << nd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
            found=TRUE;
          }
        }
        while ((nd=nli.current()))
        {
          if (nd->isLinkableInProject())
          {
            t << "<xi:include href=\"" << nd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
          }
          ++nli;
        }
      }
      t << "</chapter>\n";
      break;
    case isClassDocumentation:
      {
        t << "</title>" << endl;
        ClassSDict::Iterator cli(*Doxygen::classSDict);
        const ClassDef *cd=0;
        bool found=FALSE;
        for (cli.toFirst();(cd=cli.current()) && !found;++cli)
        {
          if (cd->isLinkableInProject() &&
              cd->templateMaster()==0 &&
             !cd->isEmbeddedInOuterScope()
             )
          {
            t << "    <xi:include href=\"" << cd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
            found=TRUE;
          }
        }
        for (;(cd=cli.current());++cli)
        {
          if (cd->isLinkableInProject() &&
              cd->templateMaster()==0 &&
             !cd->isEmbeddedInOuterScope()
             )
          {
            t << "    <xi:include href=\"" << cd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
          }
        }
      }
      t << "</chapter>\n";
      break;
    case isFileDocumentation:
      {
        t << "</title>" << endl;
        bool isFirst=TRUE;
        FileNameListIterator fnli(*Doxygen::inputNameList);
        FileName *fn;
        for (fnli.toFirst();(fn=fnli.current());++fnli)
        {
          FileNameIterator fni(*fn);
          const FileDef *fd;
          for (;(fd=fni.current());++fni)
          {
            if (fd->isLinkableInProject())
            {
              if (isFirst)
              {
                t << "    <xi:include href=\"" << fd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
                if (sourceBrowser && m_prettyCode && fd->generateSourceFile())
                {
                  t << "    <xi:include href=\"" << fd->getSourceFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
                }
                isFirst=FALSE;
              }
              else
              {
                t << "    <xi:include href=\"" << fd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
                if (sourceBrowser && m_prettyCode && fd->generateSourceFile())
                {
                  t << "    <xi:include href=\"" << fd->getSourceFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
                }
              }
            }
          }
        }
      }
      t << "</chapter>\n";
      break;
    case isExampleDocumentation:
      {
        t << "</title>" << endl;
        PageSDict::Iterator pdi(*Doxygen::exampleSDict);
        PageDef *pd=pdi.toFirst();
        if (pd)
        {
          t << "    <xi:include href=\"" << pd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
        }
        for (++pdi;(pd=pdi.current());++pdi)
        {
          t << "    <xi:include href=\"" << pd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
        }
      }
      t << "</chapter>\n";
      break;
    case isPageDocumentation:
      break;
    case isPageDocumentation2:
      break;
    case isEndIndex:
      t << "<index/>" << endl;
      break;
  }
}
void DocbookGenerator::writePageLink(const char *name, bool /*first*/)
{
DB_GEN_C
  PageSDict::Iterator pdi(*Doxygen::pageSDict);
  PageDef *pd = pdi.toFirst();
  for (pd = pdi.toFirst();(pd=pdi.current());++pdi)
  {
    if (!pd->getGroupDef() && !pd->isReference() && pd->name() == stripPath(name))
    {
      t << "<chapter>\n";
      if (pd->hasTitle())
      {
        t << "    <title>" << convertToDocBook(pd->title()) << "</title>" << endl;
      }
      else
      {
        t << "    <title>" << convertToDocBook(pd->name()) << "</title>" << endl;
      }
      t << "    <xi:include href=\"" << pd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>" << endl;
      t << "</chapter>\n";
    }
  }
}
void DocbookGenerator::writeDoc(DocNode *n,const Definition *ctx,const MemberDef *)
{
DB_GEN_C
  DocbookDocVisitor *visitor =
    new DocbookDocVisitor(t,*this);
  n->accept(visitor);
  delete visitor;
}

void DocbookGenerator::startParagraph(const char *)
{
DB_GEN_C
  t << "<para>" << endl;
}

void DocbookGenerator::endParagraph()
{
DB_GEN_C
  t << "</para>" << endl;
}
void DocbookGenerator::writeString(const char *text)
{
DB_GEN_C
  t << text;
}
void DocbookGenerator::startMemberHeader(const char *name,int)
{
DB_GEN_C
  t << "<simplesect>" << endl;
  m_inSimpleSect[m_levelListItem] = TRUE;
  t << "    <title>";
}

void DocbookGenerator::endMemberHeader()
{
DB_GEN_C
  t << "    </title>" << endl;
}
void DocbookGenerator::docify(const char *str)
{
DB_GEN_C
  t << convertToDocBook(str);
}
void DocbookGenerator::writeObjectLink(const char *ref, const char *f,
                                     const char *anchor, const char *text)
{
DB_GEN_C
  if (anchor)
    if (f) t << "<link linkend=\"_" << stripPath(f) << "_1" << anchor << "\">";
    else   t << "<link linkend=\"_" << anchor << "\">";
  else
    t << "<link linkend=\"_" << stripPath(f) << "\">";
  docify(text);
  t << "</link>";
}
void DocbookGenerator::startMemberList()
{
DB_GEN_C
  t << "        <itemizedlist>" << endl;
  m_levelListItem++;
}
void DocbookGenerator::endMemberList()
{
DB_GEN_C
  if (m_inListItem[m_levelListItem]) t << "</listitem>" << endl;
  m_inListItem[m_levelListItem] = FALSE;
  t << "        </itemizedlist>" << endl;
  m_levelListItem = (m_levelListItem> 0 ?  m_levelListItem - 1 : 0);
  if (m_inSimpleSect[m_levelListItem]) t << "</simplesect>" << endl;
  m_inSimpleSect[m_levelListItem] = FALSE;
}
void DocbookGenerator::startMemberItem(const char *,int,const char *)
{
DB_GEN_C
  if (m_inListItem[m_levelListItem]) t << "</listitem>" << endl;
  t << "            <listitem><para>";
  m_inListItem[m_levelListItem] = TRUE;
}
void DocbookGenerator::endMemberItem()
{
DB_GEN_C
  t << "</para>" << endl;
}
void DocbookGenerator::startBold(void)
{
DB_GEN_C
  t << "<emphasis role=\"strong\">";
}
void DocbookGenerator::endBold(void)
{
DB_GEN_C
  t << "</emphasis>";
}
void DocbookGenerator::startGroupHeader(int extraIndentLevel)
{
DB_GEN_C2("m_inLevel " << m_inLevel)
DB_GEN_C2("extraIndentLevel " << extraIndentLevel)
  m_firstMember = TRUE;
  if (m_inSimpleSect[m_levelListItem]) t << "</simplesect>" << endl;
  m_inSimpleSect[m_levelListItem] = FALSE;
  if (m_inLevel != -1) m_inGroup = TRUE;
  if (m_inLevel == extraIndentLevel) t << "</section>" << endl;
  m_inLevel = extraIndentLevel;
  t << "<section>" << endl;
  t << "<title>";
}
void DocbookGenerator::writeRuler(void)
{
DB_GEN_C2("m_inLevel " << m_inLevel)
DB_GEN_C2("m_inGroup " << m_inGroup)
  if (m_inGroup) t << "</section>" << endl;
  m_inGroup = FALSE;
}

void DocbookGenerator::endGroupHeader(int)
{
DB_GEN_C
  t << "</title>" << endl;
}

void DocbookGenerator::startParameterList(bool openBracket)
{
DB_GEN_C
  if (openBracket) t << "(";
}
void DocbookGenerator::endParameterList()
{
DB_GEN_C
}
void DocbookGenerator::writeNonBreakableSpace(int n)
{
DB_GEN_C
  for (int i=0;i<n;i++) t << " ";
}
void DocbookGenerator::lineBreak(const char *)
{
DB_GEN_C
  t << endl;
}
void DocbookGenerator::startTypewriter()
{
DB_GEN_C
  if (!m_denseText) t << "<computeroutput>";
}
void DocbookGenerator::endTypewriter()
{
DB_GEN_C
  if (!m_denseText) t << "</computeroutput>" << endl;
}
void DocbookGenerator::startTextBlock(bool dense)
{
DB_GEN_C
  if (dense)
  {
    m_denseText = TRUE;
    t << "<programlisting>";
  }
}
void DocbookGenerator::endTextBlock(bool dense)
{
DB_GEN_C
  if (m_denseText)
  {
    m_denseText = FALSE;
    t << "</programlisting>";
  }
}
void DocbookGenerator::startMemberDoc(const char *clname, const char *memname, const char *anchor, const char *title,
                                      int memCount, int memTotal, bool showInline)
{
DB_GEN_C2("m_inLevel " << m_inLevel)
  t << "    <section>" << endl;
  t << "    <title>" << convertToDocBook(title);
  if (memTotal>1)
  {
    t << "<computeroutput>[" << memCount << "/" << memTotal << "]</computeroutput>";
  }
  t << "</title>" << endl;
  if (memname && memname[0]!='@')
  {
    addIndexTerm(t,memname,clname);
    addIndexTerm(t,clname,memname);
  }
}
void DocbookGenerator::endMemberDoc(bool)
{
DB_GEN_C
  t << "</computeroutput></para>";
}
void DocbookGenerator::startTitleHead(const char *)
{
DB_GEN_C
  t << "<title>";
}
void DocbookGenerator::endTitleHead(const char *fileName,const char *name)
{
DB_GEN_C
  t << "</title>" << endl;
  if (name) addIndexTerm(t, name);
}
void DocbookGenerator::startDoxyAnchor(const char *fName,const char *manName,
                                 const char *anchor,const char *name,
                                 const char *args)
{
DB_GEN_C
  if (!m_inListItem[m_levelListItem] && !m_descTable)
  {
    if (!m_firstMember) t << "    </section>";
    m_firstMember = FALSE;
  }
  if (anchor)
  {
    t << "<anchor xml:id=\"_" << stripPath(fName) << "_1" << anchor << "\"/>";
  }
}
void DocbookGenerator::endDoxyAnchor(const char *fileName,const char *anchor)
{
DB_GEN_C
}
void DocbookGenerator::startMemberDocName(bool)
{
DB_GEN_C
  t << "<para><computeroutput>";
}
void DocbookGenerator::endMemberDocName()
{
DB_GEN_C
}
void DocbookGenerator::startMemberGroupHeader(bool hasHeader)
{
DB_GEN_C
  t << "<simplesect><title>";
}
void DocbookGenerator::endMemberGroupHeader()
{
DB_GEN_C
  t << "</title>" << endl;
}
void DocbookGenerator::startMemberGroup()
{
DB_GEN_C
}
void DocbookGenerator::endMemberGroup(bool)
{
DB_GEN_C
  t << "</simplesect>" << endl;
}
void DocbookGenerator::startClassDiagram()
{
DB_GEN_C
  t << "<para>";
}

void DocbookGenerator::endClassDiagram(const ClassDiagram &d, const char *fileName,const char *)
{
DB_GEN_C
  t << "    <informalfigure>" << endl;
  t << "        <mediaobject>" << endl;
  t << "            <imageobject>" << endl;
  t << "                <imagedata width=\"50%\" align=\"center\" valign=\"middle\" scalefit=\"0\" fileref=\"" 
                         << relPath << fileName << ".png\">" << "</imagedata>" << endl;
  t << "            </imageobject>" << endl;
  d.writeImage(t,dir,relPath,fileName,FALSE);
  t << "        </mediaobject>" << endl;
  t << "    </informalfigure>" << endl;
  t << "</para>" << endl;
}
void  DocbookGenerator::startLabels()
{
DB_GEN_C
}

void  DocbookGenerator::writeLabel(const char *l,bool isLast)
{
DB_GEN_C
  t << "<computeroutput>[" << l << "]</computeroutput>";
  if (!isLast) t << ", ";
}

void  DocbookGenerator::endLabels()
{
DB_GEN_C
}
void DocbookGenerator::startExamples()
{
DB_GEN_C
  t << "<simplesect><title>";
  docify(theTranslator->trExamples());
  t << "</title>";
}

void DocbookGenerator::endExamples()
{
DB_GEN_C
  t << "</simplesect>" << endl;
}
void DocbookGenerator::startSubsubsection(void)
{
DB_GEN_C
  t << "<simplesect><title>";
}
void DocbookGenerator::endSubsubsection(void)
{
DB_GEN_C
  t << "</title></simplesect>" << endl;
}
void DocbookGenerator::writeChar(char c)
{
DB_GEN_C
  char cs[2];
  cs[0]=c;
  cs[1]=0;
  docify(cs);
}
void DocbookGenerator::startMemberDocPrefixItem()
{
DB_GEN_C
  t << "<computeroutput>";
}
void DocbookGenerator::endMemberDocPrefixItem()
{
DB_GEN_C
  t << "</computeroutput>";
}
void DocbookGenerator::exceptionEntry(const char* prefix,bool closeBracket)
{
DB_GEN_C
  if (prefix)
    t << " " << prefix << "(";
  else if (closeBracket)
    t << ")";
  t << " ";
}
void DocbookGenerator::startParameterName(bool)
{
DB_GEN_C
  t << " ";
}
void DocbookGenerator::endParameterName(bool last,bool /*emptyList*/,bool closeBracket)
{
DB_GEN_C
  if (last)
  {
    if (closeBracket) t << ")";
  }
}
void DocbookGenerator::startCodeFragment()
{
DB_GEN_C
    t << "<programlisting>";
}
void DocbookGenerator::endCodeFragment()
{
DB_GEN_C
  //endCodeLine checks is there is still an open code line, if so closes it.
  endCodeLine();

    t << "</programlisting>";
}
void DocbookGenerator::startMemberTemplateParams()
{
DB_GEN_C
}

void DocbookGenerator::endMemberTemplateParams(const char *,const char *)
{
DB_GEN_C
  t << "</para>";
  t << "<para>";
}
void DocbookGenerator::startSection(const char *lab,const char *,SectionInfo::SectionType type)
{
DB_GEN_C
  t << "    <section xml:id=\"_" << stripPath(lab) << "\">";
  t << "<title>";
}
void DocbookGenerator::endSection(const char *lab,SectionInfo::SectionType)
{
DB_GEN_C
  t << "</title>";
  t << "    </section>";
}
void DocbookGenerator::addIndexItem(const char *prim,const char *sec)
{
DB_GEN_C
  addIndexTerm(t, prim, sec);
}

void DocbookGenerator::startDescTable(const char *title)
{
DB_GEN_C
  int ncols = 2;
  t << "<informaltable frame=\"all\">" << endl;
  if (title)t << "<title>" << convertToDocBook(title) << "</title>" << endl;
  t << "    <tgroup cols=\"" << ncols << "\" align=\"left\" colsep=\"1\" rowsep=\"1\">" << endl;
  for (int i = 0; i < ncols; i++)
  {
    t << "      <colspec colname='c" << i+1 << "'/>\n";
  }
  t << "<tbody>\n";
  m_descTable = TRUE;
}

void DocbookGenerator::endDescTable()
{
DB_GEN_C
  t << "    </tbody>" << endl;
  t << "    </tgroup>" << endl;
  t << "</informaltable>" << endl;
  m_descTable = FALSE;
}

void DocbookGenerator::startDescTableRow()
{
DB_GEN_C
  t << "<row>";
  t << "<entry>";
}

void DocbookGenerator::endDescTableRow()
{
DB_GEN_C
  t << "</row>";
}

void DocbookGenerator::startDescTableTitle()
{
DB_GEN_C
}

void DocbookGenerator::endDescTableTitle()
{
DB_GEN_C
}

void DocbookGenerator::startDescTableData()
{
DB_GEN_C
  t << "</entry><entry>";
}

void DocbookGenerator::endDescTableData()
{
DB_GEN_C
  t << "</entry>";
}
void DocbookGenerator::startGroupCollaboration()
{
DB_GEN_C
}
void DocbookGenerator::endGroupCollaboration(DotGroupCollaboration &g)
{
DB_GEN_C
  g.writeGraph(t,GOF_BITMAP,EOF_DocBook,Config_getString(DOCBOOK_OUTPUT),fileName,relPath,FALSE);
}
void DocbookGenerator::startDotGraph()
{
DB_GEN_C
}
void DocbookGenerator::endDotGraph(DotClassGraph &g)
{
DB_GEN_C
  g.writeGraph(t,GOF_BITMAP,EOF_DocBook,Config_getString(DOCBOOK_OUTPUT),fileName,relPath,TRUE,FALSE);
}
void DocbookGenerator::startInclDepGraph()
{
DB_GEN_C
}
void DocbookGenerator::endInclDepGraph(DotInclDepGraph &g)
{
DB_GEN_C
  QCString fn = g.writeGraph(t,GOF_BITMAP,EOF_DocBook,Config_getString(DOCBOOK_OUTPUT), fileName,relPath,FALSE);
}
void DocbookGenerator::startCallGraph()
{
DB_GEN_C
}
void DocbookGenerator::endCallGraph(DotCallGraph &g)
{
DB_GEN_C
  QCString fn = g.writeGraph(t,GOF_BITMAP,EOF_DocBook,Config_getString(DOCBOOK_OUTPUT), fileName,relPath,FALSE);
}
void DocbookGenerator::startDirDepGraph()
{
DB_GEN_C
}
void DocbookGenerator::endDirDepGraph(DotDirDeps &g)
{
DB_GEN_C
  QCString fn = g.writeGraph(t,GOF_BITMAP,EOF_DocBook,Config_getString(DOCBOOK_OUTPUT), fileName,relPath,FALSE);
}
void DocbookGenerator::startMemberDocList()
{
DB_GEN_C
}
void DocbookGenerator::endMemberDocList()
{
DB_GEN_C
  m_inGroup = TRUE;
}
void DocbookGenerator::startConstraintList(const char *header)
{
DB_GEN_C
  t << "<simplesect><title>";
  docify(header);
  t << "</title>" << endl;
}
void DocbookGenerator::startConstraintParam()
{
DB_GEN_C
  t << "<para><emphasis role=\"strong\">";
}
void DocbookGenerator::endConstraintParam()
{
DB_GEN_C
}
void DocbookGenerator::startConstraintType()
{
DB_GEN_C
  t << ":";
}
void DocbookGenerator::endConstraintType()
{
DB_GEN_C
  t << "</emphasis></para>" << endl;
}
void DocbookGenerator::startConstraintDocs()
{
DB_GEN_C
}
void DocbookGenerator::endConstraintDocs()
{
DB_GEN_C
}
void DocbookGenerator::endConstraintList()
{
DB_GEN_C
  t << "</simplesect>" << endl;
}
