/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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

#include "qtbc.h"
#include <qdir.h>
#include "message.h"
#include "htmlgen.h"
#include "config.h"
#include "util.h"
#include "doxygen.h"
#include "logos.h"
#include "diagram.h"
#include "version.h"
#include "dot.h"
#include "language.h"
#include "htmlhelp.h"
#include "docparser.h"
#include "htmldocvisitor.h"

// #define GROUP_COLOR "#ff8080"

//#define DBG_HTML(x) x;
#define DBG_HTML(x) 


/*
    changed default stylesheet, startIndexKey(), startIndexValue()
    02 jan 2002, jh
*/

static const char *defaultStyleSheet = 
"H1 { text-align: center; }\n"
"CAPTION { font-weight: bold }\n"
"A.qindex {}\n"
"A.qindexRef {}\n"
"A.el { text-decoration: none; font-weight: bold }\n"
"A.elRef { font-weight: bold }\n"
"A.code { text-decoration: none; font-weight: normal; color: #4444ee }\n"
"A.codeRef { font-weight: normal; color: #4444ee }\n"
"A:hover { text-decoration: none; background-color: #f2f2ff }\n"
"DL.el { margin-left: -1cm }\n"
"DIV.fragment { width: 100%; border: none; background-color: #eeeeee }\n"
"DIV.ah { background-color: black; font-weight: bold; color: #ffffff; margin-bottom: 3px; margin-top: 3px }\n"
"TD.md { background-color: #f2f2ff; font-weight: bold; }\n"
"TD.mdname1 { background-color: #f2f2ff; font-weight: bold; color: #602020; }\n"
"TD.mdname { background-color: #f2f2ff; font-weight: bold; color: #602020; width: 600px; }\n"
"DIV.groupHeader { margin-left: 16px; margin-top: 12px; margin-bottom: 6px; font-weight: bold }\n"
"DIV.groupText { margin-left: 16px; font-style: italic; font-size: smaller }\n"
"BODY { background: white }\n"
"TD.indexkey { \n"
"   background-color: #eeeeff; \n"
"   font-weight: bold; \n"
"   padding-right  : 10px; \n"
"   padding-top    : 2px; \n"
"   padding-left   : 10px; \n"
"   padding-bottom : 2px; \n"
"   margin-left    : 0px; \n"
"   margin-right   : 0px; \n"
"   margin-top     : 2px; \n"
"   margin-bottom  : 2px  \n"
"}\n"
"TD.indexvalue { \n"
"   background-color: #eeeeff; \n"
"   font-style: italic; \n"
"   padding-right  : 10px; \n"
"   padding-top    : 2px; \n"
"   padding-left   : 10px; \n"
"   padding-bottom : 2px; \n"
"   margin-left    : 0px; \n"
"   margin-right   : 0px; \n"
"   margin-top     : 2px; \n"
"   margin-bottom  : 2px  \n"
"}\n"
"span.keyword       { color: #008000 }\n"
"span.keywordtype   { color: #604020 }\n"
"span.keywordflow   { color: #e08000 }\n"
"span.comment       { color: #800000 }\n"
"span.preprocessor  { color: #806020 }\n"
"span.stringliteral { color: #002080 }\n"
"span.charliteral   { color: #008080 }\n";


static QCString g_header;
static QCString g_footer;

HtmlGenerator::HtmlGenerator() : OutputGenerator()
{
  dir=Config_getString("HTML_OUTPUT");
  col=0;  
}

HtmlGenerator::~HtmlGenerator()
{
  //printf("HtmlGenerator::~HtmlGenerator()\n");
}

void HtmlGenerator::append(const OutputGenerator *g)
{
  t << g->getContents();
  col+=((HtmlGenerator *)g)->col;
}

void HtmlGenerator::init()
{
  QCString dname=Config_getString("HTML_OUTPUT");
  QDir d(dname);
  if (!d.exists() && !d.mkdir(dname))
  {
    err("Could not create output directory %s\n",dname.data());
    exit(1);
  }
  writeLogo(dname);
  if (!Config_getString("HTML_HEADER").isEmpty()) g_header=fileToString(Config_getString("HTML_HEADER"));
  if (!Config_getString("HTML_FOOTER").isEmpty()) g_footer=fileToString(Config_getString("HTML_FOOTER"));
}

