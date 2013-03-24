/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
  QCString ext = Config_getString("MAN_EXTENSION");
  if( ext.length() >= 2 &&
      ext.data()[0] == '.')
  {
    ext = ext.mid(1, ext.length()-1);
  }
  else
  {
    ext = "3"; 
  }
  return ext;
}

ManGenerator::ManGenerator() : OutputGenerator()
{
  dir=Config_getString("MAN_OUTPUT")+"/man" + getExtension();
  firstCol=TRUE;
  paragraph=FALSE;
  col=0;
  upperCase=FALSE;
  insideTabbing=FALSE;
  inHeader=FALSE;
}

ManGenerator::~ManGenerator()
{
}

//void ManGenerator::append(const OutputGenerator *g)
//{
//  QCString r=g->getContents();
//  if (upperCase)
//    t << r.upper();
//  else
//    t << r;
//  if (!r.isEmpty())
//    firstCol = r.at(r.length()-1)=='\n';
//  else
//    firstCol = ((ManGenerator *)g)->firstCol;
//  col+=((ManGenerator *)g)->col;
//  inHeader=((ManGenerator *)g)->inHeader;
//  paragraph=FALSE;
//}

void ManGenerator::init()
{
  QCString ext = getExtension();
  QCString &manOutput = Config_getString("MAN_OUTPUT");
  
  QDir d(manOutput);
  if (!d.exists() && !d.mkdir(manOutput))
  {
    err("Could not create output directory %s\n",manOutput.data());
    exit(1);
  }
  d.setPath(manOutput+"/man"+ext);
  if (!d.exists() && !d.mkdir(manOutput+"/man"+ext))
  {
    err("Could not create output directory %s/man%s\n",manOutput.data(),ext.data());
    exit(1);
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

  QCString &manExtension = Config_getString("MAN_EXTENSION");
  if (fileName.right(2)!=manExtension) 
  {
    fileName+=manExtension;
  }

  return fileName;
}

void ManGenerator::startFile(const char *,const char *manName,const char *)
{
  startPlainFile( buildFileName( manName ) );
  firstCol=TRUE;
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
  if (!Config_getString("PROJECT_NUMBER").isEmpty())
    t << "Version " << Config_getString("PROJECT_NUMBER") << "\" \"";
  if (Config_getString("PROJECT_NAME").isEmpty()) 
    t << "Doxygen";
  else
    t << Config_getString("PROJECT_NAME");
  t << "\" \\\" -*- nroff -*-" << endl;
  t << ".ad l" << endl;
  t << ".nh" << endl;
  t << ".SH NAME" << endl;
  t << name << " \\- ";
  firstCol=FALSE;
  inHeader=TRUE;
}

void ManGenerator::newParagraph()
{
  if (!paragraph)
  {
    if (!firstCol) t << endl;
    t << ".PP" << endl;
    firstCol=TRUE;
  }
  paragraph=TRUE;
}

void ManGenerator::startParagraph()
{
  if (!paragraph)
  {
    if (!firstCol) t << endl;
    t << ".PP" << endl;
    firstCol=TRUE;
  }
  paragraph=TRUE;
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
  if (!firstCol) t << endl;
  t << ".SH \"";
  upperCase=TRUE;
  firstCol=FALSE;
}

void ManGenerator::endGroupHeader(int)
{
  t << "\"\n.PP " << endl;
  firstCol=TRUE;
  paragraph=TRUE;
  upperCase=FALSE;
}

void ManGenerator::startMemberHeader(const char *)
{
  if (!firstCol) t << endl;
  t << ".SS \"";
}

void ManGenerator::endMemberHeader()
{
  t << "\"\n";
  firstCol=TRUE;
  paragraph=FALSE;
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
        case '.':  t << "\\&."; break; // see  bug652277
        case '\\': t << "\\\\"; col++; break;
        case '\n': t << "\n"; col=0; break;
        case '\"':  c = '\''; // no break!
        default: t << c; col++; break;
      }
    }
    firstCol=(c=='\n');
    //printf("%s",str);fflush(stdout);
  }
  paragraph=FALSE;
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
                          Config_getInt("TAB_SIZE") - (col%Config_getInt("TAB_SIZE"));
                    t << Doxygen::spaces.left(spacesToNextTabStop); 
                    col+=spacesToNextTabStop; 
                    break;
        case '\n':  t << "\n"; firstCol=TRUE; col=0; break;
        case '\\':  t << "\\"; col++; break;
        case '\"':  c = '\''; // no break!
        default:    p=writeUtf8Char(t,p-1); firstCol=FALSE; col++; break;
      }
    }
    //printf("%s",str);fflush(stdout);
  }
  paragraph=FALSE;
}

