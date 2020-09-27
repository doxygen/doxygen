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

#include <qdir.h>
#include "message.h"
#include "mangen.h"
#include "config.h"
#include "util.h"
#include "doxygen.h"
#include <string.h>
#include "docparser.h"
#include "mandocvisitor.h"
#include "language.h"

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

  QDir d(manOutput);
  if (!d.exists() && !d.mkdir(manOutput))
  {
    term("Could not create output directory %s\n",manOutput.data());
  }
  d.setPath(manOutput + "/" + getSubdir());
  if (!d.exists() && !d.mkdir(manOutput + "/" + getSubdir()))
  {
    term("Could not create output directory %s/%s\n",manOutput.data(), getSubdir().data());
  }
  createSubDirs(d);
}

static QCString buildFileName(const char *name)
{
  QCString fileName;
  if (name==0) return "noname";

  const char *p=name;
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

void ManGenerator::startFile(const char *,const char *manName,const char *)
{
  startPlainFile( buildFileName( manName ) );
  m_firstCol=TRUE;
}

void ManGenerator::endFile()
{
  t << endl;
  endPlainFile();
}

void ManGenerator::endTitleHead(const char *,const char *name)
{
  t << ".TH \"" << name << "\" " << getExtension() << " \""
    << dateToString(FALSE) << "\" \"";
  if (!Config_getString(PROJECT_NUMBER).isEmpty())
    t << "Version " << Config_getString(PROJECT_NUMBER) << "\" \"";
  if (Config_getString(PROJECT_NAME).isEmpty())
    t << "Doxygen";
  else
    t << Config_getString(PROJECT_NAME);
  t << "\" \\\" -*- nroff -*-" << endl;
  t << ".ad l" << endl;
  t << ".nh" << endl;
  t << ".SH NAME" << endl;
  t << name;
  m_firstCol=FALSE;
  m_paragraph=TRUE;
  m_inHeader=TRUE;
}

void ManGenerator::newParagraph()
{
  if (!m_paragraph)
  {
    if (!m_firstCol) t << endl;
    t << ".PP" << endl;
    m_firstCol=TRUE;
  }
  m_paragraph=TRUE;
}

void ManGenerator::startParagraph(const char *)
{
  if (!m_paragraph)
  {
    if (!m_firstCol) t << endl;
    t << ".PP" << endl;
    m_firstCol=TRUE;
  }
  m_paragraph=TRUE;
}

void ManGenerator::endParagraph()
{
}

void ManGenerator::writeString(const char *text)
{
  docify(text);
}

void ManGenerator::startIndexItem(const char *,const char *)
{
}

void ManGenerator::endIndexItem(const char *,const char *)
{
}

void ManGenerator::writeStartAnnoItem(const char *,const char *,
                                       const char *,const char *)
{
}

void ManGenerator::writeObjectLink(const char *,const char *,
                                    const char *, const char *name)
{
  startBold(); docify(name); endBold();
}

void ManGenerator::writeCodeLink(const char *,const char *,
                                 const char *, const char *name,
                                 const char *)
{
  docify(name);
}

void ManGenerator::startHtmlLink(const char *)
{
}

void ManGenerator::endHtmlLink()
{
}

//void ManGenerator::writeMailLink(const char *url)
//{
//  docify(url);
//}

void ManGenerator::startGroupHeader(int)
{
  if (!m_firstCol) t << endl;
  t << ".SH \"";
  m_upperCase=TRUE;
  m_firstCol=FALSE;
}

void ManGenerator::endGroupHeader(int)
{
  t << "\"\n.PP " << endl;
  m_firstCol=TRUE;
  m_paragraph=TRUE;
  m_upperCase=FALSE;
}

void ManGenerator::startMemberHeader(const char *,int)
{
  if (!m_firstCol) t << endl;
  t << ".SS \"";
}

void ManGenerator::endMemberHeader()
{
  t << "\"\n";
  m_firstCol=TRUE;
  m_paragraph=FALSE;
}

void ManGenerator::docify(const char *str)
{
  if (str)
  {
    const char *p=str;
    char c=0;
    while ((c=*p++))
    {
      switch(c)
      {
        case '-':  t << "\\-"; break; // see  bug747780
        case '.':  t << "\\&."; break; // see  bug652277
        case '\\': t << "\\\\"; m_col++; break;
        case '\n': t << "\n"; m_col=0; break;
        case '\"':  c = '\''; // no break!
        default: t << c; m_col++; break;
      }
    }
    m_firstCol=(c=='\n');
    //printf("%s",str);fflush(stdout);
  }
  m_paragraph=FALSE;
}

void ManGenerator::codify(const char *str)
{
  //static char spaces[]="        ";
  if (str)
  {
    const char *p=str;
    char c;
    int spacesToNextTabStop;
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '.':   t << "\\&."; break; // see  bug652277
        case '\t':  spacesToNextTabStop =
                          Config_getInt(TAB_SIZE) - (m_col%Config_getInt(TAB_SIZE));
                    t << Doxygen::spaces.left(spacesToNextTabStop);
                    m_col+=spacesToNextTabStop;
                    break;
        case '\n':  t << "\n"; m_firstCol=TRUE; m_col=0; break;
        case '\\':  t << "\\"; m_col++; break;
        case '\"':  // no break!
        default:    p=writeUtf8Char(t,p-1); m_firstCol=FALSE; m_col++; break;
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
    case '\\': t << "\\\\"; break;
  case '\"': c = '\''; // no break!
    default:   t << c; break;
  }
  //printf("%c",c);fflush(stdout);
  m_paragraph=FALSE;
}

void ManGenerator::startDescList(SectionTypes)
{
  if (!m_firstCol)
  { t << endl << ".PP" << endl;
    m_firstCol=TRUE; m_paragraph=TRUE;
    m_col=0;
  }
  m_paragraph=FALSE;
  startBold();
}

void ManGenerator::startTitle()
{
  if (!m_firstCol) t << endl;
  t << ".SH \"";
  m_firstCol=FALSE;
  m_paragraph=FALSE;
}

void ManGenerator::endTitle()
{
    t << "\"";
}

void ManGenerator::startItemListItem()
{
  if (!m_firstCol) t << endl;
  t << ".TP" << endl;
  m_firstCol=TRUE;
  m_paragraph=FALSE;
  m_col=0;
}

void ManGenerator::endItemListItem()
{
}

void ManGenerator::startCodeFragment(const char *)
{
  newParagraph();
  t << ".nf" << endl;
  m_firstCol=TRUE;
  m_paragraph=FALSE;
}

void ManGenerator::endCodeFragment()
{
  if (!m_firstCol) t << endl;
  t << ".fi" << endl;
  m_firstCol=TRUE;
  m_paragraph=FALSE;
  m_col=0;
}

void ManGenerator::startMemberDoc(const char *,const char *,const char *,const char *,int,int,bool)
{
  if (!m_firstCol) t << endl;
  t << ".SS \"";
  m_firstCol=FALSE;
  m_paragraph=FALSE;
}

void ManGenerator::startDoxyAnchor(const char *,const char *manName,
                                   const char *, const char *name,
                                   const char *)
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
    //       name,baseName.data(),buildFileName(baseName).data());

    // - remove dangerous characters and append suffix, then add dir prefix
    QCString fileName=dir()+"/"+buildFileName( baseName );
    QFile linkfile( fileName );
    // - only create file if it doesn't exist already
    if ( !linkfile.open( IO_ReadOnly ) )
    {
	if ( linkfile.open( IO_WriteOnly ) )
        {
	      FTextStream linkstream;
	      linkstream.setDevice(&linkfile);
	      //linkstream.setEncoding(QTextStream::UnicodeUTF8);
	      linkstream << ".so " << getSubdir() << "/" << buildFileName( manName ) << endl;
	}
    }
    linkfile.close();
}

