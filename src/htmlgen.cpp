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

#include <qdir.h>
#include <stdlib.h>
#include "message.h"
#include "htmlgen.h"
#include "config.h"
#include "util.h"
#include "doxygen.h"
#include "logos.h"
#include "diagram.h"

HtmlGenerator::HtmlGenerator() : OutputGenerator()
{
  if (headerFile.length()>0) header=fileToString(headerFile);
  if (footerFile.length()>0) footer=fileToString(footerFile);
  dir=htmlOutputDir;
}

HtmlGenerator::~HtmlGenerator()
{
}

void HtmlGenerator::append(const OutputGenerator *g)
{
  t << g->getContents();
}

void HtmlGenerator::init()
{
  QDir d(htmlOutputDir);
  if (!d.exists() && !d.mkdir(htmlOutputDir))
  {
    err("Could not create output directory %s\n",htmlOutputDir.data());
    exit(1);
  }
  writeLogo(htmlOutputDir);
}

void HtmlGenerator::startFile(const char *name,const char *title,bool external)
{
  QString fileName=name;
  lastTitle=title;
  if (fileName.right(5)!=".html") fileName+=".html";
  startPlainFile(fileName);
  if (header.length()==0) 
  {
    t << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
         "<html><head><meta name=\"robots\" content=\"noindex\">\n"
         "<title>" << title << "</title>\n";
    t << "<link ";
    if (external) 
      t << "doxygen=\"_doc:\" href=\"/";
    else
      t << "href=\"";
    t << "doxygen.css\" rel=\"stylesheet\" type=\"text/css\">\n"
         "</head><body bgcolor=\"#ffffff\">\n";
  }
  else
  {
    t << substitute(
           substitute(  
             substitute(header,"$title",lastTitle),
             "$datetime",dateToString(TRUE)
           ),
           "$date",dateToString(FALSE)
         );
  }
}

void HtmlGenerator::startQuickIndexItem(const char *s,const char *l)
{
  t << "<a ";
  if (s) t << "doxygen=\"" << s << "\" ";
  t << "href=\"" << l << "\">";
}

void HtmlGenerator::endQuickIndexItem()
{
  t << "</a> &nbsp; ";
}

void HtmlGenerator::writeFooter(int part,bool external)
{
  switch (part)
  {
    case 0:
      if (footer.length()==0)
        t << "<hr><address><small>";
      else
        t << substitute(
               substitute(  
                 substitute(footer,"$title",lastTitle),
                 "$datetime",dateToString(TRUE)
               ),
               "$date",dateToString(FALSE)
             );
      break;
    case 1:
      if (footer.length()==0)
      {
        t << " <a href=\"http://www.stack.nl/~dimitri/doxygen/index.html\">";
        t << "<img ";
        if (external)
        {
          t << "doxygen=\"_doc:\" src=\"/";
        }
        else
        {
          t << "src=\"";
        }
        t << "doxygen.gif\" alt=\"doxygen\" align=center border=0 "
             "width=118 height=53></a>&nbsp; ";
      }
      break;
    default:
      if (footer.length()==0)
        t << " <a href=\"mailto:dimitri@stack.nl\">Dimitri van Heesch</a>, &copy; "
             "1997-1999</small></address>\n</body>\n</html>\n";
      break;
      
  }
}

void HtmlGenerator::endFile()
{
  endPlainFile();
}

void HtmlGenerator::startProjectNumber()
{
  t << "<h3 align=center>";
}

void HtmlGenerator::endProjectNumber()
{
  t << "</h3>";
}

void HtmlGenerator::writeStyleInfo(int part)
{
  if (part==0)
  {
    startPlainFile("doxygen.css"); 
    t << "H1 { border-width: thin; border: solid; text-align: center }" << endl
      << "A.el { text-decoration: none; font-weight: bold }" << endl
      << "DL.el { margin-left: -1cm }" << endl
      << "DIV.fragment { width: 100%; border: none; background-color: #eeeeee }" << endl
      << "DIV.in { margin-left: 16 }" << endl
      << endl;
    endPlainFile();
  }
}

void HtmlGenerator::writeDoxyAnchor(const char *,const char *anchor, const char *name)
{
  t << "<a name=\"" << anchor << "\" doxytag=\"" << name << "\"></a>";
}

void HtmlGenerator::newParagraph()
{
  t << endl << "<p>" << endl;
}

void HtmlGenerator::writeString(const char *text)
{
  t << text;
}

void HtmlGenerator::writeIndexItem(const char *ref,const char *f,
                                   const char *name)
{
  t << "<li>";
  if (ref || f)
  {
    t << "<a class=\"el\" ";
    if (ref) t << "doxygen=\"" << ref << ":\" ";
    t << "href=\"";
    if (f) t << f << ".html\">";
  }
  else
  {
    t << "<b>";
  }
  docify(name);
  if (ref || f)
  {
    t << "</a>" << endl;
  }
  else
  {
    t << "</b>";
  }
}

void HtmlGenerator::writeStartAnnoItem(const char *,const char *f,
                                       const char *path,const char *name)
{
  t << "<li>";
  if (path) docify(path);
  t << "<a class=\"el\" href=\"" << f << ".html\">";
  docify(name);
  t << "</a> ";
}

void HtmlGenerator::writeObjectLink(const char *ref,const char *f,
                                    const char *anchor, const char *name)
{
  t << "<a class=\"el\" ";
  if (ref) t << "doxygen=\"" << ref << ":\" ";
  t << "href=\"";
  if (f) t << f << ".html";
  if (anchor) t << "#" << anchor;
  t << "\">";
  docify(name);
  t << "</a>";
}

void HtmlGenerator::startTextLink(const char *f,const char *anchor)
{
  t << "<a href=\"";
  if (f)   t << f << ".html";
  if (anchor) t << "#" << anchor;
  t << "\">"; 
}