void HtmlGenerator::writeStyleSheetFile(QFile &file)
{
  QTextStream t(&file);
  t << defaultStyleSheet;
}

static void writeDefaultHeaderFile(QTextStream &t, const char *title,
                                   bool external)
{
  t << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
    "<html><head>" /*"<meta name=\"robots\" content=\"noindex\">\n"*/
    "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=" 
         << theTranslator->idLanguageCharset() << "\">\n"
    "<title>"; 
  t << convertToHtml(title);
  t << "</title>\n";
  t << "<link ";
  if (external) 
    t << "doxygen=\"_doc:" << Config_getString("DOC_URL") 
      << "\" href=\"" << Config_getString("DOC_URL") << "/";
  else
    t << "href=\"";
  if (Config_getString("HTML_STYLESHEET").isEmpty())
  {
    t << "doxygen.css";
  }
  else
  {
    QCString cssname=Config_getString("HTML_STYLESHEET");
    QFileInfo cssfi(cssname);
    if (!cssfi.exists())
    {
      err("Error: user specified HTML style sheet file does not exist!\n");
    }
    t << cssfi.fileName();
  }
  t << "\" rel=\"stylesheet\" type=\"text/css\">\n"
    "</head><body>\n";
}


void HtmlGenerator::writeHeaderFile(QFile &file)
{
  QTextStream t(&file);
#if QT_VERSION >= 200
  t.setEncoding(QTextStream::Latin1);
#endif
  writeDefaultHeaderFile(t,"$title",FALSE);
}

void HtmlGenerator::writeFooterFile(QFile &file)
{
  QTextStream t(&file);
  t << "<hr><address style=\"align: right;\"><small>\n";
  t << theTranslator->trGeneratedAt( "$datetime", "$projectname" );
  t << " <a href=\"http://www.doxygen.org/index.html\">\n"
    << "<img src=\"doxygen.png\" alt=\"doxygen\" " 
    << "align=\"middle\" border=0 width=110 height=53>\n"
    << "</a> $doxygenversion";
  //  << " " << theTranslator->trWrittenBy()
  //  << " <a href=\"mailto:dimitri@stack.nl\">Dimitri van Heesch</a>,\n"
  //  << " &copy;&nbsp;1997-2002"
  t << "</small></address>\n"
    << "</body>\n"
    << "</html>\n";
}


void HtmlGenerator::startFile(const char *name,const char *,
                              const char *title,bool external)
{
  //printf("HtmlGenerator::startFile(%s)\n",name);
  QCString fileName=name;
  lastTitle=title;
  if (fileName.right(Doxygen::htmlFileExtension.length())!=Doxygen::htmlFileExtension) 
  {
    fileName+=Doxygen::htmlFileExtension;
  }
  startPlainFile(fileName);
  if (Config_getBool("GENERATE_HTMLHELP"))
  {
    HtmlHelp::getInstance()->addIndexFile(fileName);
  }
 
  lastFile = fileName;
  if (g_header.isEmpty()) 
  {
    writeDefaultHeaderFile(t,title,external);
  }
  else
  {
    t << substituteKeywords(g_header,convertToHtml(lastTitle));
  }
  t << "<!-- " << theTranslator->trGeneratedBy() << " Doxygen " 
    << versionString << " -->" << endl;
}

