/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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
#include "index.h"
#include "pagedef.h"
#include "debug.h"
#include "dirdef.h"

// #define GROUP_COLOR "#ff8080"

//#define DBG_HTML(x) x;
#define DBG_HTML(x) 

static const char *defaultStyleSheet = 
"BODY,H1,H2,H3,H4,H5,H6,P,CENTER,TD,TH,UL,DL,DIV {\n"
"	font-family: Geneva, Arial, Helvetica, sans-serif;\n"
"}\n"
"H1 {\n"
"	text-align: center;\n"
"}\n"
"CAPTION { font-weight: bold }\n"
"DIV.qindex {\n"
"	width: 100%;\n"
"	background-color: #eeeeff;\n"
"	border: 1px solid #b0b0b0;\n"
"	text-align: center;\n"
"	margin: 2px;\n"
"	padding: 2px;\n"
"	line-height: 140%;\n"
"}\n"
"DIV.nav {\n"
"	width: 100%;\n"
"	background-color: #eeeeff;\n"
"	border: 1px solid #b0b0b0;\n"
"	text-align: center;\n"
"	margin: 2px;\n"
"	padding: 2px;\n"
"	line-height: 140%;\n"
"}\n"
"A.qindex {\n"
"       text-decoration: none;\n"
"       font-weight: bold;\n"
"       color: #1A419D;\n"
"}\n"
"A.qindex:visited {\n"
"       text-decoration: none;\n"
"       font-weight: bold;\n"
"       color: #1A419D\n"
"}\n"
"A.qindex:hover {\n"
"	text-decoration: none;\n"
"	background-color: #ddddff;\n"
"}\n"
"A.qindexHL {\n"
"	text-decoration: none;\n"
"	font-weight: bold;\n"
"	background-color: #6666cc;\n"
"	color: #ffffff;\n"
"	border: 1px double #9295C2;\n"
"}\n"
"A.qindexHL:hover {\n"
"	text-decoration: none;\n"
"	background-color: #6666cc;\n"
"	color: #ffffff;\n"
"}\n"
"A.qindexHL:visited { text-decoration: none; background-color: #6666cc; color: #ffffff }\n"
"A.el { text-decoration: none; font-weight: bold }\n"
"A.elRef { font-weight: bold }\n"
"A.code:link { text-decoration: none; font-weight: normal; color: #0000FF}\n"
"A.code:visited { text-decoration: none; font-weight: normal; color: #0000FF}\n"
"A.codeRef:link { font-weight: normal; color: #0000FF}\n"
"A.codeRef:visited { font-weight: normal; color: #0000FF}\n"
"A:hover { text-decoration: none; background-color: #f2f2ff }\n"
"DL.el { margin-left: -1cm }\n"
".fragment {\n"
"       font-family: monospace\n"
"}\n"
"PRE.fragment {\n"
"	border: 1px solid #CCCCCC;\n"
"	background-color: #f5f5f5;\n"
"	margin-top: 4px;\n"
"	margin-bottom: 4px;\n"
"	margin-left: 2px;\n"
"	margin-right: 8px;\n"
"	padding-left: 6px;\n"
"	padding-right: 6px;\n"
"	padding-top: 4px;\n"
"	padding-bottom: 4px;\n"
"}\n"
//"DIV.fragment {\n"
//"       font-family: monospace\n"
//"	border: 1px solid #CCCCCC;\n"
//"	background-color: #f5f5f5;\n"
//"	padding: 6px;\n"
//"}\n"
"DIV.ah { background-color: black; font-weight: bold; color: #ffffff; margin-bottom: 3px; margin-top: 3px }\n"
"TD.md { background-color: #F4F4FB; font-weight: bold; }\n"
"TD.mdname1 { background-color: #F4F4FB; font-weight: bold; color: #602020; }\n"
"TD.mdname { background-color: #F4F4FB; font-weight: bold; color: #602020; width: 600px; }\n"
"DIV.groupHeader {\n"
"       margin-left: 16px;\n"
"       margin-top: 12px;\n"
"       margin-bottom: 6px;\n"
"       font-weight: bold;\n"
"}\n"
"DIV.groupText { margin-left: 16px; font-style: italic; font-size: 14px }\n"
"BODY {\n"
"	background: white;\n"
"	color: black;\n"
"	margin-right: 20px;\n"
"	margin-left: 20px;\n"
"}\n"
"TD.indexkey {\n"
"	background-color: #eeeeff;\n"
"	font-weight: bold;\n"
"	padding-right  : 10px;\n"
"	padding-top    : 2px;\n"
"	padding-left   : 10px;\n"
"	padding-bottom : 2px;\n"
"	margin-left    : 0px;\n"
"	margin-right   : 0px;\n"
"	margin-top     : 2px;\n"
"	margin-bottom  : 2px;\n"
"	border: 1px solid #CCCCCC;\n"
"}\n"
"TD.indexvalue {\n"
"	background-color: #eeeeff;\n"
"	font-style: italic;\n"
"	padding-right  : 10px;\n"
"	padding-top    : 2px;\n"
"	padding-left   : 10px;\n"
"	padding-bottom : 2px;\n"
"	margin-left    : 0px;\n"
"	margin-right   : 0px;\n"
"	margin-top     : 2px;\n"
"	margin-bottom  : 2px;\n"
"	border: 1px solid #CCCCCC;\n"
"}\n"
"TR.memlist {\n"
"   background-color: #f0f0f0; \n"
"}\n"
"P.formulaDsp { text-align: center; }\n"
"IMG.formulaDsp { }\n"
"IMG.formulaInl { vertical-align: middle; }\n"
"SPAN.keyword       { color: #008000 }\n"
"SPAN.keywordtype   { color: #604020 }\n"
"SPAN.keywordflow   { color: #e08000 }\n"
"SPAN.comment       { color: #800000 }\n"
"SPAN.preprocessor  { color: #806020 }\n"
"SPAN.stringliteral { color: #002080 }\n"
"SPAN.charliteral   { color: #008080 }\n"
".mdTable {\n"
"	border: 1px solid #868686;\n"
"	background-color: #F4F4FB;\n"
"}\n"
".mdRow {\n"
"	padding: 8px 10px;\n"
"}\n"
".mdescLeft {\n"
"       padding: 0px 8px 4px 8px;\n"
"	font-size: 14px;\n"
"	font-style: italic;\n"
"	background-color: #FAFAFA;\n"
"	border-top: 1px none #E0E0E0;\n"
"	border-right: 1px none #E0E0E0;\n"
"	border-bottom: 1px none #E0E0E0;\n"
"	border-left: 1px none #E0E0E0;\n"
"	margin: 0px;\n"
"}\n"
".mdescRight {\n"
"       padding: 0px 8px 4px 8px;\n"
"	font-size: 14px;\n"
"	font-style: italic;\n"
"	background-color: #FAFAFA;\n"
"	border-top: 1px none #E0E0E0;\n"
"	border-right: 1px none #E0E0E0;\n"
"	border-bottom: 1px none #E0E0E0;\n"
"	border-left: 1px none #E0E0E0;\n"
"	margin: 0px;\n"
"}\n"
".memItemLeft {\n"
"	padding: 1px 0px 0px 8px;\n"
"	margin: 4px;\n"
"	border-top-width: 1px;\n"
"	border-right-width: 1px;\n"
"	border-bottom-width: 1px;\n"
"	border-left-width: 1px;\n"
"	border-top-style: solid;\n"
"	border-top-color: #E0E0E0;\n"
"	border-right-color: #E0E0E0;\n"
"	border-bottom-color: #E0E0E0;\n"
"	border-left-color: #E0E0E0;\n"
"	border-right-style: none;\n"
"	border-bottom-style: none;\n"
"	border-left-style: none;\n"
"	background-color: #FAFAFA;\n"
"	font-size: 12px;\n"
"}\n"
".memItemRight {\n"
"	padding: 1px 8px 0px 8px;\n"
"	margin: 4px;\n"
"	border-top-width: 1px;\n"
"	border-right-width: 1px;\n"
"	border-bottom-width: 1px;\n"
"	border-left-width: 1px;\n"
"	border-top-style: solid;\n"
"	border-top-color: #E0E0E0;\n"
"	border-right-color: #E0E0E0;\n"
"	border-bottom-color: #E0E0E0;\n"
"	border-left-color: #E0E0E0;\n"
"	border-right-style: none;\n"
"	border-bottom-style: none;\n"
"	border-left-style: none;\n"
"	background-color: #FAFAFA;\n"
"	font-size: 13px;\n"
"}\n"
".search     { color: #003399;\n"
"              font-weight: bold;\n"
"}\n"
"FORM.search {\n"
"              margin-bottom: 0px;\n"
"              margin-top: 0px;\n"
"}\n"
"INPUT.search { font-size: 75%;\n"
"               color: #000080;\n"
"               font-weight: normal;\n"
"               background-color: #eeeeff;\n"
"}\n"
"TD.tiny      { font-size: 75%;\n"
"}\n"
"a {\n"
"	color: #252E78;\n"
"}\n"
"a:visited {\n"
"	color: #3D2185;\n"
"}\n";

