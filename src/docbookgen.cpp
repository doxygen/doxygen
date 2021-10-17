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
#include "dir.h"

// no debug info
#define Docbook_DB(x) do {} while(0)
// debug to stdout
//#define Docbook_DB(x) printf x
// debug inside output
//#define Docbook_DB(x) QCString __t;__t.sprintf x;m_t << __t

#if 0
#define DB_GEN_C DB_GEN_C1(m_t)
#define DB_GEN_C1(x) x << "<!-- DB_GEN_C " << __LINE__ << " -->\n";
#define DB_GEN_C2(y) DB_GEN_C2a(m_t,y)
#define DB_GEN_C2a(x,y) x << "<!-- DB_GEN_C " << __LINE__ << " " << y << " -->\n";
#else
#define DB_GEN_C
#define DB_GEN_C1(x)
#define DB_GEN_C2(y)
#define DB_GEN_C2a(x,y)
#endif

//------------------

inline void writeDocbookString(TextStream &t,const QCString &s)
{
  t << convertToDocBook(s);
}

inline void writeDocbookCodeString(TextStream &t,const QCString &str, int &col)
{
  if (str.isEmpty()) return;
  const char *s = str.data();
  char c;
  while ((c=*s++))
  {
    switch(c)
    {
      case '\t':
        {
          int tabSize = Config_getInt(TAB_SIZE);
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
      default:
        {
          uchar uc = static_cast<uchar>(c);
          static const char *hex="0123456789ABCDEF";
          if (uc<32)
          {
            t << "&#x24" << hex[uc>>4] << hex[uc&0xF] << ";";
          }
          else
          {
            t << c;
          }
          col++;
        }
        break;
    }
  }
}

static void addIndexTerm(TextStream &t, QCString prim, QCString sec = "")
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
  t << "</indexterm>\n";
}
void writeDocbookLink(TextStream &t,const QCString & /*extRef*/,const QCString &compoundId,
    const QCString &anchorId,const QCString & text,const QCString & /*tooltip*/)
{
  t << "<link linkend=\"_" << stripPath(compoundId);
  if (!anchorId.isEmpty()) t << "_1" << anchorId;
  t << "\"";
  t << ">";
  writeDocbookString(t,text);
  t << "</link>";
}

DocbookCodeGenerator::DocbookCodeGenerator(TextStream &t) : m_t(t)
{
}

DocbookCodeGenerator::~DocbookCodeGenerator() {}

void DocbookCodeGenerator::codify(const QCString &text)
{
  Docbook_DB(("(codify \"%s\")\n",text));
  writeDocbookCodeString(m_t,text,m_col);
}

void DocbookCodeGenerator::writeCodeLink(CodeSymbolType,
    const QCString &ref,const QCString &file,
    const QCString &anchor,const QCString &name,
    const QCString &tooltip)
{
  Docbook_DB(("(writeCodeLink)\n"));
  writeDocbookLink(m_t,ref,file,anchor,name,tooltip);
  m_col+=name.length();
}

void DocbookCodeGenerator::writeCodeLinkLine(CodeSymbolType,
    const QCString &,const QCString &file,
    const QCString &,const QCString &name,
    const QCString &,bool writeLineAnchor)
{
  Docbook_DB(("(writeCodeLinkLine)\n"));
  if (!writeLineAnchor) return;
  m_t << "<anchor xml:id=\"_" << stripExtensionGeneral(stripPath(file),".xml");
  m_t << "_1l";
  writeDocbookString(m_t,name);
  m_t << "\"/>";
  m_col+=name.length();
}

void DocbookCodeGenerator::writeTooltip(const QCString &, const DocLinkInfo &, const QCString &,
                  const QCString &, const SourceLinkInfo &, const SourceLinkInfo &
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
  if (m_insideCodeLine) m_t << "\n";
  Docbook_DB(("(endCodeLine)\n"));
  m_lineNumber = -1;
  m_refId.resize(0);
  m_external.resize(0);
  m_insideCodeLine=FALSE;
}

void DocbookCodeGenerator::startFontClass(const QCString &colorClass)
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

void DocbookCodeGenerator::writeCodeAnchor(const QCString &)
{
  Docbook_DB(("(writeCodeAnchor)\n"));
}