void HtmlGenerator::startQuickIndexItem(const char *s,const char *l)
{
  QCString *dest;
  if (s) 
  {
    t << "<a class=\"qindexRef\" ";
    t << "doxygen=\"" << s << ":";
    if ((dest=Doxygen::tagDestinationDict[s])) t << *dest;
    if (strcmp(s,"_cgi")!=0) t << "/"; // small hack to get the cgi binary link right
    t << "\" ";
  }
  else
  {
    t << "<a class=\"qindex\" ";
  }
  t << "href=\""; 
  if (s)
  {
    if ((dest=Doxygen::tagDestinationDict[s])) t << *dest;
    if (strcmp(s,"_cgi")!=0) t << "/";
  }
  t << l << "\">";
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
      if (g_footer.isEmpty())
        t << "<hr><address style=\"align: right;\"><small>";
      else
        t << substituteKeywords(g_footer,convertToHtml(lastTitle));
      break;
    case 1:
      if (g_footer.isEmpty())
      {
        t << endl << "<a href=\"http://www.doxygen.org/index.html\">";
        t << endl << "<img ";
        if (external)
        {
          t << "doxygen=\"_doc:" << Config_getString("DOC_URL") 
            << "\" src=\"" << Config_getString("DOC_URL") << "/";
        }
        else
        {
          t << "src=\"";
        }
        t << "doxygen.png\" alt=\"doxygen\" " 
          << "align=\"middle\" border=0 " << endl << 
             "width=110 height=53></a>" << versionString <<" ";
      }
      break;
    default:
      if (g_footer.isEmpty())
      {
        //t << " <a href=\"mailto:dimitri@stack.nl\">Dimitri van Heesch</a>,\n &copy;&nbsp;1997-2002";
        t << "</small></address>\n</body>\n</html>\n";
      }
      break;
      
  }
}

void HtmlGenerator::endFile()
{
  endPlainFile();
}

void HtmlGenerator::startProjectNumber()
{
  t << "<h3 align=\"center\">";
}

void HtmlGenerator::endProjectNumber()
{
  t << "</h3>";
}

void HtmlGenerator::writeStyleInfo(int part)
{
  if (part==0)
  {
    if (Config_getString("HTML_STYLESHEET").isEmpty()) // write default style sheet
    {
      startPlainFile("doxygen.css"); 
      
      // alternative, cooler looking titles
      //t << "H1 { text-align: center; border-width: thin none thin none;" << endl;
      //t << "     border-style : double; border-color : blue; padding-left : 1em; padding-right : 1em }" << endl;

      t << defaultStyleSheet;
      endPlainFile();
    }
    else // write user defined style sheet
    {
      QCString cssname=Config_getString("HTML_STYLESHEET");
      QFileInfo cssfi(cssname);
      if (!cssfi.exists() || !cssfi.isFile() || !cssfi.isReadable())
      {
        err("Error: style sheet %s does not exist or is not readable!", Config_getString("HTML_STYLESHEET").data());
      }
      else
      {
        // convert style sheet to string
        QCString fileStr = fileToString(cssname);
        // write the string into the output dir
        startPlainFile(cssfi.fileName());
        t << fileStr;
        endPlainFile();
      }
    }
  }
}

void HtmlGenerator::startDoxyAnchor(const char *,const char *,
                                    const char *anchor, const char *name)
{
  t << "<a name=\"" << anchor << "\" doxytag=\"" << name << "\"></a>";
}