void HtmlGenerator::endTextLink()
{
  t << "</a>";
}

void HtmlGenerator::writeHtmlLink(const char *url,const char *text)
{
  t << "<a href=\"";
  if (url) t << url;
  t << "\">"; 
  docify(text);
  t << "</a>";
}

void HtmlGenerator::startGroupHeader()
{
  t << "<h2>";
}

void HtmlGenerator::endGroupHeader()
{
  t << "</h2>" << endl;
}

void HtmlGenerator::writeSection(const char *lab,const char *title,bool sub)
{
  t << "<a name=\"" << lab << "\">";
  if (sub) t << "<h3>"; else t << "<h2>";
  docify(title);
  if (sub) t << "</h3>"; else t << "</h2>";
  t << "</a>" << endl;
}

void HtmlGenerator::writeSectionRef(const char *name,const char *lab,
                                    const char *title)
{
  QString refName=name;
  if (refName.right(5)!=".html") refName+=".html";
  t << "<a href=\"" << refName << "#" << lab << "\">";
  docify(title);
  t << "</a>";
}

void HtmlGenerator::writeSectionRefItem(const char *name,const char *lab,
                                    const char *title)
{
  QString refName=name;
  if (refName.right(5)!=".html") refName+=".html";
  t << "<a href=\"" << refName << "#" << lab << "\">";
  docify(title);
  t << "</a>";
}

void HtmlGenerator::writeSectionRefAnchor(const char *name,const char *lab,
                                          const char *title)
{
  writeSectionRef(name,lab,title);
}

void HtmlGenerator::docify(const char *str)
{
  if (str)
  {
    const char *p=str;
    char c;
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '<':  t << "&lt;"; break;
        case '>':  t << "&gt;"; break;
        case '&':  t << "&amp;"; break;
        case '\\':
                   if (*p=='<')
                     { t << "&lt;"; p++; }
                   else if (*p=='>')
                     { t << "&gt;"; p++; }
                   else
                     t << "\\";
                   break;
        default:   t << c; 
      }
    }
  }
}

void HtmlGenerator::codify(const char *str)
{
  docify(str);
}

void HtmlGenerator::writeChar(char c)
{
  char cs[2];
  cs[0]=c;
  cs[1]=0;
  docify(cs);
}

void HtmlGenerator::startClassDiagram()
{
  t << "<p>";
}

void HtmlGenerator::endClassDiagram(ClassDiagram &d,
                                const char *fileName,const char *name)
{
  t << ":\n<p><center><img src=\""
    << fileName << ".gif\" usemap=\"#" << name << "_map\""
    << " border=\"0\"></center>" << endl
    << "<map name=\"" << name << "_map\"><p>" << endl;

  d.writeImageMap(t,dir,fileName);
}

void HtmlGenerator::startColorFont(uchar red,uchar green,uchar blue)
{
  QString colorString;
  colorString.sprintf("%02x%02x%02x",red,green,blue);
  t << "<font color=\"#" << colorString << "\">";
}

void HtmlGenerator::endColorFont()
{
  t << "</font>";
}

void HtmlGenerator::writeFormula(const char *n,const char *text)
{
  if (text && text[0]=='\\') t << "<p><center>" << endl;
  t << "<img align=\"top\" src=\"" << n << "\">" << endl;
  if (text && text[0]=='\\') t << "</center></p>" << endl;
}

void HtmlGenerator::startMemberList()  
{ 
  if (htmlAlignMemberFlag)
  {
    //t << endl << "<p><table border=0 cellspacing=2 cellpadding=0>" << endl; 
  }
  else
  {
    t << "<ul>" << endl; 
  }
}

void HtmlGenerator::endMemberList()    
{ 
  if (htmlAlignMemberFlag)
  {
    //t << "</table>" << endl; 
  }
  else
  {
    t << "</ul>" << endl; 
  }
}

void HtmlGenerator::startMemberItem() 
{ 
  if (htmlAlignMemberFlag)
  {
    t << "<tr><td align=right valign=top>"; 
  }
  else
  {
    t << "<li>"; 
  }
}

void HtmlGenerator::insertMemberAlign() 
{ 
  if (htmlAlignMemberFlag)
  {
    t << "</td><td valign=top>"; 
  }
}

void HtmlGenerator::endMemberItem() 
{ 
  if (htmlAlignMemberFlag)
  {
    t << "</td></tr>"; 
  }
  t << endl; 
}

void HtmlGenerator::startMemberDescription() 
{ 
  if (htmlAlignMemberFlag)
  {
    t << "<tr><td></td><td><font size=-1><em>"; 
  }
  else
  {
    t << "<dl class=\"el\"><dd><font size=-1><em>";
  }
}

void HtmlGenerator::endMemberDescription()   
{ 
  if (htmlAlignMemberFlag)
  {
    t << "</em></font><br><br></td></tr>" << endl; 
  }
  else
  {
    t << "</em></font></dl>";
  }
}

void HtmlGenerator::startMemberSections()
{
  if (htmlAlignMemberFlag)
  {
    t << "<table border=0 cellpadding=0 cellspacing=1>" << endl;
  }
}

void HtmlGenerator::endMemberSections()
{
  if (htmlAlignMemberFlag)
  {
    t << "</table>" << endl;
  }
}

void HtmlGenerator::startMemberHeader()
{
  if (htmlAlignMemberFlag)
  {
    t << "<tr><td colspan=2><br><h2>";
  }
  else
  {
    startGroupHeader();
  }
}

void HtmlGenerator::endMemberHeader()
{
  if (htmlAlignMemberFlag)
  {
    t << "</h2></td></tr>" << endl;
  }
  else
  {
    endGroupHeader();
  }
}