void DocbookCodeGenerator::writeLineNumber(const QCString &ref,const QCString &fileName,
    const QCString &anchor,int l,bool writeLineAnchor)
{
  Docbook_DB(("(writeLineNumber)\n"));
  m_insideCodeLine = TRUE;
  if (Config_getBool(SOURCE_BROWSER))
  {
    QCString lineNumber;
    lineNumber.sprintf("%05d",l);

    if (!m_sourceFileName.isEmpty())
    {
      writeCodeLinkLine(CodeSymbolType::Default,ref,m_sourceFileName,anchor,lineNumber,QCString(),writeLineAnchor);
    }
    if (!fileName.isEmpty())
    {
      writeCodeLink(CodeSymbolType::Default,ref,fileName,anchor,lineNumber,QCString());
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

void DocbookCodeGenerator::setCurrentDoc(const Definition *,const QCString &,bool)
{
}

void DocbookCodeGenerator::addWord(const QCString &,bool)
{
}

void DocbookCodeGenerator::finish()
{
  endCodeLine();
}

void DocbookCodeGenerator::startCodeFragment(const QCString &)
{
DB_GEN_C1(m_t)
  m_t << "<programlisting linenumbering=\"unnumbered\">";
}

void DocbookCodeGenerator::endCodeFragment(const QCString &)
{
DB_GEN_C1(m_t)
  //endCodeLine checks is there is still an open code line, if so closes it.
  endCodeLine();

  m_t << "</programlisting>";
}

//-------------------------------------------------------------------------------

DocbookGenerator::DocbookGenerator() : OutputGenerator(Config_getString(DOCBOOK_OUTPUT)), m_codeGen(m_t)
{
DB_GEN_C
}

DocbookGenerator::DocbookGenerator(const DocbookGenerator &og) : OutputGenerator(og), m_codeGen(m_t)
{
}

DocbookGenerator &DocbookGenerator::operator=(const DocbookGenerator &og)
{
  OutputGenerator::operator=(og);
  return *this;
}

std::unique_ptr<OutputGenerator> DocbookGenerator::clone() const
{
  return std::make_unique<DocbookGenerator>(*this);
}

DocbookGenerator::~DocbookGenerator()
{
DB_GEN_C
}

void DocbookGenerator::init()
{
  QCString dir=Config_getString(DOCBOOK_OUTPUT);
  Dir d(dir.str());
  if (!d.exists() && !d.mkdir(dir.str()))
  {
    term("Could not create output directory %s\n",qPrint(dir));
  }

  createSubDirs(d);
}
void DocbookGenerator::cleanup()
{
  QCString dname = Config_getString(DOCBOOK_OUTPUT);
  Dir d(dname.str());
  clearSubDirs(d);
}


void DocbookGenerator::startFile(const QCString &name,const QCString &,const QCString &,int)
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
  m_codeGen.setRelativePath(relPath);
  m_codeGen.setSourceFileName(stripPath(fileName));

  m_t << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n";;
  m_t << "<" << fileType << " xmlns=\"http://docbook.org/ns/docbook\" version=\"5.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"";
  if (!pageName.isEmpty()) m_t << " xml:id=\"_" <<  stripPath(pageName) << "\"";
  m_t << " xml:lang=\"" << theTranslator->trISOLang() << "\"";
  m_t << ">\n";
}

void DocbookGenerator::endFile()
{
DB_GEN_C
  closeAllSections();
  m_inLevel = -1;
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
  m_t << "</" << fileType << ">\n";
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
        m_t << "    <info>\n";
        m_t << "    <title>" << convertToDocBook(dbk_projectName) << "</title>\n";
        m_t << "    </info>\n";
      }
      break;
    case isTitlePageAuthor:
      break;
    case isMainPage:
      m_t << "<chapter>\n";
      m_t << "    <title>";
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
    case isConceptIndex:
      //Concept Index}\n"
      break;
    case isClassHierarchyIndex:
      //Hierarchical Index}\n"
      break;
    case isCompoundIndex:
      //m_t << "{"; //Class Index}\n"
      break;
    case isFileIndex:
      //Annotated File Index}\n"
      break;
    case isPageIndex:
      //Annotated Page Index}\n"
      break;
    case isModuleDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case isDirDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case isNamespaceDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case isConceptDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case isClassDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case isFileDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case isExampleDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
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
  switch (is)
  {
    case isTitlePageStart:
      break;
    case isTitlePageAuthor:
      break;
    case isMainPage:
      m_t << "</title>\n";
      m_t << "    <xi:include href=\"mainpage.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
      m_t << "</chapter>\n";
      break;
    case isModuleIndex:
      //m_t << "</chapter>\n";
      break;
    case isDirIndex:
      //m_t << "<xi:include href=\"dirs.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case isNamespaceIndex:
      //m_t << "<xi:include href=\"namespaces.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case isConceptIndex:
      //m_t << "<xi:include href=\"concepts.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case isClassHierarchyIndex:
      //m_t << "<xi:include href=\"hierarchy.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case isCompoundIndex:
      //m_t << "</chapter>\n";
      break;
    case isFileIndex:
      //m_t << "<xi:include href=\"files.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case isPageIndex:
      //m_t << "<xi:include href=\"pages.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case isModuleDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &gd : *Doxygen::groupLinkedMap)
        {
          if (!gd->isReference())
          {
            m_t << "    <xi:include href=\"" << gd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
          }
        }
      }
      m_t << "</chapter>\n";
      break;
    case isDirDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &dd : *Doxygen::dirLinkedMap)
        {
          if (dd->isLinkableInProject())
          {
            m_t << "<    xi:include href=\"" << dd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
          }
        }
      }
      m_t << "</chapter>\n";
      break;
    case isNamespaceDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &nd : *Doxygen::namespaceLinkedMap)
        {
          if (nd->isLinkableInProject() && !nd->isAlias())
          {
            m_t << "<xi:include href=\"" << nd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
          }
        }
      }
      m_t << "</chapter>\n";
      break;
    case isConceptDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &cd : *Doxygen::conceptLinkedMap)
        {
          if (cd->isLinkableInProject() && !cd->isAlias())
          {
            m_t << "<xi:include href=\"" << cd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
          }
        }
      }
      m_t << "</chapter>\n";
      break;
    case isClassDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &cd : *Doxygen::classLinkedMap)
        {
          if (cd->isLinkableInProject() &&
              cd->templateMaster()==0 &&
             !cd->isEmbeddedInOuterScope() &&
             !cd->isAlias()
             )
          {
            m_t << "    <xi:include href=\"" << cd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
          }
        }
      }
      m_t << "</chapter>\n";
      break;
    case isFileDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &fn : *Doxygen::inputNameLinkedMap)
        {
          for (const auto &fd : *fn)
          {
            if (fd->isLinkableInProject())
            {
              m_t << "    <xi:include href=\"" << fd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
            }
            if (fd->generateSourceFile())
            {
              m_t << "    <xi:include href=\"" << fd->getSourceFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
            }
          }
        }
      }
      m_t << "</chapter>\n";
      break;
    case isExampleDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &pd : *Doxygen::exampleLinkedMap)
        {
          m_t << "    <xi:include href=\"" << pd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
        }
      }
      m_t << "</chapter>\n";
      break;
    case isPageDocumentation:
      break;
    case isPageDocumentation2:
      break;
    case isEndIndex:
      m_t << "<index/>\n";
      break;
  }
}
void DocbookGenerator::writePageLink(const QCString &name, bool /*first*/)
{
DB_GEN_C
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    if (!pd->getGroupDef() && !pd->isReference() && pd->name() == stripPath(name))
    {
      m_t << "<chapter>\n";
      if (pd->hasTitle())
      {
        m_t << "    <title>" << convertToDocBook(pd->title()) << "</title>\n";
      }
      else
      {
        m_t << "    <title>" << convertToDocBook(pd->name()) << "</title>\n";
      }
      m_t << "    <xi:include href=\"" << pd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
      m_t << "</chapter>\n";
    }
  }
}