void HtmlGenerator::endDoxyAnchor(const char *,const char *)
{
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
  //printf("HtmlGenerator::writeIndexItem(%s,%s,%s)\n",ref,f,name);
  QCString *dest;
  t << "<li>";
  if (ref || f)
  {
    if (ref) 
    {
      t << "<a class=\"elRef\" ";
      t << "doxygen=\"" << ref << ":";
      if ((dest=Doxygen::tagDestinationDict[ref])) t << *dest << "/";
      t << "\" ";
    }
    else
    {
      t << "<a class=\"el\" ";
    }
    t << "href=\"";
    if (ref)
    {
      if ((dest=Doxygen::tagDestinationDict[ref])) t << *dest << "/";
    }
    if (f) t << f << Doxygen::htmlFileExtension << "\">";
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
  //if (Config_getBool("GENERATE_HTMLHELP") && f)
  //{
  //  htmlHelp->addItem(name,((QCString)f)+htmlFileExtension);
  //}
}

void HtmlGenerator::writeStartAnnoItem(const char *,const char *f,
                                       const char *path,const char *name)
{
  t << "<li>";
  if (path) docify(path);
  t << "<a class=\"el\" href=\"" << f << Doxygen::htmlFileExtension << "\">";
  docify(name);
  t << "</a> ";
  //if (Config_getBool("GENERATE_HTMLHELP") && f)
  //{
  //  htmlHelp->addItem(name, ((QCString)f)+htmlFileExtension);
  //}
}

void HtmlGenerator::writeObjectLink(const char *ref,const char *f,
                                    const char *anchor, const char *name)
{
  QCString *dest;
  if (ref) 
  {
    t << "<a class=\"elRef\" ";
    t << "doxygen=\"" << ref << ":";
    if ((dest=Doxygen::tagDestinationDict[ref])) t << *dest << "/";
    t << "\" ";
  }
  else
  {
    t << "<a class=\"el\" ";
  }
  t << "href=\"";
  if (ref)
  {
    if ((dest=Doxygen::tagDestinationDict[ref])) t << *dest << "/";
  }
  if (f) t << f << Doxygen::htmlFileExtension;
  if (anchor) t << "#" << anchor;
  t << "\">";
  docify(name);
  t << "</a>";
}

void HtmlGenerator::writeCodeLink(const char *ref,const char *f,
                                  const char *anchor, const char *name)
{
  QCString *dest;
  if (ref) 
  {
    t << "<a class=\"codeRef\" ";
    t << "doxygen=\"" << ref << ":";
    if ((dest=Doxygen::tagDestinationDict[ref])) t << *dest << "/";
    t << "\" ";
  }
  else
  {
    t << "<a class=\"code\" ";
  }
  t << "href=\"";
  if (ref)
  {
    if ((dest=Doxygen::tagDestinationDict[ref])) t << *dest << "/";
  }
  if (f) t << f << Doxygen::htmlFileExtension;
  if (anchor) t << "#" << anchor;
  t << "\">";
  docify(name);
  t << "</a>";
  col+=strlen(name);
}

void HtmlGenerator::startTextLink(const char *f,const char *anchor)
{
  t << "<a href=\"";
  if (f)   t << f << Doxygen::htmlFileExtension;
  if (anchor) t << "#" << anchor;
  t << "\">"; 
}

void HtmlGenerator::endTextLink()
{
  t << "</a>";
}

void HtmlGenerator::startHtmlLink(const char *url)
{
  t << "<a ";
  if (Config_getBool("GENERATE_TREEVIEW")) t << "target=\"top\" ";
  t << "href=\"";
  if (url) t << url;
  t << "\">"; 
}

void HtmlGenerator::endHtmlLink()
{
  t << "</a>";
}

//void HtmlGenerator::writeMailLink(const char *url)
//{
//  t << "<a href=\"mailto:" << url << "\">";
//  docify(url); 
//  t << "</a>";
//}

void HtmlGenerator::startGroupHeader()
{
  t << "<h2>";
}

void HtmlGenerator::endGroupHeader()
{
  t << "</h2>" << endl;
}

void HtmlGenerator::startSection(const char *lab,const char *,SectionInfo::SectionType type)
{
  switch(type)
  {
    case SectionInfo::Page:          t << "<h1>"; break;
    case SectionInfo::Section:       t << "<h2>"; break;
    case SectionInfo::Subsection:    t << "<h3>"; break;
    case SectionInfo::Subsubsection: t << "<h4>"; break;
    case SectionInfo::Paragraph:     t << "<h5>"; break;
    default: ASSERT(0); break;
  }
  t << "<a name=\"" << lab << "\">";
}

void HtmlGenerator::endSection(const char *,SectionInfo::SectionType type)
{
  t << "</a>" << endl;
  switch(type)
  {
    case SectionInfo::Page:          t << "</h1>"; break;
    case SectionInfo::Section:       t << "</h2>"; break;
    case SectionInfo::Subsection:    t << "</h3>"; break;
    case SectionInfo::Subsubsection: t << "</h4>"; break;
    case SectionInfo::Paragraph:     t << "</h5>"; break;
    default: ASSERT(0); break;
  }
}

//void HtmlGenerator::writeSectionRef(const char *ref,const char *name,
//                                    const char *anchor,const char *title)
//{
//  QCString *dest;
//  //printf("writeSectionRef(%s,%s,%s,%s)\n",ref,name,anchor,title);
//  QCString refName=name;
//  if (refName.right(Doxygen::htmlFileExtension.length())!=Doxygen::htmlFileExtension) 
//  {
//    refName+=Doxygen::htmlFileExtension;
//  }
//  t << "<a "; 
//  if (ref) 
//  {
//    t << "doxygen=\"" << ref << ":";
//    if ((dest=Doxygen::tagDestinationDict[ref])) t << *dest << "/";
//    t << "\" ";
//  }
//  t << "href=\""; 
//  if (ref)
//  {
//    if ((dest=Doxygen::tagDestinationDict[ref])) t << *dest << "/";
//  }
//  t << refName << "#" << anchor << "\">";
//  docify(title);
//  t << "</a>";
//}
//
//void HtmlGenerator::writeSectionRefItem(const char *name,const char *lab,
//                                    const char *title)
//{
//  QCString refName=name;
//  if (refName.right(Doxygen::htmlFileExtension.length())!=Doxygen::htmlFileExtension) 
//  {
//    refName+=Doxygen::htmlFileExtension;
//  }
//  t << "<li><a href=\"" << refName << "#" << lab << "\">";
//  docify(title);
//  t << "</a>";
//}
//
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
  //docify(str);
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
        case '\t': spacesToNextTabStop = 
                         Config_getInt("TAB_SIZE") - (col%Config_getInt("TAB_SIZE")); 
                   t << spaces.left(spacesToNextTabStop); 
                   col+=spacesToNextTabStop; 
                   break; 
        case '\n': t << '\n'; col=0; 
                   break;
        case '\r': break;
        case '<':  t << "&lt;"; col++; 
                   break;
        case '>':  t << "&gt;"; col++; 
                   break;
        case '&':  t << "&amp;"; col++; 
                   break;
        case '\\':
                   if (*p=='<')
                     { t << "&lt;"; p++; }
                   else if (*p=='>')
                     { t << "&gt;"; p++; }
                   else
                     t << "\\";
                   col++;
                   break;
        default:   t << c;    
                   col++;                    
                   break;
      }
    }
  }
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
  t << "\n<p><center><img src=\""
    << fileName << ".png\" usemap=\"#" << name << "_map\""
    << " border=\"0\" alt=\"\"></center>" << endl
    << "<map name=\"" << name << "_map\">" << endl;

  d.writeImage(t,dir,fileName);
}

