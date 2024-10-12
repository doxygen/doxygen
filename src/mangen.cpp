/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

/* http://www.cubic.org/source/archive/fileform/txt/man/ has some
   nice introductions to groff and man pages. */

#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "mangen.h"
#include "config.h"
#include "util.h"
#include "doxygen.h"
#include "docparser.h"
#include "mandocvisitor.h"
#include "language.h"
#include "dir.h"
#include "utf8.h"
#include "datetime.h"
#include "portable.h"
#include "outputlist.h"

static QCString getExtension()
{
  /*
   * [.][number][rest]
   * in case of . missing, just ignore it
   * in case number missing, just place a 3 in front of it
   */
  QCString ext = Config_getString(MAN_EXTENSION);
  if (ext.isEmpty())
  {
    ext = "3";
  }
  else
  {
    if (ext.at(0)=='.')
    {
      if (ext.length()==1)
      {
        ext = "3";
      }
      else // strip .
      {
        ext = ext.mid(1);
      }
    }
    if (ext.at(0)<'0' || ext.at(0)>'9')
    {
      ext.prepend("3");
    }
  }
  return ext;
}

static QCString getSubdir()
{
  QCString dir = Config_getString(MAN_SUBDIR);
  if (dir.isEmpty())
  {
    dir = "man" + getExtension();
  }
  return dir;
}

static QCString docifyToString(const QCString &str)
{
  QCString result;
  result.reserve(str.length());
  if (!str.isEmpty())
  {
    const char *p=str.data();
    char c=0;
    while ((c=*p++))
    {
      switch(c)
      {
        case '-':  result += "\\-";  break; // see  bug747780
        case '.':  result += "\\&."; break; // see  bug652277
        case '\\': result += "\\\\"; break;
        case '\n': result += "\n";   break;
        case '\"': c = '\'';   // no break!
        default:   result += c;      break;
      }
    }
    //printf("%s",str);fflush(stdout);
  }
  return result;
}

static QCString objectLinkToString(const QCString &text)
{
  return "\\fB" + docifyToString(text) + "\\fP";
}

//-------------------------------------------------------------------------------

ManCodeGenerator::ManCodeGenerator(TextStream *t) : m_t(t)
{
}

void ManCodeGenerator::startCodeFragment(const QCString &)
{
  *m_t << "\n";
  *m_t << ".nf\n";
}

void ManCodeGenerator::endCodeFragment(const QCString &)
{
  if (m_col>0) *m_t << "\n";
  *m_t << ".PP\n";
  *m_t << ".fi\n";
  m_col=0;
}

void ManCodeGenerator::writeLineNumber(const QCString &,const QCString &,const QCString &,int l, bool)
{
  if (m_hide) return;
  *m_t << l << " ";
  m_col=0;
}

void ManCodeGenerator::writeCodeLink(CodeSymbolType,
                                 const QCString &,const QCString &,
                                 const QCString &, const QCString &name,
                                 const QCString &)
{
  if (m_hide) return;
  if (!name.isEmpty())
  {
    const char *p=name.data();
    char c=0;
    while ((c=*p++))
    {
      switch(c)
      {
        case '-':  *m_t << "\\-"; break; // see  bug747780
        case '.':  *m_t << "\\&."; break; // see  bug652277
        case '\\': *m_t << "\\\\"; m_col++; break;
        case '\n': *m_t << "\n"; m_col=0; break;
        case '\"':  c = '\''; // no break!
        default: *m_t << c; m_col++; break;
      }
    }
    //printf("%s",str);fflush(stdout);
  }
}

void ManCodeGenerator::codify(const QCString &str)
{
  if (m_hide) return;
  //static char spaces[]="        ";
  if (!str.isEmpty())
  {
    const char *p=str.data();
    while (*p)
    {
      char c=*p++;
      switch(c)
      {
        case '-':  *m_t << "\\-"; break; // see  bug747780
        case '.':   *m_t << "\\&."; break; // see  bug652277
        case '\t':  {
                      int spacesToNextTabStop = Config_getInt(TAB_SIZE) - (m_col%Config_getInt(TAB_SIZE));
                      *m_t << Doxygen::spaces.left(spacesToNextTabStop);
                      m_col+=spacesToNextTabStop;
                    }
                    break;
        case '\n':  *m_t << "\n"; m_col=0; break;
        case '\\':  *m_t << "\\\\"; m_col++; break;
        case '\"':  // no break!
        default:    p=writeUTF8Char(*m_t,p-1); m_col++; break;
      }
    }
    //printf("%s",str);fflush(stdout);
  }
}