void DocbookGenerator::writeDoc(DocNode *n,const Definition *ctx,const MemberDef *,int)
{
DB_GEN_C
  DocbookDocVisitor *visitor =
    new DocbookDocVisitor(m_t,*this,ctx?ctx->getDefFileExtension():QCString());
  n->accept(visitor);
  delete visitor;
}

void DocbookGenerator::startParagraph(const QCString &)
{
DB_GEN_C
  m_t << "<para>\n";
}

void DocbookGenerator::endParagraph()
{
DB_GEN_C
  m_t << "</para>\n";
}
void DocbookGenerator::writeString(const QCString &text)
{
DB_GEN_C
  m_t << text;
}
void DocbookGenerator::startMemberHeader(const QCString &,int)
{
DB_GEN_C
  m_t << "<simplesect>\n";
  m_inSimpleSect[m_levelListItem] = TRUE;
  m_t << "    <title>";
}

void DocbookGenerator::endMemberHeader()
{
DB_GEN_C
  m_t << "    </title>\n";
}
void DocbookGenerator::docify(const QCString &str)
{
DB_GEN_C
  m_t << convertToDocBook(str);
}
void DocbookGenerator::writeObjectLink(const QCString &, const QCString &f,
                                     const QCString &anchor, const QCString &text)
{
DB_GEN_C
  if (!anchor.isEmpty())
  {
    if (!f.isEmpty()) m_t << "<link linkend=\"_" << stripPath(f) << "_1" << anchor << "\">";
    else   m_t << "<link linkend=\"_" << anchor << "\">";
  }
  else
  {
    m_t << "<link linkend=\"_" << stripPath(f) << "\">";
  }
  docify(text);
  m_t << "</link>";
}
void DocbookGenerator::startMemberList()
{
DB_GEN_C
  m_t << "        <itemizedlist>\n";
  m_levelListItem++;
}
void DocbookGenerator::endMemberList()
{
DB_GEN_C
  if (m_inListItem[m_levelListItem]) m_t << "</listitem>\n";
  m_inListItem[m_levelListItem] = FALSE;
  m_t << "        </itemizedlist>\n";
  m_levelListItem = (m_levelListItem> 0 ?  m_levelListItem - 1 : 0);
  if (m_inSimpleSect[m_levelListItem]) m_t << "</simplesect>\n";
  m_inSimpleSect[m_levelListItem] = FALSE;
}
void DocbookGenerator::startMemberItem(const QCString &,int,const QCString &)
{
DB_GEN_C
  if (m_inListItem[m_levelListItem]) m_t << "</listitem>\n";
  m_t << "            <listitem><para>";
  m_inListItem[m_levelListItem] = TRUE;
}
void DocbookGenerator::endMemberItem()
{
DB_GEN_C
  m_t << "</para>\n";
}
void DocbookGenerator::startBold()
{
DB_GEN_C
  m_t << "<emphasis role=\"strong\">";
}
void DocbookGenerator::endBold()
{
DB_GEN_C
  m_t << "</emphasis>";
}
void DocbookGenerator::startGroupHeader(int extraIndentLevel)
{
DB_GEN_C2("m_inLevel " << m_inLevel)
DB_GEN_C2("extraIndentLevel " << extraIndentLevel)
  m_firstMember = TRUE;
  if (m_inSimpleSect[m_levelListItem]) m_t << "</simplesect>\n";
  m_inSimpleSect[m_levelListItem] = FALSE;
  if (m_inLevel != -1) m_inGroup = TRUE;
  if (m_inLevel == extraIndentLevel) closeSection();
  m_inLevel = extraIndentLevel;
  openSection();
  m_t << "<title>";
}
void DocbookGenerator::writeRuler()
{
DB_GEN_C2("m_inLevel " << m_inLevel)
DB_GEN_C2("m_inGroup " << m_inGroup)
  if (m_inGroup) closeSection();
  m_inGroup = FALSE;
}