//void HtmlGenerator::startColorFont(uchar red,uchar green,uchar blue)
//{
//  QCString colorString;
//  colorString.sprintf("%02x%02x%02x",red,green,blue);
//  t << "<font color=\"#" << colorString << "\">";
//}
//
//void HtmlGenerator::endColorFont()
//{
//  t << "</font>";
//}

//void HtmlGenerator::writeFormula(const char *n,const char *text)
//{
//  if (text && text[0]=='\\') t << "<p><center>" << endl;
//  t << "<img align=";
//#if !defined(_WIN32)
//  t << "\"top\"";            // assume Unix users use Netscape 4.x which does
//                             // not seem to support align == "middle" :-((
//#else
//  t << "\"middle\"";         // assume Windows users use IE or HtmlHelp which only
//                             // displays formulas nicely with align == "middle" 
//#endif
//  t << " src=\"" << n << ".png\">" << endl;
//  if (text && text[0]=='\\') t << "</center><p>" << endl;
//}

void HtmlGenerator::startMemberList()  
{ 
  DBG_HTML(t << "<!-- startMemberList -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
  }
  else
  {
    t << "<ul>" << endl; 
  }
}

void HtmlGenerator::endMemberList()    
{ 
  DBG_HTML(t << "<!-- endMemberList -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
  }
  else
  {
    t << "</ul>" << endl; 
  }
}

// annonymous type:
//  0 = single column right aligned
//  1 = double column left aligned
//  2 = single column left aligned
void HtmlGenerator::startMemberItem(int annoType) 
{ 
  DBG_HTML(t << "<!-- startMemberItem() -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "<tr>";
    switch(annoType)
    {
      case 0:  t << "<td nowrap align=right valign=top>"; break;
      case 1:  t << "<td nowrap>"; break;
      default: t << "<td nowrap valign=top>"; break;
    }
  }
  else
  {
    t << "<li>"; 
  }
}

void HtmlGenerator::endMemberItem(bool) 
{ 
  //DBG_HTML(t << "<!-- endMemberItem(" << (int)inGroup << "," << fileName << "," << headerName << " -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "</td></tr>"; 
  }
  t << endl; 
}