void ManCodeGenerator::stripCodeComments(bool b)
{
  m_stripCodeComments = b;
}

void ManCodeGenerator::startSpecialComment()
{
  m_hide = m_stripCodeComments;
}

void ManCodeGenerator::endSpecialComment()
{
  m_hide = false;
}


//-------------------------------------------------------------------------------

ManGenerator::ManGenerator()
  : OutputGenerator(Config_getString(MAN_OUTPUT)+"/"+getSubdir())
  , m_codeList(std::make_unique<OutputCodeList>())
{
  m_codeGen = m_codeList->add<ManCodeGenerator>(&m_t);
}

ManGenerator::ManGenerator(const ManGenerator &og) : OutputGenerator(og.m_dir), OutputGenIntf()
{
  m_codeList = std::make_unique<OutputCodeList>(*og.m_codeList);
  m_codeGen      = m_codeList->get<ManCodeGenerator>(OutputType::Man);
  m_codeGen->setTextStream(&m_t);
  m_firstCol      = og.m_firstCol;
  m_col           = og.m_col;
  m_paragraph     = og.m_paragraph;
  m_upperCase     = og.m_upperCase;
  m_insideTabbing = og.m_insideTabbing;
  m_inHeader      = og.m_inHeader;
}

ManGenerator &ManGenerator::operator=(const ManGenerator &og)
{
  if (this!=&og)
  {
    m_dir           = og.m_dir;
    m_codeList = std::make_unique<OutputCodeList>(*og.m_codeList);
    m_codeGen       = m_codeList->get<ManCodeGenerator>(OutputType::Man);
    m_codeGen->setTextStream(&m_t);
    m_firstCol      = og.m_firstCol;
    m_col           = og.m_col;
    m_paragraph     = og.m_paragraph;
    m_upperCase     = og.m_upperCase;
    m_insideTabbing = og.m_insideTabbing;
    m_inHeader      = og.m_inHeader;
  }
  return *this;
}

ManGenerator::~ManGenerator() = default;

void ManGenerator::addCodeGen(OutputCodeList &list)
{
  list.add<ManCodeGeneratorDefer>(m_codeGen);
}

void ManGenerator::init()
{
  QCString manOutput = Config_getString(MAN_OUTPUT);

  Dir d(manOutput.str());
  if (!d.exists() && !d.mkdir(manOutput.str()))
  {
    term("Could not create output directory %s\n",qPrint(manOutput));
  }
  std::string manDir = manOutput.str()+"/"+getSubdir().str();
  if (!d.exists(manDir) && !d.mkdir(manDir))
  {
    term("Could not create output directory %s/%s\n",qPrint(manOutput), qPrint(getSubdir()));
  }
  createSubDirs(d);
}

void ManGenerator::cleanup()
{
  QCString dname = Config_getString(MAN_OUTPUT);
  Dir d(dname.str());
  clearSubDirs(d);
}

static QCString buildFileName(const QCString &name)
{
  QCString fileName;
  if (name.isEmpty()) return "noname";

  const char *p=name.data();
  char c = 0;
  while ((c=*p++))
  {
    switch (c)
    {
      case ':':
        fileName+="_";
        if (*p==':') p++;
        break;
      case '<':
      case '>':
      case '&':
      case '*':
      case '!':
      case '^':
      case '~':
      case '%':
      case '+':
      case '/':
        fileName+="_";
        break;
      default:
        fileName+=c;
    }
  }

  QCString manExtension = "." + getExtension();
  if (fileName.right(manExtension.length())!=manExtension)
  {
    fileName+=manExtension;
  }

  return fileName;
}

void ManGenerator::startFile(const QCString &,const QCString &manName,const QCString &,int,int)
{
  startPlainFile( buildFileName( manName ) );
  m_firstCol=TRUE;
}

void ManGenerator::endFile()
{
  m_t << "\n";
  endPlainFile();
}

void ManGenerator::endTitleHead(const QCString &,const QCString &name)
{
  m_t << ".TH \"" << name << "\" " << getExtension() << " \"";
  switch (Config_getEnum(TIMESTAMP))
  {
    case TIMESTAMP_t::YES:
    case TIMESTAMP_t::DATETIME:
      m_t << dateToString(DateTimeType::DateTime) << "\" \"";
      break;
    case TIMESTAMP_t::DATE:
      m_t << dateToString(DateTimeType::Date) << "\" \"";
      break;
    case TIMESTAMP_t::NO:
      break;
  }
  if (!Config_getString(PROJECT_NUMBER).isEmpty())
    m_t << "Version " << Config_getString(PROJECT_NUMBER) << "\" \"";
  if (Config_getString(PROJECT_NAME).isEmpty())
    m_t << "Doxygen";
  else
    m_t << Config_getString(PROJECT_NAME);
  m_t << "\" \\\" -*- nroff -*-\n";
  m_t << ".ad l\n";
  m_t << ".nh\n";
  m_t << ".SH NAME\n";
  m_t << name;
  m_firstCol=FALSE;
  m_paragraph=TRUE;
  m_inHeader=TRUE;
}