void DocbookGenerator::endGroupHeader(int)
{
DB_GEN_C
  m_t << "</title>\n";
}

void DocbookGenerator::startParameterList(bool openBracket)
{
DB_GEN_C
  if (openBracket) m_t << "(";
}
void DocbookGenerator::endParameterList()
{
DB_GEN_C
}
void DocbookGenerator::writeNonBreakableSpace(int n)
{
DB_GEN_C
  for (int i=0;i<n;i++) m_t << " ";
}
void DocbookGenerator::lineBreak(const QCString &)
{
DB_GEN_C
  m_t << "\n";
}
void DocbookGenerator::startTypewriter()
{
DB_GEN_C
  if (!m_denseText) m_t << "<computeroutput>";
}
void DocbookGenerator::endTypewriter()
{
DB_GEN_C
  if (!m_denseText) m_t << "</computeroutput>\n";
}
void DocbookGenerator::startTextBlock(bool dense)
{
DB_GEN_C
  if (dense)
  {
    m_denseText = TRUE;
    m_t << "<programlisting linenumbering=\"unnumbered\">";
  }
}
void DocbookGenerator::endTextBlock(bool)
{
DB_GEN_C
  if (m_denseText)
  {
    m_denseText = FALSE;
    m_t << "</programlisting>";
  }
}
void DocbookGenerator::startMemberDoc(const QCString &clname, const QCString &memname, const QCString &, const QCString &title,
                                      int memCount, int memTotal, bool)
{
DB_GEN_C2("m_inLevel " << m_inLevel)
  openSection();
  m_t << "    <title>" << convertToDocBook(title);
  if (memTotal>1)
  {
    m_t << "<computeroutput>[" << memCount << "/" << memTotal << "]</computeroutput>";
  }
  m_t << "</title>\n";
  if (!memname.isEmpty() && memname[0]!='@')
  {
    addIndexTerm(m_t,memname,clname);
    addIndexTerm(m_t,clname,memname);
  }
}
void DocbookGenerator::endMemberDoc(bool)
{
DB_GEN_C
  m_t << "</computeroutput></para>";
}
void DocbookGenerator::startTitleHead(const QCString &)
{
DB_GEN_C
  m_t << "<title>";
}
void DocbookGenerator::endTitleHead(const QCString &,const QCString &name)
{
DB_GEN_C
  m_t << "</title>\n";
  if (!name.isEmpty()) addIndexTerm(m_t, name);
}
void DocbookGenerator::startDoxyAnchor(const QCString &fName,const QCString &,
                                 const QCString &anchor,const QCString &,
                                 const QCString &)
{
DB_GEN_C
  if (!m_inListItem[m_levelListItem] && !m_descTable && !m_simpleTable)
  {
    if (!m_firstMember) closeSection();
    m_firstMember = FALSE;
  }
  if (!anchor.isEmpty())
  {
    m_t << "<anchor xml:id=\"_" << stripPath(fName) << "_1" << anchor << "\"/>";
  }
}
void DocbookGenerator::endDoxyAnchor(const QCString &,const QCString &)
{
DB_GEN_C
}
void DocbookGenerator::startMemberDocName(bool)
{
DB_GEN_C
  m_t << "<para><computeroutput>";
}
void DocbookGenerator::endMemberDocName()
{
DB_GEN_C
}
void DocbookGenerator::startMemberGroupHeader(bool)
{
DB_GEN_C
  m_t << "<simplesect><title>";
}
void DocbookGenerator::endMemberGroupHeader()
{
DB_GEN_C
  m_t << "</title>\n";
}
void DocbookGenerator::startMemberGroup()
{
DB_GEN_C
}
void DocbookGenerator::endMemberGroup(bool)
{
DB_GEN_C
  m_t << "</simplesect>\n";
}
void DocbookGenerator::startClassDiagram()
{
DB_GEN_C
  m_t << "<para>";
}