void HtmlGenerator::insertMemberAlign() 
{ 
  DBG_HTML(t << "<!-- insertMemberAlign -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "&nbsp;</td><td valign=bottom>"; 
  }
}

void HtmlGenerator::startMemberDescription() 
{ 
  DBG_HTML(t << "<!-- startMemberDescription -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "<tr><td>&nbsp;</td><td><font size=-1><em>"; 
  }
  else
  {
    t << "<dl class=\"el\"><dd><font size=-1><em>";
  }
}

void HtmlGenerator::endMemberDescription()   
{ 
  DBG_HTML(t << "<!-- endMemberDescription -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "</em></font><br><br></td></tr>" << endl; 
  }
  else
  {
    t << "<br><br></em></font></dl>";
  }
}

void HtmlGenerator::startMemberSections()
{
  DBG_HTML(t << "<!-- startMemberSections -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "<table border=0 cellpadding=0 cellspacing=0>" << endl;
    // HTML is not recursively decomposable, sorry
    t << "<tr><td></td></tr>" << endl;
  }
}

void HtmlGenerator::endMemberSections()
{
  DBG_HTML(t << "<!-- endMemberSections -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "</table>" << endl;
  }
}

void HtmlGenerator::startMemberHeader()
{
  DBG_HTML(t << "<!-- startMemberHeader -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
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
  DBG_HTML(t << "<!-- endMemberHeader -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "</h2></td></tr>" << endl;
  }
  else
  {
    endGroupHeader();
  }
}

void HtmlGenerator::startMemberSubtitle()
{
  DBG_HTML(t << "<!-- startMemberSubtitle -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS")) t << "<tr><td colspan=2>";
}

void HtmlGenerator::endMemberSubtitle()
{
  DBG_HTML(t << "<!-- endMemberSubtitle -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS")) t << "<br><br></td></tr>" << endl;
}

void HtmlGenerator::startIndexList() 
{ 
  t << "<table>"  << endl; 
}

void HtmlGenerator::endIndexList()
{
  t << "</table>" << endl;
}

void HtmlGenerator::startIndexKey() 
{ 
  // inserted 'class = ...', 02 jan 2002, jh
  t << "  <tr><td class=\"indexkey\">"; 
}

void HtmlGenerator::endIndexKey()
{
  t << "</td>";
}

void HtmlGenerator::startIndexValue(bool) 
{ 
  // inserted 'class = ...', 02 jan 2002, jh
  t << "<td class=\"indexvalue\">"; 
}

void HtmlGenerator::endIndexValue(const char *,bool)
{
  t << "</td></tr>" << endl;
}

//void HtmlGenerator::startAlphabeticalIndexList()
//{
//  t << "<table align=center width=\"95%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">" << endl;
//}

//void HtmlGenerator::endAlphabeticalIndexList()
//{
//  t << "</table>" << endl;
//}

//void HtmlGenerator::writeIndexHeading(const char *s)
//{
//  t << "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td><div class=\"ah\">&nbsp;&nbsp;" << s 
//    << "&nbsp;&nbsp;</td></tr></table>";
//}

//void HtmlGenerator::startImage(const char *name,const char *,bool hasCaption)
//{
//  QCString baseName=name;
//  int i;
//  if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
//  {
//    baseName=baseName.right(baseName.length()-i-1); 
//  }
//  t << "<div align=\"center\">" << endl;
//  t << "<img src=\"" << name << "\" alt=\"" << baseName << "\">" << endl;
//  if (hasCaption)
//  {
//    t << "<p><strong>";
//  } 
//}

//void HtmlGenerator::endImage(bool hasCaption)
//{
//  if (hasCaption)
//  {
//    t << "</strong></p>" << endl;
//  }
//  t << "</div>" << endl;
//}