void ManGenerator::newParagraph()
{
  if (!m_paragraph)
  {
    if (!m_firstCol) m_t << "\n";
    m_t << ".PP\n";
    m_firstCol=TRUE;
  }
  m_paragraph=TRUE;
}

void ManGenerator::startParagraph(const QCString &)
{
  if (!m_paragraph)
  {
    if (!m_firstCol) m_t << "\n";
    m_t << ".PP\n";
    m_firstCol=TRUE;
  }
  m_paragraph=TRUE;
}

void ManGenerator::endParagraph()
{
}

void ManGenerator::writeString(const QCString &text)
{
  docify(text);
}

void ManGenerator::startIndexItem(const QCString &,const QCString &)
{
}

void ManGenerator::endIndexItem(const QCString &,const QCString &)
{
}

void ManGenerator::writeStartAnnoItem(const QCString &,const QCString &,
                                       const QCString &,const QCString &)
{
}

void ManGenerator::writeObjectLink(const QCString &,const QCString &,
                                   const QCString &, const QCString &name)
{
  startBold(); docify(name); endBold();
}

void ManGenerator::startGroupHeader(int)
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".SH \"";
  m_upperCase=TRUE;
  m_firstCol=FALSE;
}

void ManGenerator::endGroupHeader(int)
{
  m_t << "\"\n.PP \n";
  m_firstCol=TRUE;
  m_paragraph=TRUE;
  m_upperCase=FALSE;
}

void ManGenerator::startMemberHeader(const QCString &,int)
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".SS \"";
}

void ManGenerator::endMemberHeader()
{
  m_t << "\"\n";
  m_firstCol=TRUE;
  m_paragraph=FALSE;
}

void ManGenerator::docify(const QCString &str)
{
  if (!str.isEmpty())
  {
    const char *p=str.data();
    char c=0;
    while ((c=*p++))
    {
      switch(c)
      {
        case '-':  m_t << "\\-";           break; // see  bug747780
        case '.':  m_t << "\\&.";          break; // see  bug652277
        case '\\': m_t << "\\\\"; m_col++; break;
        case '\n': m_t << "\n";   m_col=0; break;
        case '\"': c = '\'';         // no break!
        default:   m_t << c;      m_col++; break;
      }
    }
    m_firstCol=(c=='\n');
    //printf("%s",str);fflush(stdout);
  }
  m_paragraph=FALSE;
}

void ManGenerator::writeChar(char c)
{
  m_firstCol=(c=='\n');
  if (m_firstCol) m_col=0; else m_col++;
  switch (c)
  {
    case '\\': m_t << "\\\\"; break;
  case '\"': c = '\''; // no break!
    default:   m_t << c; break;
  }
  //printf("%c",c);fflush(stdout);
  m_paragraph=FALSE;
}

void ManGenerator::startTitle()
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".SH \"";
  m_firstCol=FALSE;
  m_paragraph=FALSE;
}

void ManGenerator::endTitle()
{
    m_t << "\"";
}

void ManGenerator::startItemListItem()
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".TP\n";
  m_firstCol=TRUE;
  m_paragraph=FALSE;
  m_col=0;
}

void ManGenerator::endItemListItem()
{
}

void ManGenerator::startMemberDoc(const QCString &,const QCString &,const QCString &,const QCString &,int,int,bool)
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".SS \"";
  m_firstCol=FALSE;
  m_paragraph=FALSE;
}

void ManGenerator::startDoxyAnchor(const QCString &,const QCString &manName,
                                   const QCString &, const QCString &name,
                                   const QCString &)
{
    // something to be done?
    if( !Config_getBool(MAN_LINKS) )
    {
	return; // no
    }

    // the name of the link file is derived from the name of the anchor:
    // - truncate after an (optional) ::
    QCString baseName = name;
    int i=baseName.findRev("::");
    if (i!=-1) baseName=baseName.right(baseName.length()-i-2);

    //printf("Converting man link '%s'->'%s'->'%s'\n",
    //       name,qPrint(baseName),qPrint(buildFileName(baseName)));

    // - remove dangerous characters and append suffix, then add dir prefix
    QCString fileName=dir()+"/"+buildFileName( baseName );
    FileInfo fi(fileName.str());
    if (!fi.exists())
    {
      std::ofstream linkStream = Portable::openOutputStream(fileName);
      if (linkStream.is_open())
      {
        linkStream << ".so " << getSubdir() << "/" << buildFileName( manName ) << "\n";
      }
    }
}