static QCString g_header;
static QCString g_footer;

const char search_script[]=
#include "search_php.h"
;

HtmlGenerator::HtmlGenerator() : OutputGenerator()
{
  dir=Config_getString("HTML_OUTPUT");
  col=0;  
}

HtmlGenerator::~HtmlGenerator()
{
  //printf("HtmlGenerator::~HtmlGenerator()\n");
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
  if (!Config_getString("HTML_HEADER").isEmpty()) 
  {
    g_header=fileToString(Config_getString("HTML_HEADER"));
    //printf("g_header='%s'\n",g_header.data());
  }
  if (!Config_getString("HTML_FOOTER").isEmpty()) 
  {
    g_footer=fileToString(Config_getString("HTML_FOOTER"));
    //printf("g_footer='%s'\n",g_footer.data());
  }
  createSubDirs(d);
}

void HtmlGenerator::writeStyleSheetFile(QFile &file)
{
  QTextStream t(&file);
  t << defaultStyleSheet;
}

static void writeDefaultHeaderFile(QTextStream &t, const char *title,
                                   const char *relPath,bool usePathCmd)
{
  QString relPathStr;
  if (usePathCmd) 
    relPathStr="$relpath$";
  else
    relPathStr=relPath;

  t << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
    "<html><head>" 
    "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=" 
         << theTranslator->idLanguageCharset() << "\">\n"
    "<title>"; 
  t << convertToHtml(title);
  t << "</title>\n";
  t << "<link ";
  t << "href=\"";
  if (Config_getString("HTML_STYLESHEET").isEmpty())
  {
    t << relPathStr << "doxygen.css";
  }
  else
  {
    QCString cssname=Config_getString("HTML_STYLESHEET");
    QFileInfo cssfi(cssname);
    if (!cssfi.exists())
    {
      err("Error: user specified HTML style sheet file does not exist!\n");
    }
    t << relPathStr << cssfi.fileName();
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
  writeDefaultHeaderFile(t,"$title",relativePathToRoot(0),TRUE);
}

void HtmlGenerator::writeFooterFile(QFile &file)
{
  QTextStream t(&file);
  t << "<hr size=\"1\"><address style=\"align: right;\"><small>\n";
  t << theTranslator->trGeneratedAt( "$datetime", "$projectname" );
  t << "&nbsp;<a href=\"http://www.doxygen.org/index.html\">"
    << "<img src=\"doxygen.png\" alt=\"doxygen\" " 
    << "align=\"middle\" border=\"0\">"
    << "</a> $doxygenversion";
  t << "</small></address>\n"
    << "</body>\n"
    << "</html>\n";
}


void HtmlGenerator::startFile(const char *name,const char *,
                              const char *title)
{
  //printf("HtmlGenerator::startFile(%s)\n",name);
  QCString fileName=name;
  lastTitle=title;
  relPath = relativePathToRoot(fileName);

  if (fileName.right(Doxygen::htmlFileExtension.length())!=Doxygen::htmlFileExtension) 
  {
    fileName+=Doxygen::htmlFileExtension;
  }
  startPlainFile(fileName);
  if (Config_getBool("GENERATE_HTMLHELP"))
  {
    HtmlHelp::getInstance()->addIndexFile(fileName);
  }
  
  QCString dispTitle = title;
  QCString projName = Config_getString("PROJECT_NAME");
  if (!projName.isEmpty())
  {
    dispTitle.prepend(projName+": ");
  }
 
  lastFile = fileName;
  if (g_header.isEmpty()) 
  {
    writeDefaultHeaderFile(t,dispTitle,relPath,FALSE);
  }
  else
  {
    t << substituteKeywords(g_header,convertToHtml(dispTitle),relPath);
  }
  t << "<!-- " << theTranslator->trGeneratedBy() << " Doxygen " 
    << versionString << " -->" << endl;
}

//void HtmlGenerator::startQuickIndexItem(const char *s,const char *l)
//{
//  QCString *dest;
//  if (s) 
//  {
//    t << "<a class=\"qindexRef\" ";
//    t << "doxygen=\"" << s << ":";
//    if ((dest=Doxygen::tagDestinationDict[s])) t << *dest;
//    if (strcmp(s,"_cgi")!=0) t << "/"; // small hack to get the cgi binary link right
//    t << "\" ";
//  }
//  else
//  {
//    t << "<a class=\"qindex\" ";
//  }
//  t << "href=\""; 
//  if (s)
//  {
//    if ((dest=Doxygen::tagDestinationDict[s])) t << *dest;
//    if (strcmp(s,"_cgi")!=0) t << "/";
//  }
//  t << l << "\">";
//}
//
//void HtmlGenerator::endQuickIndexItem()
//{
//  t << "</a> &nbsp; ";
//}

static void writePageFooter(QTextStream &t,const QCString &lastTitle,
                            const QCString relPath)
{
  if (g_footer.isEmpty())
  {
    t << "<hr size=\"1\"><address style=\"align: right;\"><small>";
    t << theTranslator->trGeneratedAt(
        dateToString(TRUE),
        Config_getString("PROJECT_NAME")
        );
    t << "&nbsp;" << endl << "<a href=\"http://www.doxygen.org/index.html\">";
    t << endl << "<img src=\"" << relPath << "doxygen.png\" alt=\"doxygen\" " 
      << "align=\"middle\" border=\"0\">" << "</a> " << versionString << " ";
    t << "</small></address>";
    if (Debug::isFlagSet(Debug::Validate))
    {
      t << "<p><a href=\"http://validator.w3.org/check/referer\">"
           "<img border=\"0\" src=\"http://www.w3.org/Icons/valid-html401\""
           " height=\"31\" width=\"88\" alt=\"This page is Valid HTML 4.01 "
           "Transitional!\"></a><a href=\"http://jigsaw.w3.org/css-validator/\">"
           "<img style=\"border:0;width:88px;height:31px\" "
           "src=\"http://jigsaw.w3.org/css-validator/images/vcss\" "
           "alt=\"This page uses valid CSS!\"></a></p>";
    }
    t << "\n</body>\n</html>\n";
  }
  else
  {
    t << substituteKeywords(g_footer,convertToHtml(lastTitle),relPath);
  }
}

void HtmlGenerator::writeFooter()
{
  writePageFooter(t,lastTitle,relPath);
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
  //printf("writeStyleInfo(%d)\n",part);
  if (part==0)
  {
    if (Config_getString("HTML_STYLESHEET").isEmpty()) // write default style sheet
    {
      //printf("write doxygen.css\n");
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
  t << "<a class=\"anchor\" name=\"" << anchor << "\" doxytag=\"" << name << "\"></a>";
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
}

void HtmlGenerator::writeStartAnnoItem(const char *,const char *f,
                                       const char *path,const char *name)
{
  t << "<li>";
  if (path) docify(path);
  t << "<a class=\"el\" href=\"" << f << Doxygen::htmlFileExtension << "\">";
  docify(name);
  t << "</a> ";
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
    if ((dest=Doxygen::tagDestinationDict[ref])) t << relPath << *dest << "/";
  }
  else
  {
    t << relPath;
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
    if ((dest=Doxygen::tagDestinationDict[ref])) t << relPath << *dest << "/";
  }
  else
  {
    t << relPath;
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
  if (f)   t << relPath << f << Doxygen::htmlFileExtension;
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
  t << "<a class=\"anchor\" name=\"" << lab << "\">";
}

void HtmlGenerator::endSection(const char *,SectionInfo::SectionType type)
{
  t << "</a>";
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
    << relPath << fileName << ".png\" usemap=\"#" << name << "_map\""
    << " border=\"0\" alt=\"\"></center>" << endl
    << "<map name=\"" << name << "_map\">" << endl;

  d.writeImage(t,dir,relPath,fileName);
}


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
      case 0:  t << "<td class=\"memItemLeft\" nowrap align=\"right\" valign=\"top\">"; break;
      case 1:  t << "<td class=\"memItemLeft\" nowrap>"; break;
      default: t << "<td class=\"memItemLeft\" nowrap valign=\"top\">"; break;
    }
  }
  else
  {
    t << "<li>"; 
  }
}