void DocbookGenerator::endClassDiagram(const ClassDiagram &d, const QCString &fileName,const QCString &)
{
DB_GEN_C
  m_t << "    <informalfigure>\n";
  m_t << "        <mediaobject>\n";
  m_t << "            <imageobject>\n";
  m_t << "                <imagedata width=\"50%\" align=\"center\" valign=\"middle\" scalefit=\"0\" fileref=\""
                         << relPath << fileName << ".png\">" << "</imagedata>\n";
  m_t << "            </imageobject>\n";
  d.writeImage(m_t,dir(),relPath,fileName,FALSE);
  m_t << "        </mediaobject>\n";
  m_t << "    </informalfigure>\n";
  m_t << "</para>\n";
}
void  DocbookGenerator::startLabels()
{
DB_GEN_C
}

void  DocbookGenerator::writeLabel(const QCString &l,bool isLast)
{
DB_GEN_C
  m_t << "<computeroutput>[" << l << "]</computeroutput>";
  if (!isLast) m_t << ", ";
}

void  DocbookGenerator::endLabels()
{
DB_GEN_C
}
void DocbookGenerator::startExamples()
{
DB_GEN_C
  m_t << "<simplesect><title>";
  docify(theTranslator->trExamples());
  m_t << "</title>";
}

void DocbookGenerator::endExamples()
{
DB_GEN_C
  m_t << "</simplesect>\n";
}
void DocbookGenerator::startSubsubsection()
{
DB_GEN_C
  m_t << "<simplesect><title>";
}
void DocbookGenerator::endSubsubsection()
{
DB_GEN_C
  m_t << "</title></simplesect>\n";
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
  m_t << "<computeroutput>";
}
void DocbookGenerator::endMemberDocPrefixItem()
{
DB_GEN_C
  m_t << "</computeroutput>";
}
void DocbookGenerator::exceptionEntry(const QCString &prefix,bool closeBracket)
{
DB_GEN_C
  if (!prefix.isEmpty())
  {
    m_t << " " << prefix << "(";
  }
  else if (closeBracket)
  {
    m_t << ")";
  }
  m_t << " ";
}
void DocbookGenerator::startParameterName(bool)
{
DB_GEN_C
  m_t << " ";
}
void DocbookGenerator::endParameterName(bool last,bool /*emptyList*/,bool closeBracket)
{
DB_GEN_C
  if (last)
  {
    if (closeBracket) m_t << ")";
  }
}
void DocbookGenerator::startMemberTemplateParams()
{
DB_GEN_C
}