void ManGenerator::addLabel(const QCString &,const QCString &)
{
}

void ManGenerator::endMemberDoc(bool)
{
    m_t << "\"\n";
}

void ManGenerator::startCompoundTemplateParams()
{
  if (!m_firstCol) m_t << "\n";
  m_t << "\n.SS \"";
  m_firstCol=FALSE;
  m_paragraph=FALSE;
}

void ManGenerator::endCompoundTemplateParams()
{
  m_t << "\"";
}
void ManGenerator::writeSynopsis()
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".SH SYNOPSIS\n.br\n.PP\n";
  m_firstCol=TRUE;
  m_paragraph=FALSE;
}

void ManGenerator::startDescForItem()
{
  if (!m_firstCol) m_t << "\n";
  if (!m_paragraph) m_t << ".in -1c\n";
  m_t << ".in +1c\n";
  m_firstCol=TRUE;
  m_paragraph=FALSE;
  m_col=0;
}

void ManGenerator::endDescForItem()
{
}

void ManGenerator::startAnonTypeScope(int indentLevel)
{
  if (indentLevel==0)
  {
    m_insideTabbing=TRUE;
  }
}

void ManGenerator::endAnonTypeScope(int indentLevel)
{
  if (indentLevel==0)
  {
    m_insideTabbing=FALSE;
  }
}


void ManGenerator::startMemberItem(const QCString &,MemberItemType,const QCString &)
{
  if (m_firstCol && !m_insideTabbing) m_t << ".in +1c\n";
  m_t << "\n.ti -1c\n.RI \"";
  m_firstCol=FALSE;
}

void ManGenerator::endMemberItem(MemberItemType)
{
  m_t << "\"\n.br";
}

void ManGenerator::startMemberList()
{
  if (!m_insideTabbing)
  {
    m_t << "\n.in +1c"; m_firstCol=FALSE;
  }
}

void ManGenerator::endMemberList()
{
  if (!m_insideTabbing)
  {
    m_t << "\n.in -1c"; m_firstCol=FALSE;
  }
}

void ManGenerator::startMemberGroupHeader(bool)
{
  m_t << "\n.PP\n.RI \"\\fB";
}

void ManGenerator::endMemberGroupHeader()
{
  m_t << "\\fP\"\n.br\n";
  m_firstCol=TRUE;
}

void ManGenerator::startMemberGroupDocs()
{
}

void ManGenerator::endMemberGroupDocs()
{
  m_t << "\n.PP";
}

void ManGenerator::startMemberGroup()
{
  m_t << "\n.in +1c";
}

void ManGenerator::endMemberGroup(bool)
{
  m_t << "\n.in -1c";
  m_firstCol=FALSE;
}

void ManGenerator::startSection(const QCString &,const QCString &,SectionType type)
{
  if( !m_inHeader )
  {
    switch(type.level())
    {
      case SectionType::Page:             // fall through
      case SectionType::Section:          startGroupHeader(0); break;
      case SectionType::Subsection:       // fall through
      case SectionType::Subsubsection:    // fall through
      case SectionType::Paragraph:        // fall through
      case SectionType::Subparagraph:     // fall through
      case SectionType::Subsubparagraph:  startMemberHeader(QCString(), -1); break;
      default: ASSERT(0); break;
    }
  }
}

void ManGenerator::endSection(const QCString &,SectionType type)
{
  if( !m_inHeader )
  {
    switch(type.level())
    {
      case SectionType::Page:            // fall through
      case SectionType::Section:         endGroupHeader(0); break;
      case SectionType::Subsection:      // fall through
      case SectionType::Subsubsection:   // fall through
      case SectionType::Paragraph:       // fall through
      case SectionType::Subparagraph:    // fall through
      case SectionType::Subsubparagraph: endMemberHeader(); break;
      default: ASSERT(0); break;
    }
  }
  else
  {
    m_t << "\n.PP\n";
    m_firstCol=TRUE;
    m_paragraph=FALSE;
    m_inHeader=FALSE;
  }
}

