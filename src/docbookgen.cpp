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
#include "outputlist.h"
#include "moduledef.h"

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

inline void writeDocbookCodeString(bool hide,TextStream &t,const QCString &str, size_t &col, size_t stripIndentAmount)
{
  if (str.isEmpty()) return;
  const int tabSize = Config_getInt(TAB_SIZE);
  const char *s = str.data();
  char c=0;
  if (hide)
  {
    col = updateColumnCount(s,col);
  }
  else
  {
    while ((c=*s++))
    {
      switch(c)
      {
        case '\t':
          {
            int spacesToNextTabStop = tabSize - (col%tabSize);
            while (spacesToNextTabStop--)
            {
              if (col>=stripIndentAmount) t << "&#32;";
              col++;
            }
            break;
          }
        case ' ':
          if (col>=stripIndentAmount) t << "&#32;";
          col++;
          break;
        case '<':  t << "&lt;"; col++;   break;
        case '>':  t << "&gt;"; col++;   break;
        case '&':  t << "&amp;"; col++;  break;
        case '\'': t << "&apos;"; col++; break;
        case '"':  t << "&quot;"; col++; break;
        default:
                   {
                     uint8_t uc = static_cast<uint8_t>(c);
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

DocbookCodeGenerator::DocbookCodeGenerator(TextStream *t) : m_t(t)
{
}

void DocbookCodeGenerator::codify(const QCString &text)
{
  Docbook_DB(("(codify \"%s\")\n",text));
  writeDocbookCodeString(m_hide,*m_t,text,m_col,static_cast<size_t>(m_stripIndentAmount));
}

void DocbookCodeGenerator::stripCodeComments(bool b)
{
  m_stripCodeComments = b;
}

void DocbookCodeGenerator::startSpecialComment()
{
  m_hide = m_stripCodeComments;
}

void DocbookCodeGenerator::endSpecialComment()
{
  m_hide = false;
}

void DocbookCodeGenerator::setStripIndentAmount(size_t amount)
{
  m_stripIndentAmount = amount;
}

void DocbookCodeGenerator::writeCodeLink(CodeSymbolType,
    const QCString &ref,const QCString &file,
    const QCString &anchor,const QCString &name,
    const QCString &tooltip)
{
  if (m_hide) return;
  Docbook_DB(("(writeCodeLink)\n"));
  writeDocbookLink(*m_t,ref,file,anchor,name,tooltip);
  m_col+=name.length();
}

void DocbookCodeGenerator::writeCodeLinkLine(CodeSymbolType,
    const QCString &,const QCString &file,
    const QCString &,const QCString &name,
    const QCString &,bool writeLineAnchor)
{
  if (m_hide) return;
  Docbook_DB(("(writeCodeLinkLine)\n"));
  if (!writeLineAnchor) return;
  *m_t << "<anchor xml:id=\"_" << stripExtensionGeneral(stripPath(file),".xml");
  *m_t << "_1l";
  writeDocbookString(*m_t,name);
  *m_t << "\"/>";
  m_col+=name.length();
}

void DocbookCodeGenerator::writeTooltip(const QCString &, const DocLinkInfo &, const QCString &,
                  const QCString &, const SourceLinkInfo &, const SourceLinkInfo &
                 )
{
  Docbook_DB(("(writeToolTip)\n"));
}

void DocbookCodeGenerator::startCodeLine(int)
{
  if (m_hide) return;
  Docbook_DB(("(startCodeLine)\n"));
  m_insideCodeLine=TRUE;
  m_col=0;
}

void DocbookCodeGenerator::endCodeLine()
{
  if (m_hide) return;
  if (m_insideCodeLine) *m_t << "\n";
  Docbook_DB(("(endCodeLine)\n"));
  m_lineNumber = -1;
  m_refId.clear();
  m_external.clear();
  m_insideCodeLine=FALSE;
}

void DocbookCodeGenerator::startFontClass(const QCString &colorClass)
{
  if (m_hide) return;
  Docbook_DB(("(startFontClass)\n"));
  *m_t << "<emphasis role=\"" << colorClass << "\">";
  m_insideSpecialHL=TRUE;
}

void DocbookCodeGenerator::endFontClass()
{
  if (m_hide) return;
  Docbook_DB(("(endFontClass)\n"));
  *m_t << "</emphasis>"; // non DocBook
  m_insideSpecialHL=FALSE;
}

void DocbookCodeGenerator::writeCodeAnchor(const QCString &)
{
  Docbook_DB(("(writeCodeAnchor)\n"));
}

void DocbookCodeGenerator::writeLineNumber(const QCString &ref,const QCString &fileName,
    const QCString &anchor,int l,bool writeLineAnchor)
{
  if (m_hide) return;
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
    *m_t << " ";
  }
  else
  {
    *m_t << l << " ";
  }
  m_col=0;
}

void DocbookCodeGenerator::finish()
{
  endCodeLine();
}

void DocbookCodeGenerator::startCodeFragment(const QCString &)
{
DB_GEN_C1(*m_t)
  *m_t << "<programlisting linenumbering=\"unnumbered\">";
}

void DocbookCodeGenerator::endCodeFragment(const QCString &)
{
DB_GEN_C1(*m_t)
  bool wasHidden = m_hide;
  m_hide = false;
  //endCodeLine checks is there is still an open code line, if so closes it.
  endCodeLine();
  m_hide = wasHidden;

  *m_t << "</programlisting>";
}

//-------------------------------------------------------------------------------

DocbookGenerator::DocbookGenerator()
  : OutputGenerator(Config_getString(DOCBOOK_OUTPUT))
  , m_codeList(std::make_unique<OutputCodeList>())
{
DB_GEN_C
  m_codeGen = m_codeList->add<DocbookCodeGenerator>(&m_t);
}

DocbookGenerator::DocbookGenerator(const DocbookGenerator &og) : OutputGenerator(og.m_dir), OutputGenIntf()
{
  m_codeList         = std::make_unique<OutputCodeList>(*og.m_codeList);
  m_codeGen          = m_codeList->get<DocbookCodeGenerator>(OutputType::Docbook);
  m_codeGen->setTextStream(&m_t);
  m_denseText        = og.m_denseText;
  m_inGroup          = og.m_inGroup;
  m_levelListItem    = og.m_levelListItem;
  m_inListItem       = og.m_inListItem;
  m_inSimpleSect     = og.m_inSimpleSect;
  m_descTable        = og.m_descTable;
  m_simpleTable      = og.m_simpleTable;
  m_inLevel          = og.m_inLevel;
  m_firstMember      = og.m_firstMember;
  m_openSectionCount = og.m_openSectionCount;
}

DocbookGenerator &DocbookGenerator::operator=(const DocbookGenerator &og)
{
  if (this!=&og)
  {
    m_dir              = og.m_dir;
    m_codeList         = std::make_unique<OutputCodeList>(*og.m_codeList);
    m_codeGen          = m_codeList->get<DocbookCodeGenerator>(OutputType::Docbook);
    m_codeGen->setTextStream(&m_t);
    m_denseText        = og.m_denseText;
    m_inGroup          = og.m_inGroup;
    m_levelListItem    = og.m_levelListItem;
    m_inListItem       = og.m_inListItem;
    m_inSimpleSect     = og.m_inSimpleSect;
    m_descTable        = og.m_descTable;
    m_simpleTable      = og.m_simpleTable;
    m_inLevel          = og.m_inLevel;
    m_firstMember      = og.m_firstMember;
    m_openSectionCount = og.m_openSectionCount;
  }
  return *this;
}

DocbookGenerator::~DocbookGenerator() = default;

void DocbookGenerator::addCodeGen(OutputCodeList &list)
{
  list.add<DocbookCodeGeneratorDefer>(m_codeGen);
}

void DocbookGenerator::init()
{
  QCString dir=Config_getString(DOCBOOK_OUTPUT);
  Dir d(dir.str());
  if (!d.exists() && !d.mkdir(dir.str()))
  {
    term("Could not create output directory {}\n",dir);
  }

  createSubDirs(d);
}
void DocbookGenerator::cleanup()
{
  QCString dname = Config_getString(DOCBOOK_OUTPUT);
  Dir d(dname.str());
  clearSubDirs(d);
}


void DocbookGenerator::startFile(const QCString &name,bool,const QCString &,const QCString &,int,int)
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
  if (!fileName.endsWith(".xml")) fileName+=".xml";
  startPlainFile(fileName);
  m_codeGen->setRelativePath(relPath);
  m_codeGen->setSourceFileName(stripPath(fileName));
  m_pageLinks = QCString();

  m_t << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n";
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

  // Write page links only after all sections have been closed to avoid bugs
  m_t << m_pageLinks;

  QCString fileType="section";
  QCString fileName= m_codeGen->sourceFileName();
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
  m_codeGen->setSourceFileName("");
}

void DocbookGenerator::startIndexSection(IndexSection is)
{
DB_GEN_C2("IndexSection " << is)
  switch (is)
  {
    case IndexSection::isTitlePageStart:
      {
        QCString dbk_projectName = Config_getString(PROJECT_NAME);
        m_t << "    <info>\n";
        m_t << "    <title>" << convertToDocBook(dbk_projectName) << "</title>\n";
        m_t << "    </info>\n";
      }
      break;
    case IndexSection::isTitlePageAuthor:
      break;
    case IndexSection::isMainPage:
      break;
    case IndexSection::isModuleIndex:
      //Module Index\n"
      break;
    case IndexSection::isTopicIndex:
      //Module Index\n"
      break;
    case IndexSection::isDirIndex:
      //Directory Index\n"
      break;
    case IndexSection::isNamespaceIndex:
      //Namespace Index\n"
      break;
    case IndexSection::isConceptIndex:
      //Concept Index\n"
      break;
    case IndexSection::isClassHierarchyIndex:
      //Hierarchical Index\n"
      break;
    case IndexSection::isCompoundIndex:
      //m_t << "{"; //Class Index}\n"
      break;
    case IndexSection::isFileIndex:
      //Annotated File Index\n"
      break;
    case IndexSection::isPageIndex:
      //Annotated Page Index\n"
      break;
    case IndexSection::isModuleDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case IndexSection::isTopicDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case IndexSection::isDirDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case IndexSection::isNamespaceDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case IndexSection::isConceptDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case IndexSection::isClassDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case IndexSection::isFileDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case IndexSection::isExampleDocumentation:
      m_t << "<chapter>\n";
      m_t << "    <title>";
      break;
    case IndexSection::isPageDocumentation:
      break;
    case IndexSection::isPageDocumentation2:
      break;
    case IndexSection::isEndIndex:
      break;
  }
}

void DocbookGenerator::endIndexSection(IndexSection is)
{
DB_GEN_C2("IndexSection " << is)
  switch (is)
  {
    case IndexSection::isTitlePageStart:
      break;
    case IndexSection::isTitlePageAuthor:
      break;
    case IndexSection::isMainPage:
      {
        if (Doxygen::mainPage)
        {
          writePageLink(QCString("mainpage"), TRUE);
        }
      }
      break;
    case IndexSection::isModuleIndex:
      //m_t << "</chapter>\n";
      break;
    case IndexSection::isTopicIndex:
      //m_t << "</chapter>\n";
      break;
    case IndexSection::isDirIndex:
      //m_t << "<xi:include href=\"dirs.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case IndexSection::isNamespaceIndex:
      //m_t << "<xi:include href=\"namespaces.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case IndexSection::isConceptIndex:
      //m_t << "<xi:include href=\"concepts.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case IndexSection::isClassHierarchyIndex:
      //m_t << "<xi:include href=\"hierarchy.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case IndexSection::isCompoundIndex:
      //m_t << "</chapter>\n";
      break;
    case IndexSection::isFileIndex:
      //m_t << "<xi:include href=\"files.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case IndexSection::isPageIndex:
      //m_t << "<xi:include href=\"pages.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>";
      //m_t << "</chapter>\n";
      break;
    case IndexSection::isTopicDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &gd : *Doxygen::groupLinkedMap)
        {
          if (!gd->isReference() && !gd->isASubGroup())
          {
            writePageLink(gd->getOutputFileBase(), TRUE);
          }
        }
      }
      m_t << "</chapter>\n";
      break;
    case IndexSection::isModuleDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &mod : ModuleManager::instance().modules())
        {
          if (!mod->isReference() && mod->isPrimaryInterface())
          {
            writePageLink(mod->getOutputFileBase(), TRUE);
          }
        }
      }
      m_t << "</chapter>\n";
      break;
      break;
    case IndexSection::isDirDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &dd : *Doxygen::dirLinkedMap)
        {
          if (dd->isLinkableInProject())
          {
            m_t << "    <xi:include href=\"" << dd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
          }
        }
      }
      m_t << "</chapter>\n";
      break;
    case IndexSection::isNamespaceDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &nd : *Doxygen::namespaceLinkedMap)
        {
          if (nd->isLinkableInProject() && !nd->isAlias())
          {
            m_t << "    <xi:include href=\"" << nd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
          }
        }
      }
      m_t << "</chapter>\n";
      break;
    case IndexSection::isConceptDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &cd : *Doxygen::conceptLinkedMap)
        {
          if (cd->isLinkableInProject() && !cd->isAlias())
          {
            m_t << "    <xi:include href=\"" << cd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
          }
        }
      }
      m_t << "</chapter>\n";
      break;
    case IndexSection::isClassDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &cd : *Doxygen::classLinkedMap)
        {
          if (cd->isLinkableInProject() &&
             !cd->isImplicitTemplateInstance() &&
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
    case IndexSection::isFileDocumentation:
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
    case IndexSection::isExampleDocumentation:
      {
        m_t << "</title>\n";
        for (const auto &pd : *Doxygen::exampleLinkedMap)
        {
          m_t << "    <xi:include href=\"" << pd->getOutputFileBase() << ".xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n";
        }
      }
      m_t << "</chapter>\n";
      break;
    case IndexSection::isPageDocumentation:
        for (const auto &pd : *Doxygen::pageLinkedMap)
        {
          if (!pd->getGroupDef() && !pd->isReference() && !pd->hasParentPage()
            && Doxygen::mainPage.get() != pd.get())
          {
            writePageLink(pd->getOutputFileBase(), TRUE);
          }
        }
      break;
    case IndexSection::isPageDocumentation2:
      break;
    case IndexSection::isEndIndex:
      m_t << "<index/>\n";
      break;
  }
}
void DocbookGenerator::writePageLink(const QCString &name, bool first)
{
DB_GEN_C
  QCString link;
  link.sprintf("    <xi:include href=\"%s.xml\" xmlns:xi=\"http://www.w3.org/2001/XInclude\"/>\n",
               name.data());
  if (first)
    m_t << link;
  else
    // Buffer page links and write them after all sections are closed
    m_pageLinks += link;
}