void ManGenerator::endMemberDoc(bool)
{
    t << "\"\n";
}

void ManGenerator::startSubsection()
{
  if (!m_firstCol) t << endl;
  t << ".SS \"";
  m_firstCol=FALSE;
  m_paragraph=FALSE;
}

void ManGenerator::endSubsection()
{
  t << "\"";
}


void ManGenerator::startSubsubsection()
{
  if (!m_firstCol) t << endl;
  t << "\n.SS \"";
  m_firstCol=FALSE;
  m_paragraph=FALSE;
}

void ManGenerator::endSubsubsection()
{
  t << "\"";
}

void ManGenerator::writeSynopsis()
{
  if (!m_firstCol) t << endl;
  t << ".SH SYNOPSIS\n.br\n.PP\n";
  m_firstCol=TRUE;
  m_paragraph=FALSE;
}

void ManGenerator::startDescItem()
{
  if (!m_firstCol) t << endl;
  t << ".IP \"";
  m_firstCol=FALSE;
}

//void ManGenerator::endDescTitle()
//{
//  endBold();
//  m_paragraph=TRUE;
//}

void ManGenerator::startDescForItem()
{
  if (!m_firstCol) t << endl;
  if (!m_paragraph) t << ".in -1c" << endl;
  t << ".in +1c" << endl;
  m_firstCol=TRUE;
  m_paragraph=FALSE;
  m_col=0;
}

void ManGenerator::endDescForItem()
{
}