//void HtmlGenerator::startDotFile(const char *name,bool hasCaption)
//{
//  QCString baseName=name;
//  int i;
//  if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
//  {
//    baseName=baseName.right(baseName.length()-i-1); 
//  }
//  QCString outDir = Config_getString("HTML_OUTPUT");
//  writeDotGraphFromFile(name,outDir,baseName,BITMAP);
//  t << "<div align=\"center\">" << endl;
//  t << "<img src=\"" << baseName << "." 
//    << Config_getEnum("DOT_IMAGE_FORMAT") << "\" alt=\"" 
//    << baseName << "\">" << endl;
//  if (hasCaption)
//  {
//    t << "<p><strong>";
//  } 
//}
//
//void HtmlGenerator::endDotFile(bool hasCaption)
//{
//  if (hasCaption)
//  {
//    t << "</strong></p>" << endl;
//  }
//  t << "</div>" << endl;
//}
//
void HtmlGenerator::startMemberDoc(const char *,const char *,const char *,const char *) 
{ 
  DBG_HTML(t << "<!-- startMemberDoc -->" << endl;)
  t << "<p>" << endl;
  t << "<table width=\"100%\" cellpadding=\"2\" cellspacing=\"0\" border=\"0\">" << endl;
  t << "  <tr>" << endl;
  t << "    <td class=\"md\">" << endl; 
  t << "      <table cellpadding=\"0\" cellspacing=\"0\" border=\"0\">" << endl;
}

void HtmlGenerator::startMemberDocPrefixItem()
{
  DBG_HTML(t << "<!-- startMemberDocPrefixItem -->" << endl;)
  t << "        <tr>" << endl;
  t << "          <td class=\"md\" colspan=\"4\">" << endl; 
}

void HtmlGenerator::endMemberDocPrefixItem()
{
  DBG_HTML(t << "<!-- endMemberDocPrefixItem -->" << endl;)
  t << "</td>" << endl;
  t << "        </tr>" << endl;
}

void HtmlGenerator::startMemberDocName()
{
  DBG_HTML(t << "<!-- startMemberDocName -->" << endl;)
  t << "        <tr>" << endl;
  t << "          <td class=\"md\" nowrap valign=\"top\"> ";
}

void HtmlGenerator::endMemberDocName()
{
  DBG_HTML(t << "<!-- endMemberDocName -->" << endl;)
  t << "</td>" << endl;
}

void HtmlGenerator::startParameterList()
{
  DBG_HTML(t << "<!-- startParameterList -->" << endl;)
  t << "          <td class=\"md\" valign=\"top\">(&nbsp;</td>" << endl;
}

void HtmlGenerator::startParameterType(bool first)
{
  if (first)
  {
    DBG_HTML(t << "<!-- startFirstParameterType -->" << endl;)
    t << "          <td class=\"md\" nowrap valign=\"top\">";
  }
  else
  {
    DBG_HTML(t << "<!-- startParameterType -->" << endl;)
    t << "        <tr>" << endl;
    t << "          <td></td>" << endl;
    t << "          <td></td>" << endl;
    t << "          <td class=\"md\" nowrap>";
  }
}

void HtmlGenerator::endParameterType()
{
  DBG_HTML(t << "<!-- endParameterType -->" << endl;)
  t << "&nbsp;</td>" << endl;
}

void HtmlGenerator::startParameterName(bool oneArgOnly)
{
  DBG_HTML(t << "<!-- startParameterName -->" << endl;)
  t << "          <td class=\"mdname";
  if (oneArgOnly) 
  {
    t << "1\" valign=\"top";
  }
  t << "\" nowrap>&nbsp;";
}

void HtmlGenerator::endParameterName(bool last,bool emptyList)
{
  DBG_HTML(t << "<!-- endParameterName -->" << endl;)
  if (last)
  {
    if (emptyList)
    {
      t << "          </td>" << endl;
      t << "          <td class=\"md\" valign=\"top\">)&nbsp;</td>" << endl;
      t << "          <td class=\"md\" nowrap>";
    }
    else
    {
      t << "</td>" << endl;
      t << "        </tr>" << endl;
      t << "        <tr>" << endl;
      t << "          <td></td>" << endl;
      t << "          <td class=\"md\">)&nbsp;</td>" << endl;
      t << "          <td class=\"md\" colspan=\"2\">";
    }
  }
  else
  {
    t << "</td>" << endl;
    t << "        </tr>" << endl;
  }
}

void HtmlGenerator::endParameterList()
{
  DBG_HTML(t << "<!-- endParameterList -->" << endl;)
  t << "</td>" << endl;
  t << "        </tr>" << endl;
}

