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

//void HtmlGenerator::generateExternalIndex()
//{
//  //printf("Generating external index...\n");
//  QFile f;
//  f.setName(htmlOutputDir+"/header.html");
//  if (f.open(IO_WriteOnly))
//  {
//    QTextStream tt(&f);
//    if (header.length()==0) 
//    {
//      tt << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
//        "<html><head><meta name=\"robots\" content=\"noindex\">\n"
//        "<title>Doxysearch</title>\n"
//        "</head><body bgcolor=\"#ffffff\">\n";
//    }
//    else
//    {
//      tt << header;
//    }
//    if (!noIndexFlag)
//    {
//      tt << "<center>";
//      if (documentedGroups>0)
//        tt << "<a doxygen=\"_doc:\" href=\"/modules.html\">Modules</a> &nbsp; ";
//      if (hierarchyClasses>0)
//        tt << "<a doxygen=\"_doc:\" href=\"/hierarchy.html\">Class hierarchy</a> &nbsp; ";
//      if (annotatedClasses>0)
//        tt << "<a doxygen=\"_doc:\" href=\"/annotated.html\">Compound list</a> &nbsp; "; 
//      if (documentedFiles>0)
//        tt << "<a doxygen=\"_doc:\" href=\"/files.html\">File list</a> &nbsp; ";
//      if (includeFiles.count()>0)
//        tt << "<a doxygen=\"_doc:\" href=\"/headers.html\">Header files</a> &nbsp; "; 
//      if (documentedMembers>0)
//        tt << "<a doxygen=\"_doc:\" href=\"/functions.html\">Compound Members</a> &nbsp; "; 
//      if (documentedFunctions>0)
//        tt << "<a doxygen=\"_doc:\" href=\"/globals.html\">File Members</a> &nbsp; ";
//      if (pageList.count()>0)
//        tt << "<a doxygen=\"_doc:\" href=\"/pages.html\">Related Pages</a> &nbsp; ";
//      if (exampleList.count()>0)
//        tt << "<a doxygen=\"_doc:\" href=\"/examples.html\">Examples</a> &nbsp; ";
//      if (searchEngineFlag)
//        tt << "<a doxygen=\"_cgi:\" href=\"\">Search</a>";
//      tt << endl << "</center><hr>" << endl;
//    }
//  }
//  f.close();
//  
//  f.setName(htmlOutputDir+"/footer.html");
//  if (f.open(IO_WriteOnly))
//  {
//    QTextStream tt(&f);
//    if (footer.length()==0)
//    {
//      tt << "<hr><address><small>Generated at " << dateToString(TRUE);
//      if (projectName.length()>0) tt << " for " << projectName; 
//      tt << " by <a href=\"http://www.stack.nl/~dimitri/doxygen/index.html\">"
//        "doxygen</a>&nbsp; written by <a href="
//        "\"mailto:dimitri@stack.nl\">Dimitri van Heesch</a>, &copy; "
//        "1997-1999</small></address>\n</body>\n</html>\n";
//    }
//    else
//    {
//      tt << footer;
//    }
//  }
//  f.close();
//}

void HtmlGenerator::startFile(const char *name,const char *title,bool external)
{
  QString fileName=name;
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
    t << header;
  }
  
//  if (!noIndexFlag)
//  {
//    t << "<center>";
//    if (documentedGroups>0)
//      t << "<a href=\"modules.html\">Modules</a> &nbsp; ";
//    if (hierarchyClasses>0)
//      t << "<a href=\"hierarchy.html\">Class hierarchy</a> &nbsp; ";
//    if (annotatedClasses>0)
//      t << "<a href=\"annotated.html\">Compound list</a> &nbsp; "; 
//    if (documentedFiles>0)
//      t << "<a href=\"files.html\">File list</a> &nbsp; ";
//    if (includeFiles.count()>0)
//      t << "<a href=\"headers.html\">Header files</a> &nbsp; "; 
//    if (documentedMembers>0)
//      t << "<a href=\"functions.html\">Compound Members</a> &nbsp; "; 
//    if (documentedFunctions>0)
//      t << "<a href=\"globals.html\">File Members</a> &nbsp; ";
//    if (pageList.count()>0)
//      t << "<a href=\"pages.html\">Related Pages</a> &nbsp; ";
//    if (exampleList.count()>0)
//      t << "<a href=\"examples.html\">Examples</a> &nbsp; ";
//    if (searchEngineFlag)
//      t << "<a doxygen=\"_cgi:\" href=\"\">Search</a>";
//    t << endl << "</center><hr>" << endl;
//  }
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
        t << footer;
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
//  if (footer.length()==0)
//  {
//    t << "<hr><address><small>Generated at " << dateToString(TRUE);
//    if (projectName.length()>0) 
//    {
//      t << " for ";
//      docify(projectName); 
//    }
//    t << " by <a href=\"http://www.stack.nl/~dimitri/doxygen/index.html\">"
//      "<img src=\"doxygen.gif\" alt=\"doxygen\" align=center border=0 "
//      "width=118 height=53></a>&nbsp; written by <a href="
//      "\"mailto:dimitri@stack.nl\">Dimitri van Heesch</a>, &copy; "
//      "1997-1999</small></address>\n</body>\n</html>\n";
//  }
//  else
//  {
//    t << footer;
//  }
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

//void HtmlGenerator::writeIndex()
//{
//  startFile("index.html","Main Index");
//  t << "<h1 align=center>"; 
//  docify(projectName);
//  t << " Documentation</h1>"
//    << "<p>" << endl << "<p>" << endl;
//  if (!projectNumber.isEmpty())
//  {
//    t << "<h3 align=center>";
//    docify(projectNumber);
//    t << "</h3>\n<p>\n<p>\n";
//  }
//  if (noIndexFlag)
//  {
//    t << "<ul>" << endl;
//    if (hierarchyClasses>0)
//      t << "<li><a href=\"hierarchy.html\">Class hierarchy</a>" << endl;
//    if (annotatedClasses>0)
//      t << "<li><a href=\"annotated.html\">Compound list</a>" << endl; 
//    if (documentedFiles>0)
//      t << "<li><a href=\"files.html\">File list</a>" << endl;
//    if (includeFiles.count()>0)
//      t << "<li><a href=\"headers.html\">Header files</a>" << endl; 
//    if (documentedMembers>0)
//      t << "<li><a href=\"functions.html\">Compound Members</a>" << endl; 
//    if (documentedFunctions>0)
//      t << "<li><a href=\"globals.html\">File Members</a>" << endl;
//    if (pageList.count()>0)
//      t << "<li><a href=\"pageList.html\">Related Pages</a>" << endl;
//    if (exampleList.count()>0)
//      t << "<li><a href=\"examples.html\">Examples</a>" << endl;
//    if (searchEngineFlag)
//      t << "<li><a doxygen=\"_cgi:\" href=\"\">Search</a>" << endl;
//    t << "</ul>" << endl;
//  }
//  endFile();
//}

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
  t << "&quot;<a href=\"" << refName << "#" << lab << "\">";
  docify(title);
  t << "</a>&quot;";
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

//void HtmlGenerator::docify(const char *str)
//{
//  docifyStatic(t,str);
//}

//void HtmlGenerator::docifyStatic(QTextStream &t,const char *str)
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