void ManGenerator::endDescItem()
{
  t << "\" 1c" << endl;
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


void ManGenerator::startMemberItem(const char *,int,const char *)
{
  if (m_firstCol && !m_insideTabbing) t << ".in +1c\n";
  t << "\n.ti -1c\n.RI \"";
  m_firstCol=FALSE;
}

void ManGenerator::endMemberItem()
{
  t << "\"\n.br";
}

void ManGenerator::startMemberList()
{
  if (!m_insideTabbing)
  {
    t << "\n.in +1c"; m_firstCol=FALSE;
  }
}

void ManGenerator::endMemberList()
{
  if (!m_insideTabbing)
  {
    t << "\n.in -1c"; m_firstCol=FALSE;
  }
}

void ManGenerator::startMemberGroupHeader(bool)
{
  t << "\n.PP\n.RI \"\\fB";
}

void ManGenerator::endMemberGroupHeader()
{
  t << "\\fP\"\n.br\n";
  m_firstCol=TRUE;
}

void ManGenerator::startMemberGroupDocs()
{
}

void ManGenerator::endMemberGroupDocs()
{
  t << "\n.PP";
}

void ManGenerator::startMemberGroup()
{
  t << "\n.in +1c";
}

void ManGenerator::endMemberGroup(bool)
{
  t << "\n.in -1c";
  m_firstCol=FALSE;
}

void ManGenerator::startSection(const char *,const char *,SectionType type)
{
  if( !m_inHeader )
  {
    switch(type)
    {
      case SectionType::Page:          startGroupHeader(FALSE); break;
      case SectionType::Section:       startGroupHeader(FALSE); break;
      case SectionType::Subsection:    startMemberHeader(0, -1); break;
      case SectionType::Subsubsection: startMemberHeader(0, -1); break;
      case SectionType::Paragraph:     startMemberHeader(0, -1); break;
      default: ASSERT(0); break;
    }
  }
}

void ManGenerator::endSection(const char *,SectionType type)
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
    t << "\n";
    m_firstCol=TRUE;
    m_paragraph=FALSE;
    m_inHeader=FALSE;
  }
}

void ManGenerator::startExamples()
{
  if (!m_firstCol)
  { t << endl << ".PP" << endl;
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

void ManGenerator::startDescTable(const char *title)
{
  if (!m_firstCol)
  { t << endl << ".PP" << endl;
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

void ManGenerator::startParamList(ParamListTypes,const char *title)
{
  if (!m_firstCol)
  { t << endl << ".PP" << endl;
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

void ManGenerator::writeDoc(DocNode *n,const Definition *ctx,const MemberDef *)
{
  ManDocVisitor *visitor = new ManDocVisitor(t,*this,ctx?ctx->getDefFileExtension():QCString(""));
  n->accept(visitor);
  delete visitor;
  m_firstCol=FALSE;
  m_paragraph = FALSE;
}

void ManGenerator::startConstraintList(const char *header)
{
  if (!m_firstCol)
  { t << endl << ".PP" << endl;
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
  t << " : ";
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
  t << endl; m_firstCol=TRUE;
}

void ManGenerator::endConstraintList()
{
}


void ManGenerator::startInlineHeader()
{
  if (!m_firstCol)
  {
    t << endl << ".PP" << endl << ".in -1c" << endl;
  }
  t << ".RI \"\\fB";
}

void ManGenerator::endInlineHeader()
{
  t << "\\fP\"" << endl << ".in +1c" << endl;
  m_firstCol = FALSE;
}

void ManGenerator::startMemberDocSimple(bool isEnum)
{
  if (!m_firstCol)
  {
    t << endl << ".PP" << endl;
  }
  t << "\\fB";
  if (isEnum)
  {
    docify(theTranslator->trEnumerationValues());
  }
  else
  {
    docify(theTranslator->trCompoundMembers());
  }
  t << ":\\fP" << endl;
  t << ".RS 4" << endl;
}

void ManGenerator::endMemberDocSimple(bool)
{
  if (!m_firstCol) t << endl;
  t << ".RE" << endl;
  t << ".PP" << endl;
  m_firstCol=TRUE;
}

void ManGenerator::startInlineMemberType()
{
}

void ManGenerator::endInlineMemberType()
{
  t << " ";
}

void ManGenerator::startInlineMemberName()
{
  t << "\\fI";
}

void ManGenerator::endInlineMemberName()
{
  t << "\\fP ";
}

void ManGenerator::startInlineMemberDoc()
{
}

void ManGenerator::endInlineMemberDoc()
{
  if (!m_firstCol) t << endl;
  t << ".br" << endl;
  t << ".PP" << endl;
  m_firstCol=TRUE;
}

void ManGenerator::startLabels()
{
}

void ManGenerator::writeLabel(const char *l,bool isLast)
{
  t << "\\fC [" << l << "]\\fP";
  if (!isLast) t << ", ";
}

void ManGenerator::endLabels()
{
}

void ManGenerator::endHeaderSection()
{
}