void DocbookGenerator::endMemberTemplateParams(const QCString &,const QCString &)
{
DB_GEN_C
  m_t << "</para>";
  m_t << "<para>";
}
void DocbookGenerator::startSection(const QCString &lab,const QCString &,SectionType)
{
DB_GEN_C
  openSection("xml:id=\"_" + stripPath(lab) + "\"");
  m_t << "<title>";
}
void DocbookGenerator::endSection(const QCString &,SectionType)
{
DB_GEN_C
  m_t << "</title>";
  closeSection();
}
void DocbookGenerator::addIndexItem(const QCString &prim,const QCString &sec)
{
DB_GEN_C
  addIndexTerm(m_t, prim, sec);
}

void DocbookGenerator::startMemberDocSimple(bool isEnum)
{
DB_GEN_C
  int ncols;
  QCString title;
  if (isEnum)
  {
    ncols = 2;
    title = theTranslator->trEnumerationValues();
  }
  else
  {
    ncols = 3;
    title = theTranslator->trCompoundMembers();
  }
  m_t << "<table frame=\"all\">\n";
  if (!title.isEmpty()) m_t << "<title>" << convertToDocBook(title) << "</title>\n";
  m_t << "    <tgroup cols=\"" << ncols << "\" align=\"left\" colsep=\"1\" rowsep=\"1\">\n";
  for (int i = 0; i < ncols; i++)
  {
    m_t << "      <colspec colname='c" << i+1 << "'/>\n";
  }
  m_t << "<tbody>\n";
  m_simpleTable = true;
}

void DocbookGenerator::endMemberDocSimple(bool isEnum)
{
DB_GEN_C
  m_t << "    </tbody>\n";
  m_t << "    </tgroup>\n";
  m_t << "</table>\n";
  m_simpleTable = false;
}

void DocbookGenerator::startInlineMemberType()
{
DB_GEN_C
  m_t << "<row><entry>";
}

void DocbookGenerator::endInlineMemberType()
{
DB_GEN_C
  m_t << "</entry>";
}

void DocbookGenerator::startInlineMemberName()
{
DB_GEN_C
  m_t << "<entry>";
}

void DocbookGenerator::endInlineMemberName()
{
DB_GEN_C
  m_t << "</entry>";
}

void DocbookGenerator::startInlineMemberDoc()
{
DB_GEN_C
  m_t << "<entry>";
}