void ManGenerator::writeChar(char c)
{
  firstCol=(c=='\n');
  if (firstCol) col=0; else col++;
  switch (c)
  {
    case '\\': t << "\\\\"; break;
  case '\"': c = '\''; // no break!
    default:   t << c; break;
  }
  //printf("%c",c);fflush(stdout);
  paragraph=FALSE;
}

void ManGenerator::startDescList(SectionTypes)      
{
  if (!firstCol) 
  { t << endl << ".PP" << endl; 
    firstCol=TRUE; paragraph=TRUE; 
    col=0;
  }
  paragraph=FALSE;
  startBold();
}

void ManGenerator::startTitle() 
{ 
  if (!firstCol) t << endl; 
  t << ".SH \""; 
  firstCol=FALSE;
  paragraph=FALSE;
}

void ManGenerator::endTitle()
{
    t << "\"";
}

void ManGenerator::startItemListItem() 
{ 
  if (!firstCol) t << endl; 
  t << ".TP" << endl; 
  firstCol=TRUE;
  paragraph=FALSE;
  col=0;
} 

void ManGenerator::endItemListItem()
{
}

void ManGenerator::startCodeFragment() 
{ 
  newParagraph();
  t << ".nf" << endl; 
  firstCol=TRUE;
  paragraph=FALSE;
}

void ManGenerator::endCodeFragment()   
{ 
  if (!firstCol) t << endl;
  t << ".fi" << endl; 
  firstCol=TRUE;
  paragraph=FALSE;
  col=0;
}

void ManGenerator::startMemberDoc(const char *,const char *,const char *,const char *,bool) 
{ 
  if (!firstCol) t << endl;
  t << ".SS \""; 
  firstCol=FALSE;
  paragraph=FALSE;
}