void HtmlGenerator::endMemberItem() 
{ 
  //DBG_HTML(t << "<!-- endMemberItem(" << (int)inGroup << "," << fileName << "," << headerName << " -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "</td></tr>\n"; 
  }
  t << endl; 
}


void HtmlGenerator::insertMemberAlign() 
{ 
  DBG_HTML(t << "<!-- insertMemberAlign -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "&nbsp;</td><td class=\"memItemRight\" valign=\"bottom\">"; 
  }
}

void HtmlGenerator::startMemberDescription() 
{ 
  DBG_HTML(t << "<!-- startMemberDescription -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "<tr><td class=\"mdescLeft\">&nbsp;</td><td class=\"mdescRight\">"; 
  }
  else
  {
    t << "<dl class=\"el\"><dd class=\"mdescRight\">";
  }
}

void HtmlGenerator::endMemberDescription()   
{ 
  DBG_HTML(t << "<!-- endMemberDescription -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "<br></td></tr>" << endl; 
  }
  else
  {
    t << "<br></dl>";
  }
}

void HtmlGenerator::startMemberSections()
{
  DBG_HTML(t << "<!-- startMemberSections -->" << endl)
  if (Config_getBool("HTML_ALIGN_MEMBERS"))
  {
    t << "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">" << endl;
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
    t << "<tr><td colspan=\"2\"><br><h2>";
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
  if (Config_getBool("HTML_ALIGN_MEMBERS")) t << "<tr><td colspan=\"2\">";
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

void HtmlGenerator::startMemberDoc(const char *,const char *,const char *,const char *) 
{ 
  DBG_HTML(t << "<!-- startMemberDoc -->" << endl;)
  t << "<p>" << endl;
  t << "<table class=\"mdTable\" width=\"100%\" cellpadding=\"2\" cellspacing=\"0\">" << endl;
  t << "  <tr>" << endl;
  t << "    <td class=\"mdRow\">" << endl; 
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

void HtmlGenerator::startMemberDocName(bool align)
{
  DBG_HTML(t << "<!-- startMemberDocName -->" << endl;)
  t << "        <tr>" << endl;
  t << "          <td class=\"md\" nowrap valign=\"top\"";
  if (align) t << " align=\"right\"";
  t << ">";
}

void HtmlGenerator::endMemberDocName()
{
  DBG_HTML(t << "<!-- endMemberDocName -->" << endl;)
  t << "          </td>" << endl;
}

void HtmlGenerator::startParameterList(bool openBracket)
{
  DBG_HTML(t << "<!-- startParameterList -->" << endl;)
  t << "          <td class=\"md\" valign=\"top\">";
  if (openBracket)
  {
    t << "(&nbsp;";
  }
  t << "</td>" << endl;
}

void HtmlGenerator::startParameterType(bool first,const char *key)
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
    t << "          <td class=\"md\" nowrap align=\"right\">";
    if (key) t << key;
    t << "</td>" << endl;
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
  t << "\" nowrap>"; //&nbsp;";
}

void HtmlGenerator::endParameterName(bool last,bool emptyList,bool closeBracket)
{
  DBG_HTML(t << "<!-- endParameterName -->" << endl;)
  if (last)
  {
    if (emptyList)
    {
      t << "          </td>" << endl;
      t << "          <td class=\"md\" valign=\"top\">";
      if (closeBracket) t << "&nbsp;)";
      t << "&nbsp;</td>" << endl;
      t << "          <td class=\"md\" nowrap>";
    }
    else
    {
      t << "</td>" << endl;
      t << "        </tr>" << endl;
      t << "        <tr>" << endl;
      t << "          <td></td>" << endl;
      t << "          <td class=\"md\">";
      if (closeBracket) t << ")";
      t << "&nbsp;</td>" << endl;
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

void HtmlGenerator::endMemberDoc(bool hasArgs)     
{ 
  DBG_HTML(t << "<!-- endMemberDoc -->" << endl;)
  if (!hasArgs)
  {
    t << "        </tr>" << endl;
  }
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
  g.writeGraph(t,BITMAP,dir,relPath);
}

void HtmlGenerator::startInclDepGraph()
{
}

void HtmlGenerator::endInclDepGraph(DotInclDepGraph &g)
{
  g.writeGraph(t,BITMAP,dir,relPath);
}

void HtmlGenerator::startCallGraph()
{
}

void HtmlGenerator::endCallGraph(DotCallGraph &g)
{
  g.writeGraph(t,BITMAP,dir,relPath);
}

void HtmlGenerator::writeGraphicalHierarchy(DotGfxHierarchyTable &g)
{
  g.writeGraph(t,dir);
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

  t << "<table cellspacing=\"5\" cellpadding=\"0\" border=\"0\">\n"
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

void HtmlGenerator::printDoc(DocNode *n)
{
  HtmlDocVisitor *visitor = new HtmlDocVisitor(t,*this);
  n->accept(visitor);
  delete visitor; 
}

static void startQuickIndexItem(QTextStream &t,const char *l,
                                bool hl,bool compact,bool &first,
                                const QCString &relPath)
{
  if (!first && compact) t << " | ";
  first=FALSE;
  if (!compact) t << "<li>";
  if (hl && compact)
  {
    t << "<a class=\"qindexHL\" ";
  }
  else
  {
    t << "<a class=\"qindex\" ";
  }
  t << "href=\"" << relPath << l << "\">";
}

static void endQuickIndexItem(QTextStream &t)
{
  t << "</a>";
}

static QCString fixSpaces(const QCString &s)
{
  return substitute(s," ","&nbsp;");
}

static void writeDefaultQuickLinks(QTextStream &t,bool compact,
                                   HighlightedItem hli,const QCString &relPath)
{
  bool first=TRUE;
  if (compact) 
  {
    t << "<div class=\"qindex\">"; 
  }
  else 
  {
    t << "<ul>";
  }

  if (Config_getBool("SEARCHENGINE"))
  {
    t << "  <form class=\"search\" action=\"" << relPath 
      << "search.php\" method=\"get\">\n";
  }

  if (Config_getBool("GENERATE_TREEVIEW"))
  {
    startQuickIndexItem(t,"main"+Doxygen::htmlFileExtension,
                        hli==HLI_Main,compact,first,relPath);
  }
  else
  {
    startQuickIndexItem(t,"index"+Doxygen::htmlFileExtension,
                        hli==HLI_Main,compact,first,relPath);
  }
  t << fixSpaces(theTranslator->trMainPage());
  endQuickIndexItem(t);

  if (documentedGroups>0)
  {
    startQuickIndexItem(t,"modules"+Doxygen::htmlFileExtension,
                        hli==HLI_Modules,compact,first,relPath);
    t << fixSpaces(theTranslator->trModules());
    endQuickIndexItem(t);
  } 
  if (documentedNamespaces>0)
  {
    startQuickIndexItem(t,"namespaces"+Doxygen::htmlFileExtension,
                        hli==HLI_Namespaces,compact,first,relPath);
    if (Config_getBool("OPTIMIZE_OUTPUT_JAVA"))
    {
      t << fixSpaces(theTranslator->trPackages());
    }
    else
    {
      t << theTranslator->trNamespaceList();
    }
    endQuickIndexItem(t);
  }
  if (hierarchyClasses>0)
  {
    startQuickIndexItem(t,"hierarchy"+Doxygen::htmlFileExtension,
                           hli==HLI_Hierarchy,compact,first,relPath);
    t << fixSpaces(theTranslator->trClassHierarchy());
    endQuickIndexItem(t);
  } 
  if (annotatedClasses>0)
  {
    if (Config_getBool("ALPHABETICAL_INDEX"))
    {
      startQuickIndexItem(t,"classes"+Doxygen::htmlFileExtension,
                          hli==HLI_Classes,compact,first,relPath);
      t << fixSpaces(theTranslator->trAlphabeticalList());
      endQuickIndexItem(t);
    }
    startQuickIndexItem(t,"annotated"+Doxygen::htmlFileExtension,
                        hli==HLI_Annotated,compact,first,relPath);
    t << fixSpaces(theTranslator->trCompoundList());
    endQuickIndexItem(t);
  } 
  if (Config_getBool("SHOW_DIRECTORIES") && documentedDirs>0)
  {
    startQuickIndexItem(t,"dirs"+Doxygen::htmlFileExtension,
                        hli==HLI_Directories,compact,first,relPath);
    t << fixSpaces(theTranslator->trDirectories());
    endQuickIndexItem(t);
  } 
  if (documentedHtmlFiles>0)
  {
    startQuickIndexItem(t,"files"+Doxygen::htmlFileExtension,
                        hli==HLI_Files,compact,first,relPath);
    t << fixSpaces(theTranslator->trFileList());
    endQuickIndexItem(t);
  } 
  if (documentedNamespaceMembers[NMHL_All]>0)
  {
    startQuickIndexItem(t,"namespacemembers"+Doxygen::htmlFileExtension,
                        hli==HLI_NamespaceMembers,compact,first,relPath);
    t << fixSpaces(theTranslator->trNamespaceMembers());
    endQuickIndexItem(t);
  }
  if (documentedClassMembers[CMHL_All]>0)
  {
    startQuickIndexItem(t,"functions"+Doxygen::htmlFileExtension,
                        hli==HLI_Functions,compact,first,relPath);
    t << fixSpaces(theTranslator->trCompoundMembers());
    endQuickIndexItem(t);
  } 
  if (documentedFileMembers[FMHL_All]>0)
  {
    startQuickIndexItem(t,"globals"+Doxygen::htmlFileExtension,
                        hli==HLI_Globals,compact,first,relPath);
    t << fixSpaces(theTranslator->trFileMembers());
    endQuickIndexItem(t);
  } 
  if (indexedPages>0)
  {
    startQuickIndexItem(t,"pages"+Doxygen::htmlFileExtension,
                        hli==HLI_Pages,compact,first,relPath);
    t << fixSpaces(theTranslator->trRelatedPages());
    endQuickIndexItem(t);
  } 
  if (Doxygen::exampleSDict->count()>0)
  {
    startQuickIndexItem(t,"examples"+Doxygen::htmlFileExtension,
                        hli==HLI_Examples,compact,first,relPath);
    t << fixSpaces(theTranslator->trExamples());
    endQuickIndexItem(t);
  } 
  if (Config_getBool("SEARCHENGINE"))
  {
    if (compact)
    {
      t << "  | ";
    }
    else
    {
      t << "<li>";
    }
    t << "<span class=\"search";
    if (hli==HLI_Search) t << "HL";
    QCString searchFor = fixSpaces(theTranslator->trSearchForIndex());
    if (searchFor.at(0)=='S') searchFor="<u>S</u>"+searchFor.mid(1);
    t << "\">" << searchFor << "&nbsp;";
    if (hli!=HLI_Search)
    {
      t << "<input class=\"search\" type=\"text\" name=\"query\" value=\"\" size=\"20\" accesskey=\"s\"/>"
           "</span></form>";
    }
  } 
  if (hli!=HLI_Search)
  {
    if (compact) 
    {
      t << "</div>\n";
    }
    else 
    {
      t << "</ul>\n";
    }
  }
}

void HtmlGenerator::writeQuickLinks(bool compact,HighlightedItem hli)
{
  writeDefaultQuickLinks(t,compact,hli,relPath);
}

void HtmlGenerator::writeSearchPage()
{
  if (Config_getBool("SEARCHENGINE") && Config_getBool("GENERATE_HTML"))
  {
    QCString fileName = Config_getString("HTML_OUTPUT")+"/search.php";
    QFile f(fileName);
    if (f.open(IO_WriteOnly))
    {
      QTextStream t(&f);
#if QT_VERSION >= 200
      t.setEncoding(QTextStream::Latin1);
#endif
      if (g_header.isEmpty()) 
      {
        writeDefaultHeaderFile(t,theTranslator->trSearch().data(),0,FALSE);
      }
      else
      {
        t << substituteKeywords(g_header,"Search","");
      }
      t << "<!-- " << theTranslator->trGeneratedBy() << " Doxygen " 
        << versionString << " -->" << endl;
      if (!Config_getBool("DISABLE_INDEX")) 
      { 
        writeDefaultQuickLinks(t,TRUE,HLI_Search,"");
      }

      t << "\n<?php\n\n";
      t << "function search_results()\n";
      t << "{\n";
      t << "  return \"" << theTranslator->trSearchResultsTitle() << "\";\n";
      t << "}\n";
      t << "\n";
      t << "function matches_text($num)\n";
      t << "{\n";
      t << "  if ($num==0)\n";
      t << "  {\n";
      t << "    return \"" << theTranslator->trSearchResults(0) << "\";\n";
      t << "  }\n";
      t << "  else if ($num==1)\n";
      t << "  {\n";
      t << "    return \"" << theTranslator->trSearchResults(1) << "\";\n";
      t << "  }\n";
      t << "  else // $num>1\n";
      t << "  {\n";
      t << "    return \"" << theTranslator->trSearchResults(2) << "\";\n";
      t << "  }\n";
      t << "}\n";
      t << "\n";
      t << "function report_matches()\n";
      t << "{\n";
      t << "  return \"" << theTranslator->trSearchMatches() << " \";\n";
      t << "}\n";
      t << "\n";
      t << search_script;
      t << "\n";
      t << "?>\n";
      writePageFooter(t,"Search","");
    }
  }
}