void DocbookGenerator::endInlineMemberDoc()
{
DB_GEN_C
  m_t << "</entry></row>\n";
}

void DocbookGenerator::startDescTable(const QCString &title)
{
DB_GEN_C
  int ncols = 2;
  m_t << "<informaltable frame=\"all\">\n";
  if (!title.isEmpty()) m_t << "<title>" << convertToDocBook(title) << "</title>\n";
  m_t << "    <tgroup cols=\"" << ncols << "\" align=\"left\" colsep=\"1\" rowsep=\"1\">\n";
  for (int i = 0; i < ncols; i++)
  {
    m_t << "      <colspec colname='c" << i+1 << "'/>\n";
  }
  m_t << "<tbody>\n";
  m_descTable = TRUE;
}

void DocbookGenerator::endDescTable()
{
DB_GEN_C
  m_t << "    </tbody>\n";
  m_t << "    </tgroup>\n";
  m_t << "</informaltable>\n";
  m_descTable = FALSE;
}

void DocbookGenerator::startDescTableRow()
{
DB_GEN_C
  m_t << "<row>";
  m_t << "<entry>";
}

void DocbookGenerator::endDescTableRow()
{
DB_GEN_C
  m_t << "</row>";
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
  m_t << "</entry><entry>";
}

void DocbookGenerator::endDescTableData()
{
DB_GEN_C
  m_t << "</entry>";
}
void DocbookGenerator::startGroupCollaboration()
{
DB_GEN_C
}
void DocbookGenerator::endGroupCollaboration(DotGroupCollaboration &g)
{
DB_GEN_C
  g.writeGraph(m_t,GOF_BITMAP,EOF_DocBook,dir(),fileName(),relPath,FALSE);
}
void DocbookGenerator::startDotGraph()
{
DB_GEN_C
}
void DocbookGenerator::endDotGraph(DotClassGraph &g)
{
DB_GEN_C
  g.writeGraph(m_t,GOF_BITMAP,EOF_DocBook,dir(),fileName(),relPath,TRUE,FALSE);
}
void DocbookGenerator::startInclDepGraph()
{
DB_GEN_C
}
void DocbookGenerator::endInclDepGraph(DotInclDepGraph &g)
{
DB_GEN_C
  QCString fn = g.writeGraph(m_t,GOF_BITMAP,EOF_DocBook,dir(),fileName(),relPath,FALSE);
}
void DocbookGenerator::startCallGraph()
{
DB_GEN_C
}
void DocbookGenerator::endCallGraph(DotCallGraph &g)
{
DB_GEN_C
  QCString fn = g.writeGraph(m_t,GOF_BITMAP,EOF_DocBook,dir(),fileName(),relPath,FALSE);
}
void DocbookGenerator::startDirDepGraph()
{
DB_GEN_C
}
void DocbookGenerator::endDirDepGraph(DotDirDeps &g)
{
DB_GEN_C
  QCString fn = g.writeGraph(m_t,GOF_BITMAP,EOF_DocBook,dir(),fileName(),relPath,FALSE);
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
void DocbookGenerator::startConstraintList(const QCString &header)
{
DB_GEN_C
  m_t << "<simplesect><title>";
  docify(header);
  m_t << "</title>\n";
}
void DocbookGenerator::startConstraintParam()
{
DB_GEN_C
  m_t << "<para><emphasis role=\"strong\">";
}
void DocbookGenerator::endConstraintParam()
{
DB_GEN_C
}
void DocbookGenerator::startConstraintType()
{
DB_GEN_C
  m_t << ":";
}
void DocbookGenerator::endConstraintType()
{
DB_GEN_C
  m_t << "</emphasis></para>\n";
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
  m_t << "</simplesect>\n";
}

void DocbookGenerator::openSection(const QCString &attr)
{
   m_t << "<section";
   if (!attr.isEmpty()) m_t << " " << attr;
   m_t << ">\n";
   m_openSectionCount++;
}

void DocbookGenerator::closeSection()
{
   m_t << "</section>\n";
   m_openSectionCount--;
}

void DocbookGenerator::closeAllSections()
{
  while (m_openSectionCount>0)
  {
    closeSection();
  }
}