void DocbookGenerator::writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *,int,int sectionLevel)
{
DB_GEN_C
  auto astImpl = dynamic_cast<const DocNodeAST*>(ast);
  if (astImpl && sectionLevel<=m_tocState.maxLevel)
  {
    DocbookDocVisitor visitor(m_t,*m_codeList,ctx?ctx->getDefFileExtension():QCString());
    std::visit(visitor,astImpl->root);
  }
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
static QCString objectLinkToString(const QCString &, const QCString &f,
                                   const QCString &anchor, const QCString &text)
{
DB_GEN_C
  QCString result;
  if (!anchor.isEmpty())
  {
    if (!f.isEmpty()) result += "<link linkend=\"_" + stripPath(f) + "_1" + anchor + "\">";
    else   result += "<link linkend=\"_" + anchor + "\">";
  }
  else
  {
    result += "<link linkend=\"_" + stripPath(f) + "\">";
  }
  result += convertToDocBook(text);
  result += "</link>";
  return result;
}
void DocbookGenerator::writeObjectLink(const QCString &ref, const QCString &f,
                                     const QCString &anchor, const QCString &text)
{
DB_GEN_C
  m_t << objectLinkToString(ref,f,anchor,text);
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
void DocbookGenerator::startMemberItem(const QCString &,MemberItemType,const QCString &)
{
DB_GEN_C
  if (m_inListItem[m_levelListItem]) m_t << "</listitem>\n";
  m_t << "            <listitem><para>";
  m_inListItem[m_levelListItem] = TRUE;
}
void DocbookGenerator::endMemberItem(MemberItemType)
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
void DocbookGenerator::startGroupHeader(const QCString &,int extraIndentLevel)
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
  for (int i=0;i<n;i++) m_t << "&#160;";
}
void DocbookGenerator::lineBreak(const QCString &)
{
DB_GEN_C
  m_t << "<?linebreak?>";
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
void DocbookGenerator::addLabel(const QCString &,const QCString &)
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
void DocbookGenerator::startMemberGroupHeader(const QCString &,bool)
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
void DocbookGenerator::startCompoundTemplateParams()
{
DB_GEN_C
  m_t << "<simplesect><title>";
}
void DocbookGenerator::endCompoundTemplateParams()
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

void DocbookGenerator::endParameterName()
{
DB_GEN_C
}

void DocbookGenerator::startParameterExtra()
{
DB_GEN_C
}

void DocbookGenerator::endParameterExtra(bool last,bool /*emptyList*/,bool closeBracket)
{
DB_GEN_C
  if (last && closeBracket)
  {
    m_t << ")";
  }
}


void DocbookGenerator::startParameterDefVal(const char *sep)
{
DB_GEN_C
  m_t << sep;
  if (!m_denseText) m_t << "<computeroutput>";
}

void DocbookGenerator::endParameterDefVal()
{
DB_GEN_C
  if (!m_denseText) m_t << "</computeroutput>\n";
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
  int ncols=0;
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

void DocbookGenerator::endMemberDocSimple(bool /* isEnum */)
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

void DocbookGenerator::startDescTable(const QCString &title,const bool hasInits)
{
DB_GEN_C
  int ncols = (hasInits?3:2);
  m_t << "<informaltable frame=\"all\">\n";
  if (!title.isEmpty()) m_t << "<title>" << convertToDocBook(title) << "</title>\n";
  m_t << "    <tgroup cols=\"" << ncols << "\" align=\"left\" colsep=\"1\" rowsep=\"1\">\n";
  int i = 1;
  m_t << "      <colspec colname='c" << i++ << "'/>\n";
  if (hasInits) m_t << "      <colspec colname='c" << i++ << "' align='right'/>\n";
  m_t << "      <colspec colname='c" << i++ << "'/>\n";
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

void DocbookGenerator::startDescTableInit()
{
DB_GEN_C
  m_t << "</entry><entry>";
}

void DocbookGenerator::endDescTableInit()
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
  g.writeGraph(m_t,GraphOutputFormat::BITMAP,EmbeddedOutputFormat::DocBook,dir(),fileName(),relPath,FALSE);
}
void DocbookGenerator::startDotGraph()
{
DB_GEN_C
}
void DocbookGenerator::endDotGraph(DotClassGraph &g)
{
DB_GEN_C
  g.writeGraph(m_t,GraphOutputFormat::BITMAP,EmbeddedOutputFormat::DocBook,dir(),fileName(),relPath,TRUE,FALSE);
}
void DocbookGenerator::startInclDepGraph()
{
DB_GEN_C
}
void DocbookGenerator::endInclDepGraph(DotInclDepGraph &g)
{
DB_GEN_C
  QCString fn = g.writeGraph(m_t,GraphOutputFormat::BITMAP,EmbeddedOutputFormat::DocBook,dir(),fileName(),relPath,FALSE);
}
void DocbookGenerator::startCallGraph()
{
DB_GEN_C
}
void DocbookGenerator::endCallGraph(DotCallGraph &g)
{
DB_GEN_C
  QCString fn = g.writeGraph(m_t,GraphOutputFormat::BITMAP,EmbeddedOutputFormat::DocBook,dir(),fileName(),relPath,FALSE);
}
void DocbookGenerator::startDirDepGraph()
{
DB_GEN_C
}
void DocbookGenerator::endDirDepGraph(DotDirDeps &g)
{
DB_GEN_C
  QCString fn = g.writeGraph(m_t,GraphOutputFormat::BITMAP,EmbeddedOutputFormat::DocBook,dir(),fileName(),relPath,FALSE);
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

void DocbookGenerator::writeInheritedSectionTitle(
                  const QCString &/*id*/,const QCString &ref,
                  const QCString &file,  const QCString &anchor,
                  const QCString &title, const QCString &name)
{
DB_GEN_C
  m_t << theTranslator->trInheritedFrom(convertToDocBook(title), objectLinkToString(ref, file, anchor, name));
}

void DocbookGenerator::startLocalToc(int level)
{
  m_tocState.level=1;
  m_tocState.maxLevel=level;
  m_tocState.inLi = BoolVector(level+1,false);
  m_t << "    <toc>\n";
  m_t << "    <title>" << theTranslator->trRTFTableOfContents() << "</title>\n";
}

void DocbookGenerator::endLocalToc()
{
  if (m_tocState.level > m_tocState.maxLevel) m_tocState.level = m_tocState.maxLevel;
  while (m_tocState.level>1 && m_tocState.level <= m_tocState.maxLevel)
  {
    m_t << "</tocdiv>\n";
    m_tocState.level--;
  }
  m_t << "    </toc>\n";
}

void DocbookGenerator::startTocEntry(const SectionInfo *si)
{
  SectionType type = si->type();
  if (type.isSection())
  {
    //printf("  level=%d title=%s\n",level,qPrint(si->title));
    int nextLevel = type.level();
    if (nextLevel>m_tocState.level)
    {
      for (int l=m_tocState.level;l<nextLevel;l++)
      {
        if (l < m_tocState.maxLevel) m_t << "    <tocdiv>\n";
      }
    }
    else if (nextLevel<m_tocState.level)
    {
      for (int l=m_tocState.level;l>nextLevel;l--)
      {
        m_tocState.inLi[l]=false;
        if (l <= m_tocState.maxLevel) m_t << "    </tocdiv>\n";
      }
    }
    if (nextLevel <= m_tocState.maxLevel)
    {
      QCString label = convertToDocBook(si->label());
      m_t << "      <tocentry>";
    }
  }
}

void DocbookGenerator::endTocEntry(const SectionInfo *si)
{
  SectionType type = si->type();
  int nextLevel = type.level();
  if (type.isSection() && nextLevel <= m_tocState.maxLevel)
  {
    m_t << "</tocentry>\n";
    m_tocState.inLi[nextLevel]=true;
    m_tocState.level = nextLevel;
  }
}

//-------------------------------------------------------------------------------------------------

static constexpr auto hex="0123456789ABCDEF";

/*! Converts a string to an DocBook-encoded string */
QCString convertToDocBook(const QCString &s, const bool retainNewline)
{
  if (s.isEmpty()) return s;
  QCString result;
  result.reserve(s.length()+32);
  const char *p = s.data();
  const char *q = nullptr;
  int cnt = 0;
  char c = 0;
  while ((c=*p++))
  {
    switch (c)
    {
      case '\n':
        if (retainNewline) 
        {
          result+="<literallayout>&#160;&#xa;</literallayout>";
          result+=c;
        }
        break;
      case '<':  result+="&lt;";   break;
      case '>':  result+="&gt;";   break;
      case '&':  // possibility to have a special symbol
        q = p;
        cnt = 2; // we have to count & and ; as well
        while ((*q >= 'a' && *q <= 'z') || (*q >= 'A' && *q <= 'Z') || (*q >= '0' && *q <= '9'))
        {
          cnt++;
          q++;
        }
        if (*q == ';')
        {
           --p; // we need & as well
           HtmlEntityMapper::SymType res = HtmlEntityMapper::instance().name2sym(QCString(p).left(cnt));
           if (res == HtmlEntityMapper::Sym_Unknown)
           {
             p++;
             result+="&amp;";
           }
           else
           {
             result+=HtmlEntityMapper::instance().docbook(res);
             q++;
             p = q;
           }
        }
        else
        {
          result+="&amp;";
        }
        break;
      case '\'': result+="&apos;"; break;
      case '"':  result+="&quot;"; break;
      case  1: case  2: case  3: case  4: case  5: case  6: case 7:  case  8:
      case 11: case 12: case 14: case 15: case 16: case 17: case 18:
      case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
      case 27: case 28: case 29: case 30: case 31:
        result+="&#x24";
        result+=hex[static_cast<uint8_t>(c)>>4];
        result+=hex[static_cast<uint8_t>(c)&0xF];
        result+=';';
        break;
      default:
        result+=c;
        break;
    }
  }
  return result;
}