void ManGenerator::startExamples()
{
  if (!m_firstCol)
  { m_t << "\n" << ".PP\n";
    m_firstCol=TRUE; m_paragraph=TRUE;
    m_col=0;
  }
  m_paragraph=FALSE;
  startBold();
  docify(theTranslator->trExamples());
  endBold();
  m_paragraph=TRUE;
}

void ManGenerator::endExamples()
{
}

void ManGenerator::startDescTable(const QCString &title,const bool hasInits)
{
  if (!m_firstCol)
  { m_t << "\n.PP\n";
    m_firstCol=TRUE; m_paragraph=TRUE;
    m_col=0;
  }
  m_paragraph=FALSE;
  startBold();
  docify(title);
  endBold();
  m_paragraph=TRUE;
  startDescForItem();
}

void ManGenerator::endDescTable()
{
  endDescForItem();
}

void ManGenerator::writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *,int)
{
  const DocNodeAST *astImpl = dynamic_cast<const DocNodeAST *>(ast);
  if (astImpl)
  {
    ManDocVisitor visitor(m_t,*m_codeList,ctx?ctx->getDefFileExtension():QCString(""));
    std::visit(visitor,astImpl->root);
  }
  m_firstCol=FALSE;
  m_paragraph = FALSE;
}

void ManGenerator::startConstraintList(const QCString &header)
{
  if (!m_firstCol)
  { m_t << "\n.PP\n";
    m_firstCol=TRUE; m_paragraph=TRUE;
    m_col=0;
  }
  m_paragraph=FALSE;
  startBold();
  docify(header);
  endBold();
  m_paragraph=TRUE;
}

void ManGenerator::startConstraintParam()
{
  startItemListItem();
  startEmphasis();
}

void ManGenerator::endConstraintParam()
{
  endEmphasis();
  endItemListItem();
  m_t << " : ";
}

void ManGenerator::startConstraintType()
{
  startEmphasis();
}

void ManGenerator::endConstraintType()
{
  endEmphasis();
}

void ManGenerator::startConstraintDocs()
{
}

void ManGenerator::endConstraintDocs()
{
  m_t << "\n"; m_firstCol=TRUE;
}

void ManGenerator::endConstraintList()
{
}


void ManGenerator::startInlineHeader()
{
  if (!m_firstCol)
  {
    m_t << "\n.PP\n" << ".in -1c\n";
  }
  m_t << ".RI \"\\fB";
}

void ManGenerator::endInlineHeader()
{
  m_t << "\\fP\"\n" << ".in +1c\n";
  m_firstCol = FALSE;
}

void ManGenerator::startMemberDocSimple(bool isEnum)
{
  if (!m_firstCol)
  {
    m_t << "\n.PP\n";
  }
  m_t << "\\fB";
  if (isEnum)
  {
    docify(theTranslator->trEnumerationValues());
  }
  else
  {
    docify(theTranslator->trCompoundMembers());
  }
  m_t << ":\\fP\n";
  m_t << ".RS 4\n";
}

void ManGenerator::endMemberDocSimple(bool)
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".RE\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManGenerator::startInlineMemberType()
{
}

void ManGenerator::endInlineMemberType()
{
  m_t << " ";
}

void ManGenerator::startInlineMemberName()
{
  m_t << "\\fI";
}

void ManGenerator::endInlineMemberName()
{
  m_t << "\\fP ";
}

void ManGenerator::startInlineMemberDoc()
{
}

void ManGenerator::endInlineMemberDoc()
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".br\n";
  m_t << ".PP\n";
  m_firstCol=TRUE;
}

void ManGenerator::startLabels()
{
}

void ManGenerator::writeLabel(const QCString &l,bool isLast)
{
  m_t << "\\fR [" << l << "]\\fP";
  if (!isLast) m_t << ", ";
}

void ManGenerator::endLabels()
{
}

void ManGenerator::endHeaderSection()
{
}

void ManGenerator::writeInheritedSectionTitle(
                  const QCString &/*id*/,    const QCString &/*ref*/,
                  const QCString &/*file*/,  const QCString &/*anchor*/,
                  const QCString &title,     const QCString &name)
{
  m_t << "\n\n";
  m_t << theTranslator->trInheritedFrom(docifyToString(title), objectLinkToString(name));
  m_firstCol = FALSE;
}

void ManGenerator::startParameterList(bool openBracket)
{
  if (openBracket) m_t << "(";
}

void ManGenerator::endParameterExtra(bool last,bool /* emptyList */, bool closeBracket)
{
  if (last && closeBracket)
  {
    m_t << ")";
  }
}
void ManGenerator::endParameterType()
{
  m_t << " ";
}

