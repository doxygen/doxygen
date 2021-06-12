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

/* http://www.cubic.org/source/archive/fileform/txt/man/ has some
   nice introductions to groff and man pages. */

#include <stdlib.h>

#include "message.h"
#include "mangen.h"
#include "config.h"
#include "util.h"
#include "doxygen.h"
#include <string.h>
#include "docparser.h"
#include "mandocvisitor.h"
#include "language.h"
#include "dir.h"
#include "utf8.h"

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

ManGenerator::ManGenerator() : OutputGenerator(Config_getString(MAN_OUTPUT)+"/"+getSubdir())
{
}

ManGenerator::ManGenerator(const ManGenerator &og) : OutputGenerator(og)
{
}

ManGenerator &ManGenerator::operator=(const ManGenerator &og)
{
  OutputGenerator::operator=(og);
  return *this;
}

std::unique_ptr<OutputGenerator> ManGenerator::clone() const
{
  return std::make_unique<ManGenerator>(*this);
}

ManGenerator::~ManGenerator()
{
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
  char c;
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

void ManGenerator::startFile(const QCString &,const QCString &manName,const QCString &,int)
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
  m_t << ".TH \"" << name << "\" " << getExtension() << " \""
    << dateToString(FALSE) << "\" \"";
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

void ManGenerator::writeCodeLink(const QCString &,const QCString &,
                                 const QCString &, const QCString &name,
                                 const QCString &)
{
  docify(name);
}

void ManGenerator::startHtmlLink(const QCString &)
{
}

void ManGenerator::endHtmlLink()
{
}

//void ManGenerator::writeMailLink(const QCString &url)
//{
//  docify(url);
//}

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
        case '-':  m_t << "\\-"; break; // see  bug747780
        case '.':  m_t << "\\&."; break; // see  bug652277
        case '\\': m_t << "\\\\"; m_col++; break;
        case '\n': m_t << "\n"; m_col=0; break;
        case '\"':  c = '\''; // no break!
        default: m_t << c; m_col++; break;
      }
    }
    m_firstCol=(c=='\n');
    //printf("%s",str);fflush(stdout);
  }
  m_paragraph=FALSE;
}

void ManGenerator::codify(const QCString &str)
{
  //static char spaces[]="        ";
  if (!str.isEmpty())
  {
    const char *p=str.data();
    char c;
    int spacesToNextTabStop;
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '.':   m_t << "\\&."; break; // see  bug652277
        case '\t':  spacesToNextTabStop =
                          Config_getInt(TAB_SIZE) - (m_col%Config_getInt(TAB_SIZE));
                    m_t << Doxygen::spaces.left(spacesToNextTabStop);
                    m_col+=spacesToNextTabStop;
                    break;
        case '\n':  m_t << "\n"; m_firstCol=TRUE; m_col=0; break;
        case '\\':  m_t << "\\"; m_col++; break;
        case '\"':  // no break!
        default:    p=writeUTF8Char(m_t,p-1); m_firstCol=FALSE; m_col++; break;
      }
    }
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

void ManGenerator::startDescList(SectionTypes)
{
  if (!m_firstCol)
  { m_t << "\n" << ".PP\n";
    m_firstCol=TRUE; m_paragraph=TRUE;
    m_col=0;
  }
  m_paragraph=FALSE;
  startBold();
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

void ManGenerator::startCodeFragment(const QCString &)
{
  newParagraph();
  m_t << ".nf\n";
  m_firstCol=TRUE;
  m_paragraph=FALSE;
}

void ManGenerator::endCodeFragment(const QCString &)
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".fi\n";
  m_firstCol=TRUE;
  m_paragraph=FALSE;
  m_col=0;
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
      std::ofstream linkStream(fileName.str(),std::ofstream::out | std::ofstream::binary);
      if (linkStream.is_open())
      {
        linkStream << ".so " << getSubdir() << "/" << buildFileName( manName ) << "\n";
      }
    }
}

void ManGenerator::endMemberDoc(bool)
{
    m_t << "\"\n";
}

void ManGenerator::startSubsection()
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".SS \"";
  m_firstCol=FALSE;
  m_paragraph=FALSE;
}

void ManGenerator::endSubsection()
{
  m_t << "\"";
}


void ManGenerator::startSubsubsection()
{
  if (!m_firstCol) m_t << "\n";
  m_t << "\n.SS \"";
  m_firstCol=FALSE;
  m_paragraph=FALSE;
}

void ManGenerator::endSubsubsection()
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

void ManGenerator::startDescItem()
{
  if (!m_firstCol) m_t << "\n";
  m_t << ".IP \"";
  m_firstCol=FALSE;
}

//void ManGenerator::endDescTitle()
//{
//  endBold();
//  m_paragraph=TRUE;
//}

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

void ManGenerator::endDescItem()
{
  m_t << "\" 1c\n";
  m_firstCol=TRUE;
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


void ManGenerator::startMemberItem(const QCString &,int,const QCString &)
{
  if (m_firstCol && !m_insideTabbing) m_t << ".in +1c\n";
  m_t << "\n.ti -1c\n.RI \"";
  m_firstCol=FALSE;
}

void ManGenerator::endMemberItem()
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
    switch(type)
    {
      case SectionType::Page:          startGroupHeader(FALSE); break;
      case SectionType::Section:       startGroupHeader(FALSE); break;
      case SectionType::Subsection:    startMemberHeader(QCString(), -1); break;
      case SectionType::Subsubsection: startMemberHeader(QCString(), -1); break;
      case SectionType::Paragraph:     startMemberHeader(QCString(), -1); break;
      default: ASSERT(0); break;
    }
  }
}

void ManGenerator::endSection(const QCString &,SectionType type)
{
  if( !m_inHeader )
  {
    switch(type)
    {
      case SectionType::Page:          endGroupHeader(0); break;
      case SectionType::Section:       endGroupHeader(0); break;
      case SectionType::Subsection:    endMemberHeader(); break;
      case SectionType::Subsubsection: endMemberHeader(); break;
      case SectionType::Paragraph:     endMemberHeader(); break;
      default: ASSERT(0); break;
    }
  }
  else
  {
    m_t << "\n";
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

void ManGenerator::startDescTable(const QCString &title)
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

void ManGenerator::startParamList(ParamListTypes,const QCString &title)
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
}

void ManGenerator::endParamList()
{
}

void ManGenerator::writeDoc(DocNode *n,const Definition *ctx,const MemberDef *,int)
{
  ManDocVisitor *visitor = new ManDocVisitor(m_t,*this,ctx?ctx->getDefFileExtension():QCString(""));
  n->accept(visitor);
  delete visitor;
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
  m_t << "\\fC [" << l << "]\\fP";
  if (!isLast) m_t << ", ";
}

void ManGenerator::endLabels()
{
}

void ManGenerator::endHeaderSection()
{
}
