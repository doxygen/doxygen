/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#include <stdlib.h>

#include "qtbc.h"
#include <qdir.h>
#include "message.h"
#include "mangen.h"
#include "config.h"
#include "util.h"
#include "doxygen.h"

ManGenerator::ManGenerator() : OutputGenerator()
{
  dir=Config::manOutputDir+"/man3";
  firstCol=TRUE;
  paragraph=FALSE;
  col=0;
  upperCase=FALSE;
  insideTabbing=FALSE;
}

ManGenerator::~ManGenerator()
{
}

void ManGenerator::append(const OutputGenerator *g)
{
  QCString r=g->getContents();
  if (upperCase)
    t << r.upper();
  else
    t << r;
  if (r.length()>0)
    firstCol = r.at(r.length()-1)=='\n';
  else
    firstCol = ((ManGenerator *)g)->firstCol;
  col+=((ManGenerator *)g)->col;
  paragraph=FALSE;
}

void ManGenerator::init()
{
  QDir d(Config::manOutputDir);
  if (!d.exists() && !d.mkdir(Config::manOutputDir))
  {
    err("Could not create output directory %s\n",Config::manOutputDir.data());
    exit(1);
  }
  d.setPath(Config::manOutputDir+"/man3");
  if (!d.exists() && !d.mkdir(Config::manOutputDir+"/man3"))
  {
    err("Could not create output directory %s/man3\n",Config::manOutputDir.data());
    exit(1);
  }
}

void ManGenerator::startFile(const char *name,const char *,bool)
{
  QCString fileName=name;
  if (fileName.left(6)=="class_")
  {
    fileName=fileName.right(fileName.length()-6);
  }
  int i;
  if ((i=fileName.findRev('.'))!=-1)
  {
    fileName=fileName.left(i); 
  }
  if (convertToQCString(fileName.right(2))!=Config::manExtension) 
    fileName+=Config::manExtension;
  startPlainFile(fileName);
  firstCol=TRUE;
}

void ManGenerator::endFile()
{
  endPlainFile();
}

void ManGenerator::writeDoxyAnchor(const char *, const char *,const char *)
{
}

void ManGenerator::endTitleHead(const char *name)
{
  t << ".TH " << name << " 3 \"" << dateToString(FALSE) << "\" \"";
  if (Config::projectName.isEmpty()) 
    t << "Doxygen";
  else
    t << Config::projectName;
  t << "\" \\\" -*- nroff -*-" << endl;
  t << ".ad l" << endl;
  t << ".nh" << endl;
  t << ".SH NAME" << endl;
  t << name << " \\- ";
  firstCol=FALSE;
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

void ManGenerator::writeString(const char *text)
{
  docify(text);
}

void ManGenerator::writeIndexItem(const char *,const char *,
                                   const char *)
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
                                    const char *, const char *name)
{
  docify(name);
}

void ManGenerator::writeHtmlLink(const char *,const char *text)
{
  docify(text);
}

void ManGenerator::startGroupHeader()
{
  if (!firstCol) t << endl;
  t << ".SH ";
  upperCase=TRUE;
  firstCol=FALSE;
}

void ManGenerator::endGroupHeader()
{
  t << "\n.PP " << endl;
  firstCol=TRUE;
  paragraph=TRUE;
  upperCase=FALSE;
}

void ManGenerator::startMemberHeader()
{
  if (!firstCol) t << endl;
  t << ".SS ";
}

void ManGenerator::endMemberHeader()
{
  t << "\n";
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
        case '\\': t << "\\\\"; col++; break;
        case '\n': t << "\n"; col=0; break;
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
  static char spaces[]="        ";
  if (str)
  {
    const char *p=str;
    char c;
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '\t':  t << &spaces[col&7]; col+=8-(col&7); break;
        case '\n':  t << "\n.br\n"; firstCol=TRUE; col=0; break;
        case '\\':  t << "\\\\"; col++; break;
        default:    t << c; firstCol=FALSE; col++; break;
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
    default:   t << c; break;
  }
  //printf("%c",c);fflush(stdout);
  paragraph=FALSE;
}

void ManGenerator::startDescList()      
{
  if (!firstCol) 
  { t << endl << ".PP" << endl; 
    firstCol=TRUE; paragraph=TRUE; 
    col=0;
  }
  paragraph=FALSE;
}

void ManGenerator::startTitle() 
{ 
  if (!firstCol) t << endl; 
  t << ".SH "; 
  firstCol=FALSE;
  paragraph=FALSE;
}

void ManGenerator::writeListItem() 
{ 
  if (!firstCol) t << endl; 
  t << ".TP" << endl; 
  firstCol=TRUE;
  paragraph=FALSE;
  col=0;
} 

void ManGenerator::startCodeFragment() 
{ 
  newParagraph();
  t << ".nf"; 
  firstCol=FALSE;
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

void ManGenerator::startMemberDoc(const char *,const char *,const char *) 
{ 
  if (!firstCol) t << endl;
  t << ".SS "; 
  firstCol=FALSE;
  paragraph=FALSE;
}

void ManGenerator::startSubsection()    
{ 
  if (!firstCol) t << endl;
  t << ".SS "; 
  firstCol=FALSE;
  paragraph=FALSE;
}

void ManGenerator::startSubsubsection() 
{ 
  if (!firstCol) t << endl;
  t << "\n.SS "; 
  firstCol=FALSE;
  paragraph=FALSE;
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

void ManGenerator::endDescTitle()
{
  paragraph=TRUE;
}

void ManGenerator::writeDescItem()
{
  if (!firstCol) t << endl;
  if (!paragraph) t << ".in -1c" << endl;
  t << ".in +1c" << endl;
  firstCol=TRUE;
  paragraph=FALSE;
  col=0;
}

void ManGenerator::endDescItem()
{
  t << "\" 1c" << endl;
  firstCol=TRUE;
}

void ManGenerator::startMemberItem(bool,int annType) 
{ 
  if (firstCol && !insideTabbing) t << ".in +1c\n";
  t << "\n.ti -1c\n.RI \""; 
  firstCol=FALSE;
  if (annType!=0) insideTabbing=TRUE;
}

void ManGenerator::endMemberItem(bool,const char *,const char *,bool endItem) 
{ 
  if (endItem)
  {
    insideTabbing=FALSE;
    t << "\"\n.br\n.RI \"";
  }
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