void ManGenerator::startDoxyAnchor(const char *,const char *manName,
                                   const char *, const char *name,
                                   const char *)
{
    // something to be done?
    if( !Config_getBool("MAN_LINKS") ) 
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
    QCString fileName=dir+"/"+buildFileName( baseName );
    QFile linkfile( fileName );
    // - only create file if it doesn't exist already
    if ( !linkfile.open( IO_ReadOnly ) ) 
    {
	if ( linkfile.open( IO_WriteOnly ) ) 
        {
	      FTextStream linkstream;
	      linkstream.setDevice(&linkfile);
	      //linkstream.setEncoding(QTextStream::UnicodeUTF8);
	      linkstream << ".so man" << getExtension() << "/" << buildFileName( manName ) << endl;
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
  if (!firstCol) t << endl;
  t << ".SS \""; 
  firstCol=FALSE;
  paragraph=FALSE;
}

void ManGenerator::endSubsection()
{
  t << "\"";
}


void ManGenerator::startSubsubsection() 
{ 
  if (!firstCol) t << endl;
  t << "\n.SS \""; 
  firstCol=FALSE;
  paragraph=FALSE;
}

void ManGenerator::endSubsubsection()
{
  t << "\"";
}

void ManGenerator::writeSynopsis()      
{ 
  if (!firstCol) t << endl;
  t << ".SH SYNOPSIS\n.br\n.PP\n"; 
  firstCol=TRUE;
  paragraph=FALSE;
}

void ManGenerator::startDescItem()
{
  if (!firstCol) t << endl;
  t << ".IP \"";
  firstCol=FALSE;
}

//void ManGenerator::endDescTitle()
//{
//  endBold();
//  paragraph=TRUE;
//}

void ManGenerator::startDescForItem()
{
  if (!firstCol) t << endl;
  if (!paragraph) t << ".in -1c" << endl;
  t << ".in +1c" << endl;
  firstCol=TRUE;
  paragraph=FALSE;
  col=0;
}

void ManGenerator::endDescForItem()
{
}

void ManGenerator::endDescItem()
{
  t << "\" 1c" << endl;
  firstCol=TRUE;
}

void ManGenerator::startAnonTypeScope(int indentLevel)
{
  if (indentLevel==0)
  {
    insideTabbing=TRUE;
  }
}

void ManGenerator::endAnonTypeScope(int indentLevel)
{
  if (indentLevel==0)
  {
    insideTabbing=FALSE;
  }
}


void ManGenerator::startMemberItem(const char *,int,const char *) 
{ 
  if (firstCol && !insideTabbing) t << ".in +1c\n";
  t << "\n.ti -1c\n.RI \""; 
  firstCol=FALSE;
}

void ManGenerator::endMemberItem() 
{ 
  t << "\"\n.br"; 
}

void ManGenerator::startMemberList() 
{ 
  if (!insideTabbing)
  {
    t << "\n.in +1c"; firstCol=FALSE; 
  }
}

void ManGenerator::endMemberList() 
{ 
  if (!insideTabbing)
  {
    t << "\n.in -1c"; firstCol=FALSE; 
  }
}

void ManGenerator::startMemberGroupHeader(bool)
{
  t << "\n.PP\n.RI \"\\fB";
}

void ManGenerator::endMemberGroupHeader()
{
  t << "\\fP\"\n.br\n";
  firstCol=TRUE;
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
  firstCol=FALSE;
}

void ManGenerator::startSection(const char *,const char *,SectionInfo::SectionType type)
{
  if( !inHeader ) 
  {
    switch(type)
    {
      case SectionInfo::Page:          startGroupHeader(FALSE); break;
      case SectionInfo::Section:       startGroupHeader(FALSE); break;
      case SectionInfo::Subsection:    startMemberHeader(0); break;
      case SectionInfo::Subsubsection: startMemberHeader(0); break;
      case SectionInfo::Paragraph:     startMemberHeader(0); break;
      default: ASSERT(0); break;
    }
  }
}

void ManGenerator::endSection(const char *,SectionInfo::SectionType type)
{
  if( !inHeader )
  {
    switch(type)
    {
      case SectionInfo::Page:          endGroupHeader(0); break;
      case SectionInfo::Section:       endGroupHeader(0); break;
      case SectionInfo::Subsection:    endMemberHeader(); break;
      case SectionInfo::Subsubsection: endMemberHeader(); break;
      case SectionInfo::Paragraph:     endMemberHeader(); break;
      default: ASSERT(0); break;
    }
  }
  else
  {
    t << "\n";
    firstCol=TRUE;
    paragraph=FALSE;
    inHeader=FALSE;
  }
}

void ManGenerator::startSimpleSect(SectionTypes,const char *,
                                   const char *,const char *title)
{
  if (!firstCol) 
  { t << endl << ".PP" << endl; 
    firstCol=TRUE; paragraph=TRUE; 
    col=0;
  }
  paragraph=FALSE;
  startBold();
  docify(title);
  endBold();
  paragraph=TRUE;
}

void ManGenerator::endSimpleSect()
{
}

void ManGenerator::startParamList(ParamListTypes,const char *title)
{
  if (!firstCol) 
  { t << endl << ".PP" << endl; 
    firstCol=TRUE; paragraph=TRUE; 
    col=0;
  }
  paragraph=FALSE;
  startBold();
  docify(title);
  endBold();
  paragraph=TRUE;
}

void ManGenerator::endParamList()
{
}

void ManGenerator::writeDoc(DocNode *n,Definition *ctx,MemberDef *)
{
  ManDocVisitor *visitor = new ManDocVisitor(t,*this,ctx?ctx->getDefFileExtension():QCString(""));
  n->accept(visitor);
  delete visitor; 
  firstCol=FALSE;
  paragraph = FALSE;
}

void ManGenerator::startConstraintList(const char *header)
{
  if (!firstCol) 
  { t << endl << ".PP" << endl; 
    firstCol=TRUE; paragraph=TRUE; 
    col=0;
  }
  paragraph=FALSE;
  startBold();
  docify(header);
  endBold();
  paragraph=TRUE;
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
  t << endl; firstCol=TRUE;
}

void ManGenerator::endConstraintList()
{
}


void ManGenerator::startInlineHeader() 
{
  if (!firstCol) 
  {
    t << endl << ".PP" << endl << ".in -1c" << endl;
  }
  t << ".RI \"\\fB"; 
}

void ManGenerator::endInlineHeader() 
{
  t << "\\fP\"" << endl << ".in +1c" << endl;
  firstCol = FALSE;
}

void ManGenerator::startMemberDocSimple()
{
  if (!firstCol) 
  {
    t << endl << ".PP" << endl;
  }
  t << "\\fB";
  docify(theTranslator->trCompoundMembers());
  t << ":\\fP" << endl;
  t << ".RS 4" << endl;
}

void ManGenerator::endMemberDocSimple()
{
  if (!firstCol) t << endl;
  t << ".RE" << endl;
  t << ".PP" << endl;
  firstCol=TRUE;
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
  if (!firstCol) t << endl;
  t << ".br" << endl;
  t << ".PP" << endl;
  firstCol=TRUE;
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