void HtmlGenerator::endMemberDoc()     
{ 
  DBG_HTML(t << "<!-- endMemberDoc -->" << endl;)
  t << endl;
  t << "      </table>" << endl;
  t << "    </td>" << endl;
  t << "  </tr>" << endl;
  t << "</table>" << endl; 
}

void HtmlGenerator::startDotGraph()
{
}

void HtmlGenerator::endDotGraph(DotClassGraph &g)
{
  g.writeGraph(t,BITMAP,Config_getString("HTML_OUTPUT"));
}

void HtmlGenerator::startInclDepGraph()
{
}

void HtmlGenerator::endInclDepGraph(DotInclDepGraph &g)
{
  g.writeGraph(t,BITMAP,Config_getString("HTML_OUTPUT"));
}

void HtmlGenerator::writeGraphicalHierarchy(DotGfxHierarchyTable &g)
{
  g.writeGraph(t,Config_getString("HTML_OUTPUT"));
}

void HtmlGenerator::startMemberGroupHeader(bool)
{
  t << "<tr><td colspan=\"2\"><div class=\"groupHeader\">";
}

void HtmlGenerator::endMemberGroupHeader()
{
  t << "</div></td></tr>" << endl;
}

void HtmlGenerator::startMemberGroupDocs()
{
  t << "<tr><td colspan=\"2\"><div class=\"groupText\">";
}

void HtmlGenerator::endMemberGroupDocs()
{
  t << "<br><br></div></td></tr>" << endl;
}

void HtmlGenerator::startMemberGroup()
{
}

void HtmlGenerator::endMemberGroup(bool)
{
}

void HtmlGenerator::startIndent()        
{ 
  // I really wanted to use CSS here to provide an indented section, but
  // alas, Netscape is buggy enough to sometimes "forget" to end the
  // indent cause a staircase effect where the indent continuously increases.
  // It's back to abusing tables :-(
  
  //t << "<div class=\"in\">" << endl; 

  t << "<table cellspacing=5 cellpadding=0 border=0>\n"
       "  <tr>\n"
       "    <td>\n"
       "      &nbsp;\n"
       "    </td>\n"
       "    <td>\n";
}

void HtmlGenerator::endIndent()          
{ 
  t << "    </td>\n"
       "  </tr>\n"
       "</table>\n";
  //t << "</div>" << endl; 
}

void HtmlGenerator::addIndexItem(const char *,const char *)
{
}

void HtmlGenerator::writeNonBreakableSpace(int n)
{
  int i;
  for (i=0;i<n;i++)
  {
    t << "&nbsp;";
  }
}

void HtmlGenerator::writeLineNumber(const char *ref,const char *file,
                                    const char *anchor,int l)
{
  QCString lineNumber,lineAnchor;
  lineNumber.sprintf("%05d",l);
  lineAnchor.sprintf("l%05d",l);

  if (file)
  {
    startCodeAnchor(lineAnchor);
    writeCodeLink(ref,file,anchor,lineNumber);
    endCodeAnchor();
  }
  else
  {
    codify(lineNumber);
  }
  codify(" ");
}

void HtmlGenerator::startSimpleSect(SectionTypes,
                                const char *file,const char *anchor,
                                const char *title)
{
  t << "<dl compact><dt><b>";
  if (file)
  {
    writeObjectLink(0,file,anchor,title);
  }
  else
  {
    docify(title);
  }
  t << "</b></dt>";
}

void HtmlGenerator::endSimpleSect()
{
  t << "</dl>"; 
}

void HtmlGenerator::startParamList(ParamListTypes,
                                const char *title)
{
  t << "<dl compact><dt><b>";
  docify(title);
  t << "</b></dt>";
}

void HtmlGenerator::endParamList()
{
  t << "</dl>"; 
}

//void HtmlGenerator::startSectionRefList()
//{
//  t << "<multicol cols=3>" << endl;
//  t << "<ul>" << endl;
//}

//void HtmlGenerator::endSectionRefList()
//{
//  t << "</ul>" << endl;
//  t << "</multicol>" << endl;
//}


void HtmlGenerator::printDoc(DocNode *n)
{
  HtmlDocVisitor *visitor = new HtmlDocVisitor(t,*this);
  n->accept(visitor);
  delete visitor; 
}

