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

#include <qdir.h>
#include <qregexp.h>
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
#include "searchindex.h"
#include "pagedef.h"
#include "debug.h"
#include "dirdef.h"
#include "vhdldocgen.h"
#include "layout.h"
#include "image.h"
#include "ftvhelp.h"
#include "bufstr.h"
#include "resourcemgr.h"


//#define DBG_HTML(x) x;
#define DBG_HTML(x)

static QCString g_header;
static QCString g_footer;
static QCString g_mathjax_code;


static void writeClientSearchBox(FTextStream &t,const char *relPath)
{
  t << "        <div id=\"MSearchBox\" class=\"MSearchBoxInactive\">\n";
  t << "        <span class=\"left\">\n";
  t << "          <img id=\"MSearchSelect\" src=\"" << relPath << "search/mag_sel.png\"\n";
  t << "               onmouseover=\"return searchBox.OnSearchSelectShow()\"\n";
  t << "               onmouseout=\"return searchBox.OnSearchSelectHide()\"\n";
  t << "               alt=\"\"/>\n";
  t << "          <input type=\"text\" id=\"MSearchField\" value=\"" 
    << theTranslator->trSearch() << "\" accesskey=\"S\"\n";
  t << "               onfocus=\"searchBox.OnSearchFieldFocus(true)\" \n";
  t << "               onblur=\"searchBox.OnSearchFieldFocus(false)\" \n";
  t << "               onkeyup=\"searchBox.OnSearchFieldChange(event)\"/>\n";
  t << "          </span><span class=\"right\">\n";
  t << "            <a id=\"MSearchClose\" href=\"javascript:searchBox.CloseResultsWindow()\">"
    << "<img id=\"MSearchCloseImg\" border=\"0\" src=\"" << relPath << "search/close.png\" alt=\"\"/></a>\n";
  t << "          </span>\n";
  t << "        </div>\n";
}

static void writeServerSearchBox(FTextStream &t,const char *relPath,bool highlightSearch)
{
  static bool externalSearch = Config_getBool(EXTERNAL_SEARCH);
  t << "        <div id=\"MSearchBox\" class=\"MSearchBoxInactive\">\n";
  t << "          <div class=\"left\">\n";
  t << "            <form id=\"FSearchBox\" action=\"" << relPath;
  if (externalSearch)
  {
    t << "search" << Doxygen::htmlFileExtension;
  }
  else
  {
    t << "search.php";
  }
  t << "\" method=\"get\">\n";
  t << "              <img id=\"MSearchSelect\" src=\"" << relPath << "search/mag.png\" alt=\"\"/>\n";
  if (!highlightSearch)
  {
    t << "              <input type=\"text\" id=\"MSearchField\" name=\"query\" value=\""
      << theTranslator->trSearch() << "\" size=\"20\" accesskey=\"S\" \n";
    t << "                     onfocus=\"searchBox.OnSearchFieldFocus(true)\" \n";
    t << "                     onblur=\"searchBox.OnSearchFieldFocus(false)\"/>\n";
    t << "            </form>\n";
    t << "          </div><div class=\"right\"></div>\n";
    t << "        </div>\n";
  }
}

//------------------------------------------------------------------------

/// Clear a text block \a s from \a begin to \a end markers
QCString clearBlock(const char *s,const char *begin,const char *end)
{
  if (s==0 || begin==0 || end==0) return s;
  const char *p, *q;
  int beginLen = qstrlen(begin);
  int endLen = qstrlen(end);
  int resLen = 0;
  for (p=s; (q=strstr(p,begin))!=0; p=q+endLen)
  {
    resLen+=(int)(q-p);
    p=q+beginLen;
    if ((q=strstr(p,end))==0)
    {
      resLen+=beginLen;
      break;
    }
  }
  resLen+=qstrlen(p);
  // resLen is the length of the string without the marked block

  QCString result(resLen+1);
  char *r;
  for (r=result.rawData(), p=s; (q=strstr(p,begin))!=0; p=q+endLen)
  {
    int l = (int)(q-p);
    memcpy(r,p,l);
    r+=l;
    p=q+beginLen;
    if ((q=strstr(p,end))==0)
    {
      memcpy(r,begin,beginLen);
      r+=beginLen;
      break;
    }
  }
  qstrcpy(r,p);
  return result;
}
//----------------------------------------------------------------------

QCString selectBlock(const QCString& s,const QCString &name,bool enable)
{
  // TODO: this is an expensive function that is called a lot -> optimize it
  const QCString begin = "<!--BEGIN " + name + "-->";
  const QCString end = "<!--END " + name + "-->";
  const QCString nobegin = "<!--BEGIN !" + name + "-->";
  const QCString noend = "<!--END !" + name + "-->";

  QCString result = s;
  if (enable)
  {
    result = substitute(result, begin, "");
    result = substitute(result, end, "");
    result = clearBlock(result, nobegin, noend);
  }
  else
  {
    result = substitute(result, nobegin, "");
    result = substitute(result, noend, "");
    result = clearBlock(result, begin, end);
  }

  return result;
}

static QCString getSearchBox(bool serverSide, QCString relPath, bool highlightSearch)
{
  QGString result;
  FTextStream t(&result);
  if (serverSide)
  {
    writeServerSearchBox(t, relPath, highlightSearch);
  }
  else
  {
    writeClientSearchBox(t, relPath);
  }
  return QCString(result);
}

static QCString removeEmptyLines(const QCString &s)
{
  BufStr out(s.length()+1);
  const char *p=s.data();
  if (p)
  {
    char c;
    while ((c=*p++))
    {
      if (c=='\n')
      {
        const char *e = p;
        while (*e==' ' || *e=='\t') e++;
        if (*e=='\n') 
        {
          p=e;
        }
        else out.addChar(c);
      }
      else
      {
        out.addChar(c);
      }
    }
  }
  out.addChar('\0');
  //printf("removeEmptyLines(%s)=%s\n",s.data(),out.data());
  return out.data();
}

static QCString substituteHtmlKeywords(const QCString &s,
                                       const QCString &title,
                                       const QCString &relPath,
                                       const QCString &navPath=QCString())
{
  // Build CSS/Javascript tags depending on treeview, search engine settings
  QCString cssFile;
  QStrList extraCssFile;
  QCString generatedBy;
  QCString treeViewCssJs;
  QCString searchCssJs;
  QCString searchBox;
  QCString mathJaxJs;
  QCString extraCssText;

  static QCString projectName = Config_getString(PROJECT_NAME);
  static bool timeStamp = Config_getBool(HTML_TIMESTAMP);
  static bool treeView = Config_getBool(GENERATE_TREEVIEW);
  static bool searchEngine = Config_getBool(SEARCHENGINE);
  static bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
  static bool mathJax = Config_getBool(USE_MATHJAX);
  static QCString mathJaxFormat = Config_getEnum(MATHJAX_FORMAT);
  static bool disableIndex = Config_getBool(DISABLE_INDEX);
  static bool hasProjectName = !projectName.isEmpty();
  static bool hasProjectNumber = !Config_getString(PROJECT_NUMBER).isEmpty();
  static bool hasProjectBrief = !Config_getString(PROJECT_BRIEF).isEmpty();
  static bool hasProjectLogo = !Config_getString(PROJECT_LOGO).isEmpty();
  static bool titleArea = (hasProjectName || hasProjectBrief || hasProjectLogo || (disableIndex && searchEngine));

  cssFile = Config_getString(HTML_STYLESHEET);
  if (cssFile.isEmpty())
  {
    cssFile = "doxygen.css";
  }
  else
  {
    QFileInfo cssfi(cssFile);
    if (cssfi.exists())
    {
      cssFile = cssfi.fileName().utf8();
    }
    else
    {
      cssFile = "doxygen.css";
    }
  }

  extraCssText = "";
  extraCssFile = Config_getList(HTML_EXTRA_STYLESHEET);
  for (uint i=0; i<extraCssFile.count(); ++i)
  {
    QCString fileName(extraCssFile.at(i));
    if (!fileName.isEmpty())
    {
      QFileInfo fi(fileName);
      if (fi.exists())
      {
        extraCssText += "<link href=\"$relpath^"+stripPath(fileName)+"\" rel=\"stylesheet\" type=\"text/css\"/>\n";
      }
    }
  }

  if (timeStamp) 
  {
    generatedBy = theTranslator->trGeneratedAt(dateToString(TRUE), convertToHtml(Config_getString(PROJECT_NAME)));
  }
  else 
  {
    generatedBy = theTranslator->trGeneratedBy();
  }

  if (treeView)
  {
    treeViewCssJs = "<link href=\"$relpath^navtree.css\" rel=\"stylesheet\" type=\"text/css\"/>\n"
                    "<script type=\"text/javascript\" src=\"$relpath^resize.js\"></script>\n"
                    "<script type=\"text/javascript\" src=\"$relpath^navtreedata.js\"></script>\n"
                    "<script type=\"text/javascript\" src=\"$relpath^navtree.js\"></script>\n"
                    "<script type=\"text/javascript\">\n"
                    "  $(document).ready(initResizable);\n"
                    "  $(window).load(resizeHeight);\n"
                    "</script>";
  }

  if (searchEngine)
  {
    searchCssJs = "<link href=\"$relpath^search/search.css\" rel=\"stylesheet\" type=\"text/css\"/>\n";
    if (!serverBasedSearch)
    {
      searchCssJs += "<script type=\"text/javascript\" src=\"$relpath^search/searchdata.js\"></script>\n";
    }
    searchCssJs += "<script type=\"text/javascript\" src=\"$relpath^search/search.js\"></script>\n";

    if (!serverBasedSearch) 
    {
      searchCssJs += "<script type=\"text/javascript\">\n"
                     "  $(document).ready(function() { init_search(); });\n"
                     "</script>";
    }
    else 
    {
      searchCssJs += "<script type=\"text/javascript\">\n"
                     "  $(document).ready(function() {\n"
                     "    if ($('.searchresults').length > 0) { searchBox.DOMSearchField().focus(); }\n"
                     "  });\n"
                     "</script>\n";

      // OPENSEARCH_PROVIDER {
      searchCssJs += "<link rel=\"search\" href=\"" + relPath +
                     "search_opensearch.php?v=opensearch.xml\" "
                     "type=\"application/opensearchdescription+xml\" title=\"" +
                     (hasProjectName ? projectName : QCString("Doxygen")) + 
                     "\"/>";
      // OPENSEARCH_PROVIDER }
    }
    searchBox = getSearchBox(serverBasedSearch, relPath, FALSE);
  }

  if (mathJax)
  {
    QCString path = Config_getString(MATHJAX_RELPATH);  
    if (path.isEmpty() || path.left(2)=="..") // relative path  
    {  
      path.prepend(relPath);   
    }  
    mathJaxJs = "<script type=\"text/x-mathjax-config\">\n"
                "  MathJax.Hub.Config({\n"
                "    extensions: [\"tex2jax.js\"";
    QStrList &mathJaxExtensions = Config_getList(MATHJAX_EXTENSIONS);
    const char *s = mathJaxExtensions.first();
    while (s)
    {
      mathJaxJs+= ", \""+QCString(s)+".js\"";
      s = mathJaxExtensions.next();
    }
    if (mathJaxFormat.isEmpty())
    {
      mathJaxFormat = "HTML-CSS";
    }
    mathJaxJs += "],\n"
                 "    jax: [\"input/TeX\",\"output/"+mathJaxFormat+"\"],\n"
                 "});\n";
    if (!g_mathjax_code.isEmpty())
    {
      mathJaxJs += g_mathjax_code;
      mathJaxJs += "\n";
    }
    mathJaxJs += "</script>";
    mathJaxJs += "<script type=\"text/javascript\" src=\"" + path + "MathJax.js\"></script>\n";
  }

  // first substitute generic keywords
  QCString result = substituteKeywords(s,title,
        convertToHtml(Config_getString(PROJECT_NAME)),
        convertToHtml(Config_getString(PROJECT_NUMBER)),
        convertToHtml(Config_getString(PROJECT_BRIEF)));

  // additional HTML only keywords
  result = substitute(result,"$navpath",navPath);
  result = substitute(result,"$stylesheet",cssFile);
  result = substitute(result,"$treeview",treeViewCssJs);
  result = substitute(result,"$searchbox",searchBox);
  result = substitute(result,"$search",searchCssJs);
  result = substitute(result,"$mathjax",mathJaxJs);
  result = substitute(result,"$generatedby",generatedBy);
  result = substitute(result,"$extrastylesheet",extraCssText);
  result = substitute(result,"$relpath$",relPath); //<-- obsolete: for backwards compatibility only
  result = substitute(result,"$relpath^",relPath); //<-- must be last
  
  // additional HTML only conditional blocks
  result = selectBlock(result,"DISABLE_INDEX",disableIndex);
  result = selectBlock(result,"GENERATE_TREEVIEW",treeView);
  result = selectBlock(result,"SEARCHENGINE",searchEngine);
  result = selectBlock(result,"TITLEAREA",titleArea);
  result = selectBlock(result,"PROJECT_NAME",hasProjectName);
  result = selectBlock(result,"PROJECT_NUMBER",hasProjectNumber);
  result = selectBlock(result,"PROJECT_BRIEF",hasProjectBrief);
  result = selectBlock(result,"PROJECT_LOGO",hasProjectLogo);

  result = removeEmptyLines(result);

  return result;
}

//--------------------------------------------------------------------------

HtmlCodeGenerator::HtmlCodeGenerator()
   : m_streamSet(FALSE), m_col(0)
{
}

HtmlCodeGenerator::HtmlCodeGenerator(FTextStream &t,const QCString &relPath) 
   : m_col(0), m_relPath(relPath)
{
  setTextStream(t);
}

void HtmlCodeGenerator::setTextStream(FTextStream &t)
{
  m_streamSet = t.device()!=0;
  m_t.setDevice(t.device());
}

void HtmlCodeGenerator::setRelativePath(const QCString &path)
{
  m_relPath = path;
}

void HtmlCodeGenerator::codify(const char *str)
{
  static int tabSize = Config_getInt(TAB_SIZE);
  if (str && m_streamSet)
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
                         tabSize - (m_col%tabSize); 
                   m_t << Doxygen::spaces.left(spacesToNextTabStop); 
                   m_col+=spacesToNextTabStop; 
                   break; 
        case '\n': m_t << "\n"; m_col=0; 
                   break;
        case '\r': break;
        case '<':  m_t << "&lt;"; m_col++; 
                   break;
        case '>':  m_t << "&gt;"; m_col++; 
                   break;
        case '&':  m_t << "&amp;"; m_col++; 
                   break;
        case '\'': m_t << "&#39;"; m_col++; // &apos; is not valid XHTML
                   break;
        case '"':  m_t << "&quot;"; m_col++;
                   break;
        case '\\':
                   if (*p=='<')
                     { m_t << "&lt;"; p++; }
                   else if (*p=='>')
                     { m_t << "&gt;"; p++; }
                   else
                     m_t << "\\";
                   m_col++;
                   break;
        default:   p=writeUtf8Char(m_t,p-1);    
                   m_col++;                    
                   break;
      }
    }
  }
}

void HtmlCodeGenerator::docify(const char *str)
{
  if (str && m_streamSet)
  {
    const char *p=str;
    char c;
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '<':  m_t << "&lt;"; break;
        case '>':  m_t << "&gt;"; break;
        case '&':  m_t << "&amp;"; break;
        case '"':  m_t << "&quot;"; break;
        case '\\':
                   if (*p=='<')
                     { m_t << "&lt;"; p++; }
                   else if (*p=='>')
                     { m_t << "&gt;"; p++; }
                   else
                     m_t << "\\";
                   break;
        default:   m_t << c; 
      }
    }
  }
}

void HtmlCodeGenerator::writeLineNumber(const char *ref,const char *filename,
                                    const char *anchor,int l)
{
  if (!m_streamSet) return;
  const int maxLineNrStr = 10;
  char lineNumber[maxLineNrStr];
  char lineAnchor[maxLineNrStr];
  qsnprintf(lineNumber,maxLineNrStr,"%5d",l);
  qsnprintf(lineAnchor,maxLineNrStr,"l%05d",l);

  m_t << "<div class=\"line\">";
  m_t << "<a name=\"" << lineAnchor << "\"></a><span class=\"lineno\">"; 
  if (filename)
  {
    _writeCodeLink("line",ref,filename,anchor,lineNumber,0);
  }
  else
  {
    codify(lineNumber);
  }
  m_t << "</span>"; 
  m_t << "&#160;";
}

void HtmlCodeGenerator::writeCodeLink(const char *ref,const char *f,
                                      const char *anchor, const char *name,
                                      const char *tooltip)
{
  if (!m_streamSet) return;
  //printf("writeCodeLink(ref=%s,f=%s,anchor=%s,name=%s,tooltip=%s)\n",ref,f,anchor,name,tooltip);
  _writeCodeLink("code",ref,f,anchor,name,tooltip);
}

void HtmlCodeGenerator::_writeCodeLink(const char *className,
                                      const char *ref,const char *f,
                                      const char *anchor, const char *name,
                                      const char *tooltip)
{
  if (ref) 
  {
    m_t << "<a class=\"" << className << "Ref\" ";
    m_t << externalLinkTarget() << externalRef(m_relPath,ref,FALSE);
  }
  else
  {
    m_t << "<a class=\"" << className << "\" ";
  }
  m_t << "href=\"";
  m_t << externalRef(m_relPath,ref,TRUE);
  if (f) m_t << f << Doxygen::htmlFileExtension;
  if (anchor) m_t << "#" << anchor;
  m_t << "\"";
  if (tooltip) m_t << " title=\"" << convertToHtml(tooltip) << "\"";
  m_t << ">";
  docify(name);
  m_t << "</a>";
  m_col+=qstrlen(name);
}

void HtmlCodeGenerator::writeTooltip(const char *id, const DocLinkInfo &docInfo,
                                     const char *decl, const char *desc,
                                     const SourceLinkInfo &defInfo,
                                     const SourceLinkInfo &declInfo)
{
  m_t << "<div class=\"ttc\" id=\"" << id << "\">";
  m_t << "<div class=\"ttname\">";
  if (!docInfo.url.isEmpty())
  {
    m_t << "<a href=\"";
    m_t << externalRef(m_relPath,docInfo.ref,TRUE);
    m_t << docInfo.url << Doxygen::htmlFileExtension;
    if (!docInfo.anchor.isEmpty())
    {
      m_t << "#" << docInfo.anchor;
    }
    m_t << "\">";
  }
  docify(docInfo.name);
  if (!docInfo.url.isEmpty())
  {
    m_t << "</a>";
  }
  m_t << "</div>";
  if (decl)
  {
    m_t << "<div class=\"ttdeci\">";
    docify(decl);
    m_t << "</div>";
  }
  if (desc)
  {
    m_t << "<div class=\"ttdoc\">";
    docify(desc); // desc is already HTML escaped; but there are still < and > signs
    m_t << "</div>";
  }
  if (!defInfo.file.isEmpty())
  {
    m_t << "<div class=\"ttdef\"><b>Definition:</b> ";
    if (!defInfo.url.isEmpty())
    {
      m_t << "<a href=\"";
      m_t << externalRef(m_relPath,defInfo.ref,TRUE);
      m_t << defInfo.url << Doxygen::htmlFileExtension;
      if (!defInfo.anchor.isEmpty())
      {
        m_t << "#" << defInfo.anchor;
      }
      m_t << "\">";
    }
    m_t << defInfo.file << ":" << defInfo.line;
    if (!defInfo.url.isEmpty())
    {
      m_t << "</a>";
    }
    m_t << "</div>";
  }
  if (!declInfo.file.isEmpty())
  {
    m_t << "<div class=\"ttdecl\"><b>Declaration:</b> ";
    if (!declInfo.url.isEmpty())
    {
      m_t << "<a href=\"";
      m_t << externalRef(m_relPath,declInfo.ref,TRUE);
      m_t << declInfo.url << Doxygen::htmlFileExtension;
      if (!declInfo.anchor.isEmpty())
      {
        m_t << "#" << declInfo.anchor;
      }
      m_t << "\">";
    }
    m_t << declInfo.file << ":" << declInfo.line;
    if (!declInfo.url.isEmpty())
    {
      m_t << "</a>";
    }
    m_t << "</div>";
  }
  m_t << "</div>" << endl;
}


void HtmlCodeGenerator::startCodeLine(bool hasLineNumbers) 
{ 
  if (m_streamSet)
  {
    if (!hasLineNumbers) m_t << "<div class=\"line\">";
    m_col=0; 
  }
}

void HtmlCodeGenerator::endCodeLine() 
{ 
  if (m_streamSet) m_t << "</div>";
}

void HtmlCodeGenerator::startFontClass(const char *s) 
{ 
  if (m_streamSet) m_t << "<span class=\"" << s << "\">"; 
}

void HtmlCodeGenerator::endFontClass() 
{ 
  if (m_streamSet) m_t << "</span>"; 
}

void HtmlCodeGenerator::writeCodeAnchor(const char *anchor) 
{ 
  if (m_streamSet) m_t << "<a name=\"" << anchor << "\"></a>"; 
}

//--------------------------------------------------------------------------

HtmlGenerator::HtmlGenerator() : OutputGenerator()
{
  dir=Config_getString(HTML_OUTPUT);
  m_emptySection=FALSE;
}

HtmlGenerator::~HtmlGenerator()
{
  //printf("HtmlGenerator::~HtmlGenerator()\n");
}

void HtmlGenerator::init()
{
  QCString dname=Config_getString(HTML_OUTPUT);
  QDir d(dname);
  if (!d.exists() && !d.mkdir(dname))
  {
    err("Could not create output directory %s\n",dname.data());
    exit(1);
  }
  //writeLogo(dname);
  if (!Config_getString(HTML_HEADER).isEmpty()) 
  {
    g_header=fileToString(Config_getString(HTML_HEADER));
    //printf("g_header='%s'\n",g_header.data());
  }
  else 
  {
    g_header = ResourceMgr::instance().getAsString("header.html");
  }

  if (!Config_getString(HTML_FOOTER).isEmpty()) 
  {
    g_footer=fileToString(Config_getString(HTML_FOOTER));
    //printf("g_footer='%s'\n",g_footer.data());
  }
  else 
  {
    g_footer = ResourceMgr::instance().getAsString("footer.html");
  }

  if (Config_getBool(USE_MATHJAX))
  {
    if (!Config_getString(MATHJAX_CODEFILE).isEmpty()) 
    {
      g_mathjax_code=fileToString(Config_getString(MATHJAX_CODEFILE));
      //printf("g_mathjax_code='%s'\n",g_mathjax_code.data());
    }
  }
  createSubDirs(d);

  ResourceMgr &mgr = ResourceMgr::instance();
  mgr.copyResource("tabs.css",dname);
  mgr.copyResource("jquery.js",dname);
  if (Config_getBool(INTERACTIVE_SVG))
  {
    mgr.copyResource("svgpan.js",dname);
  }

  {
    QFile f(dname+"/dynsections.js");
    if (f.open(IO_WriteOnly))
    {
      FTextStream t(&f);
      t << mgr.getAsString("dynsections.js");
      if (Config_getBool(SOURCE_BROWSER) && Config_getBool(SOURCE_TOOLTIPS))
      {
        t << endl <<
          "$(document).ready(function() {\n"
          "  $('.code,.codeRef').each(function() {\n"
          "    $(this).data('powertip',$('#'+$(this).attr('href').replace(/.*\\//,'').replace(/[^a-z_A-Z0-9]/g,'_')).html());\n"
          "    $(this).powerTip({ placement: 's', smartPlacement: true, mouseOnToPopup: true });\n"
          "  });\n"
          "});\n";
      }
    }
  }
}

/// Additional initialization after indices have been created
void HtmlGenerator::writeTabData()
{
  Doxygen::indexList->addStyleSheetFile("tabs.css");
  QCString dname=Config_getString(HTML_OUTPUT);
  ResourceMgr &mgr = ResourceMgr::instance();
  //writeColoredImgData(dname,colored_tab_data);
  mgr.copyResource("tab_a.lum",dname);
  mgr.copyResource("tab_b.lum",dname);
  mgr.copyResource("tab_h.lum",dname);
  mgr.copyResource("tab_s.lum",dname);
  mgr.copyResource("nav_h.lum",dname);
  mgr.copyResource("nav_f.lum",dname);
  mgr.copyResource("bc_s.luma",dname);
  mgr.copyResource("doxygen.luma",dname);
  mgr.copyResource("closed.luma",dname);
  mgr.copyResource("open.luma",dname);
  mgr.copyResource("bdwn.luma",dname);
  mgr.copyResource("sync_on.luma",dname);
  mgr.copyResource("sync_off.luma",dname);

  //{
  //  unsigned char shadow[6] = { 5, 5, 5, 5, 5, 5 };
  //  unsigned char shadow_alpha[6]  = { 80, 60, 40, 20, 10, 0 };
  //  ColoredImage img(1,6,shadow,shadow_alpha,0,0,100);
  //  img.save(dname+"/nav_g.png");
  //}
  mgr.copyResource("nav_g.png",dname);
}

void HtmlGenerator::writeSearchData(const char *dir)
{
  static bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
  //writeImgData(dir,serverBasedSearch ? search_server_data : search_client_data);
  ResourceMgr &mgr = ResourceMgr::instance();

  mgr.copyResource("search_l.png",dir);
  Doxygen::indexList->addImageFile("search/search_l.png");
  mgr.copyResource("search_m.png",dir);
  Doxygen::indexList->addImageFile("search/search_m.png");
  mgr.copyResource("search_r.png",dir);
  Doxygen::indexList->addImageFile("search/search_r.png");
  if (serverBasedSearch)
  {
    mgr.copyResource("mag.png",dir);
    Doxygen::indexList->addImageFile("search/mag.png");
  }
  else
  {
    mgr.copyResource("close.png",dir);
    Doxygen::indexList->addImageFile("search/close.png");
    mgr.copyResource("mag_sel.png",dir);
    Doxygen::indexList->addImageFile("search/mag_sel.png");
  }

  QCString searchDirName = Config_getString(HTML_OUTPUT)+"/search";
  QFile f(searchDirName+"/search.css");
  if (f.open(IO_WriteOnly))
  {
    FTextStream t(&f);
    QCString searchCss = replaceColorMarkers(mgr.getAsString("search.css"));
    searchCss = substitute(searchCss,"$doxygenversion",versionString);
    if (Config_getBool(DISABLE_INDEX))
    {
      // move up the search box if there are no tabs
      searchCss = substitute(searchCss,"margin-top: 8px;","margin-top: 0px;");
    }
    t << searchCss;
    Doxygen::indexList->addStyleSheetFile("search/search.css");
  }
}

void HtmlGenerator::writeStyleSheetFile(QFile &file)
{
  FTextStream t(&file);
  t << replaceColorMarkers(substitute(ResourceMgr::instance().getAsString("doxygen.css"),"$doxygenversion",versionString));
}

void HtmlGenerator::writeHeaderFile(QFile &file, const char * /*cssname*/)
{
  FTextStream t(&file);
  t << "<!-- HTML header for doxygen " << versionString << "-->" << endl;
  t << ResourceMgr::instance().getAsString("header.html");
}

void HtmlGenerator::writeFooterFile(QFile &file)
{
  FTextStream t(&file);
  t << "<!-- HTML footer for doxygen " << versionString << "-->" <<  endl;
  t << ResourceMgr::instance().getAsString("footer.html");
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
  m_codeGen.setTextStream(t);
  m_codeGen.setRelativePath(relPath);
  Doxygen::indexList->addIndexFile(fileName);
  
  lastFile = fileName;
  t << substituteHtmlKeywords(g_header,convertToHtml(filterTitle(title)),relPath);

  t << "<!-- " << theTranslator->trGeneratedBy() << " Doxygen " 
    << versionString << " -->" << endl;
  //static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  static bool searchEngine = Config_getBool(SEARCHENGINE);
  if (searchEngine /*&& !generateTreeView*/)
  {
    t << "<script type=\"text/javascript\">\n";
    t << "var searchBox = new SearchBox(\"searchBox\", \""
      << relPath<< "search\",false,'" << theTranslator->trSearch() << "');\n";
    t << "</script>\n";
  }
  //generateDynamicSections(t,relPath);
  m_sectionCount=0;
}

void HtmlGenerator::writeSearchInfo(FTextStream &t,const QCString &relPath)
{
  static bool searchEngine      = Config_getBool(SEARCHENGINE);
  static bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
  if (searchEngine && !serverBasedSearch)
  {
    (void)relPath;
    t << "<!-- window showing the filter options -->\n";
    t << "<div id=\"MSearchSelectWindow\"\n";
    t << "     onmouseover=\"return searchBox.OnSearchSelectShow()\"\n";
    t << "     onmouseout=\"return searchBox.OnSearchSelectHide()\"\n";
    t << "     onkeydown=\"return searchBox.OnSearchSelectKey(event)\">\n";
    t << "</div>\n";
    t << "\n";
    t << "<!-- iframe showing the search results (closed by default) -->\n";
    t << "<div id=\"MSearchResultsWindow\">\n";
    t << "<iframe src=\"javascript:void(0)\" frameborder=\"0\" \n";
    t << "        name=\"MSearchResults\" id=\"MSearchResults\">\n";
    t << "</iframe>\n";
    t << "</div>\n";
    t << "\n";
  }
}

void HtmlGenerator::writeSearchInfo()
{
  writeSearchInfo(t,relPath);
}


QCString HtmlGenerator::writeLogoAsString(const char *path)
{
  static bool timeStamp = Config_getBool(HTML_TIMESTAMP);
  QCString result;
  if (timeStamp)
  {
    result += theTranslator->trGeneratedAt(
               dateToString(TRUE),
               Config_getString(PROJECT_NAME)
              );
  }
  else
  {
    result += theTranslator->trGeneratedBy();
  }
  result += "&#160;\n<a href=\"http://www.doxygen.org/index.html\">\n"
            "<img class=\"footer\" src=\"";
  result += path;
  result += "doxygen.png\" alt=\"doxygen\"/></a> ";
  result += versionString;
  result += " ";
  return result;
}

void HtmlGenerator::writeLogo()
{
  t << writeLogoAsString(relPath);
}

void HtmlGenerator::writePageFooter(FTextStream &t,const QCString &lastTitle,
                              const QCString &relPath,const QCString &navPath)
{
  t << substituteHtmlKeywords(g_footer,convertToHtml(lastTitle),relPath,navPath);
}

void HtmlGenerator::writeFooter(const char *navPath)
{
  writePageFooter(t,lastTitle,relPath,navPath);
}

void HtmlGenerator::endFile()
{
  endPlainFile();
}

void HtmlGenerator::startProjectNumber()
{
  t << "<h3 class=\"version\">";
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
    if (Config_getString(HTML_STYLESHEET).isEmpty()) // write default style sheet
    {
      //printf("write doxygen.css\n");
      startPlainFile("doxygen.css"); 
      
      // alternative, cooler looking titles
      //t << "H1 { text-align: center; border-width: thin none thin none;" << endl;
      //t << "     border-style : double; border-color : blue; padding-left : 1em; padding-right : 1em }" << endl;

      t << replaceColorMarkers(substitute(ResourceMgr::instance().getAsString("doxygen.css"),"$doxygenversion",versionString));
      endPlainFile();
      Doxygen::indexList->addStyleSheetFile("doxygen.css");
    }
    else // write user defined style sheet
    {
      QCString cssname=Config_getString(HTML_STYLESHEET);
      QFileInfo cssfi(cssname);
      if (!cssfi.exists() || !cssfi.isFile() || !cssfi.isReadable())
      {
        err("style sheet %s does not exist or is not readable!", Config_getString(HTML_STYLESHEET).data());
      }
      else
      {
        // convert style sheet to string
        QCString fileStr = fileToString(cssname);
        // write the string into the output dir
        startPlainFile(cssfi.fileName().utf8());
        t << fileStr;
        endPlainFile();
      }
      Doxygen::indexList->addStyleSheetFile(cssfi.fileName().utf8());
    }
    static QStrList extraCssFile = Config_getList(HTML_EXTRA_STYLESHEET);
    for (uint i=0; i<extraCssFile.count(); ++i)
    {
      QCString fileName(extraCssFile.at(i));
      if (!fileName.isEmpty())
      {
        QFileInfo fi(fileName);
        if (fi.exists())
        {
          Doxygen::indexList->addStyleSheetFile(fi.fileName().utf8());
        }
      }
    }
  }
}

void HtmlGenerator::startDoxyAnchor(const char *,const char *,
                                    const char *anchor, const char *,
                                    const char *)
{
  t << "<a class=\"anchor\" id=\"" << anchor << "\"></a>";
}

void HtmlGenerator::endDoxyAnchor(const char *,const char *)
{
}

//void HtmlGenerator::newParagraph()
//{
//  t << endl << "<p>" << endl;
//}

void HtmlGenerator::startParagraph()
{
  t << endl << "<p>";
}

void HtmlGenerator::endParagraph()
{
  t << "</p>" << endl;
}

void HtmlGenerator::writeString(const char *text)
{
  t << text;
}

void HtmlGenerator::startIndexListItem()
{
  t << "<li>";
}

void HtmlGenerator::endIndexListItem()
{
  t << "</li>" << endl;
}

void HtmlGenerator::startIndexItem(const char *ref,const char *f)
{
  //printf("HtmlGenerator::startIndexItem(%s,%s)\n",ref,f);
  if (ref || f)
  {
    if (ref) 
    {
      t << "<a class=\"elRef\" ";
      t << externalLinkTarget() << externalRef(relPath,ref,FALSE);
    }
    else
    {
      t << "<a class=\"el\" ";
    }
    t << "href=\"";
    t << externalRef(relPath,ref,TRUE);
    if (f) t << f << Doxygen::htmlFileExtension << "\">";
  }
  else
  {
    t << "<b>";
  }
}

void HtmlGenerator::endIndexItem(const char *ref,const char *f)
{
  //printf("HtmlGenerator::endIndexItem(%s,%s,%s)\n",ref,f,name);
  if (ref || f)
  {
    t << "</a>";
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
  if (ref) 
  {
    t << "<a class=\"elRef\" ";
    t << externalLinkTarget() << externalRef(relPath,ref,FALSE);
  }
  else
  {
    t << "<a class=\"el\" ";
  }
  t << "href=\"";
  t << externalRef(relPath,ref,TRUE);
  if (f) t << f << Doxygen::htmlFileExtension;
  if (anchor) t << "#" << anchor;
  t << "\">";
  docify(name);
  t << "</a>";
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
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  t << "<a ";
  if (generateTreeView) t << "target=\"top\" ";
  t << "href=\"";
  if (url) t << url;
  t << "\">"; 
}

void HtmlGenerator::endHtmlLink()
{
  t << "</a>";
}

void HtmlGenerator::startGroupHeader(int extraIndentLevel)
{
  if (extraIndentLevel==2)
  {
    t << "<h4 class=\"groupheader\">";
  }
  else if (extraIndentLevel==1)
  {
    t << "<h3 class=\"groupheader\">";
  }
  else // extraIndentLevel==0
  {
    t << "<h2 class=\"groupheader\">";
  }
}

void HtmlGenerator::endGroupHeader(int extraIndentLevel)
{
  if (extraIndentLevel==2)
  {
    t << "</h4>" << endl;
  }
  else if (extraIndentLevel==1)
  {
    t << "</h3>" << endl;
  }
  else
  {
    t << "</h2>" << endl;
  }
}

void HtmlGenerator::startSection(const char *lab,const char *,SectionInfo::SectionType type)
{
  switch(type)
  {
    case SectionInfo::Page:          t << "\n\n<h1>"; break;
    case SectionInfo::Section:       t << "\n\n<h2>"; break;
    case SectionInfo::Subsection:    t << "\n\n<h3>"; break;
    case SectionInfo::Subsubsection: t << "\n\n<h4>"; break;
    case SectionInfo::Paragraph:     t << "\n\n<h5>"; break;
    default: ASSERT(0); break;
  }
  t << "<a class=\"anchor\" id=\"" << lab << "\"></a>";
}

void HtmlGenerator::endSection(const char *,SectionInfo::SectionType type)
{
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
  docify(str,FALSE);
}

void HtmlGenerator::docify(const char *str,bool inHtmlComment)
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
        case '"':  t << "&quot;"; break;
        case '-':  if (inHtmlComment) t << "&#45;"; else t << "-"; break;
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

void HtmlGenerator::writeChar(char c)
{
  char cs[2];
  cs[0]=c;
  cs[1]=0;
  docify(cs);
}

//--- helper function for dynamic sections -------------------------

static void startSectionHeader(FTextStream &t,
                               const QCString &relPath,int sectionCount)
{
  //t << "<!-- startSectionHeader -->";
  static bool dynamicSections = Config_getBool(HTML_DYNAMIC_SECTIONS);
  if (dynamicSections)
  {
    t << "<div id=\"dynsection-" << sectionCount << "\" "
         "onclick=\"return toggleVisibility(this)\" "
         "class=\"dynheader closed\" "
         "style=\"cursor:pointer;\">" << endl;
    t << "  <img id=\"dynsection-" << sectionCount << "-trigger\" src=\"" 
      << relPath << "closed.png\" alt=\"+\"/> ";
  }
  else
  {
    t << "<div class=\"dynheader\">" << endl;
  }
}

static void endSectionHeader(FTextStream &t)
{
  //t << "<!-- endSectionHeader -->";
  t << "</div>" << endl;
}

static void startSectionSummary(FTextStream &t,int sectionCount)
{
  //t << "<!-- startSectionSummary -->";
  static bool dynamicSections = Config_getBool(HTML_DYNAMIC_SECTIONS);
  if (dynamicSections)
  {
    t << "<div id=\"dynsection-" << sectionCount << "-summary\" "
         "class=\"dynsummary\" "
         "style=\"display:block;\">" << endl;
  }
}

static void endSectionSummary(FTextStream &t)
{
  //t << "<!-- endSectionSummary -->";
  static bool dynamicSections = Config_getBool(HTML_DYNAMIC_SECTIONS);
  if (dynamicSections)
  {
    t << "</div>" << endl;
  }
}

static void startSectionContent(FTextStream &t,int sectionCount)
{
  //t << "<!-- startSectionContent -->";
  static bool dynamicSections = Config_getBool(HTML_DYNAMIC_SECTIONS);
  if (dynamicSections)
  {
    t << "<div id=\"dynsection-" << sectionCount << "-content\" "
         "class=\"dyncontent\" "
         "style=\"display:none;\">" << endl;
  }
  else
  {
    t << "<div class=\"dyncontent\">" << endl;
  }
}

static void endSectionContent(FTextStream &t)
{
  //t << "<!-- endSectionContent -->";
  t << "</div>" << endl;
}

//----------------------------

void HtmlGenerator::startClassDiagram()
{
  startSectionHeader(t,relPath,m_sectionCount);
}

void HtmlGenerator::endClassDiagram(const ClassDiagram &d,
                                const char *fileName,const char *name)
{
  endSectionHeader(t);
  startSectionSummary(t,m_sectionCount);
  endSectionSummary(t);
  startSectionContent(t,m_sectionCount);
  t << " <div class=\"center\">" << endl;
  t << "  <img src=\"";
  t << relPath << fileName << ".png\" usemap=\"#" << convertToId(name);
  t << "_map\" alt=\"\"/>" << endl;
  t << "  <map id=\"" << convertToId(name);
  t << "_map\" name=\"";
  docify(name);
  t << "_map\">" << endl;

  d.writeImage(t,dir,relPath,fileName);
  t << " </div>";
  endSectionContent(t);
  m_sectionCount++;
}


void HtmlGenerator::startMemberList()  
{ 
  DBG_HTML(t << "<!-- startMemberList -->" << endl)
}

void HtmlGenerator::endMemberList()    
{ 
  DBG_HTML(t << "<!-- endMemberList -->" << endl)
}

// anonymous type:
//  0 = single column right aligned
//  1 = double column left aligned
//  2 = single column left aligned
void HtmlGenerator::startMemberItem(const char *anchor,int annoType,const char *inheritId) 
{ 
  DBG_HTML(t << "<!-- startMemberItem() -->" << endl)
  if (m_emptySection)
  {
    t << "<table class=\"memberdecls\">" << endl;
    m_emptySection=FALSE;
  }
  t << "<tr class=\"memitem:" << anchor;
  if (inheritId)
  {
    t << " inherit " << inheritId;
  }
  t << "\">";
  switch(annoType)
  {
    case 0:  t << "<td class=\"memItemLeft\" align=\"right\" valign=\"top\">"; break;
    case 1:  t << "<td class=\"memItemLeft\" >"; break;
    case 2:  t << "<td class=\"memItemLeft\" valign=\"top\">"; break;
    default: t << "<td class=\"memTemplParams\" colspan=\"2\">"; break;
  }
}

void HtmlGenerator::endMemberItem() 
{ 
  t << "</td></tr>"; 
  t << endl; 
}

void HtmlGenerator::startMemberTemplateParams()
{
}

void HtmlGenerator::endMemberTemplateParams(const char *anchor,const char *inheritId)
{
  t << "</td></tr>" << endl;
  t << "<tr class=\"memitem:" << anchor;
  if (inheritId)
  {
    t << " inherit " << inheritId;
  }
  t << "\"><td class=\"memTemplItemLeft\" align=\"right\" valign=\"top\">";
}


void HtmlGenerator::insertMemberAlign(bool templ) 
{ 
  DBG_HTML(t << "<!-- insertMemberAlign -->" << endl)
  QCString className = templ ? "memTemplItemRight" : "memItemRight";
  t << "&#160;</td><td class=\"" << className << "\" valign=\"bottom\">"; 
}

void HtmlGenerator::startMemberDescription(const char *anchor,const char *inheritId) 
{ 
  DBG_HTML(t << "<!-- startMemberDescription -->" << endl)
    if (m_emptySection)
    {
      t << "<table class=\"memberdecls\">" << endl;
      m_emptySection=FALSE;
    }
  t << "<tr class=\"memdesc:" << anchor;
  if (inheritId)
  {
    t << " inherit " << inheritId;
  }
  t << "\"><td class=\"mdescLeft\">&#160;</td><td class=\"mdescRight\">"; 
}

void HtmlGenerator::endMemberDescription()   
{ 
  DBG_HTML(t << "<!-- endMemberDescription -->" << endl)
  t << "<br /></td></tr>" << endl; 
}

void HtmlGenerator::startMemberSections()
{
  DBG_HTML(t << "<!-- startMemberSections -->" << endl)
  m_emptySection=TRUE; // we postpone writing <table> until we actually
                       // write a row to prevent empty tables, which 
                       // are not valid XHTML!
}

void HtmlGenerator::endMemberSections()
{
  DBG_HTML(t << "<!-- endMemberSections -->" << endl)
  if (!m_emptySection)
  {
    t << "</table>" << endl;
  }
}

void HtmlGenerator::startMemberHeader(const char *anchor)
{
  DBG_HTML(t << "<!-- startMemberHeader -->" << endl)
  if (!m_emptySection)
  {
    t << "</table>";
    m_emptySection=TRUE;
  }
  if (m_emptySection)
  {
    t << "<table class=\"memberdecls\">" << endl;
    m_emptySection=FALSE;
  }
  t << "<tr class=\"heading\"><td colspan=\"2\"><h2 class=\"groupheader\">";
  if (anchor)
  {
    t << "<a name=\"" << anchor << "\"></a>" << endl;
  }
}

void HtmlGenerator::endMemberHeader()
{
  DBG_HTML(t << "<!-- endMemberHeader -->" << endl)
  t << "</h2></td></tr>" << endl;
}

void HtmlGenerator::startMemberSubtitle()
{
  DBG_HTML(t << "<!-- startMemberSubtitle -->" << endl)
  t << "<tr><td class=\"ititle\" colspan=\"2\">";
}

void HtmlGenerator::endMemberSubtitle()
{
  DBG_HTML(t << "<!-- endMemberSubtitle -->" << endl)
  t << "</td></tr>" << endl;
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

void HtmlGenerator::startMemberDocList()
{
  DBG_HTML(t << "<!-- startMemberDocList -->" << endl;)
}

void HtmlGenerator::endMemberDocList()
{
  DBG_HTML(t << "<!-- endMemberDocList -->" << endl;)
}

void HtmlGenerator::startMemberDoc(const char *,const char *,const char *,const char *,bool) 
{ 
  DBG_HTML(t << "<!-- startMemberDoc -->" << endl;)
 
  t << "\n<div class=\"memitem\">" << endl;
  t << "<div class=\"memproto\">" << endl;
}

void HtmlGenerator::startMemberDocPrefixItem()
{
  DBG_HTML(t << "<!-- startMemberDocPrefixItem -->" << endl;)
  t << "<div class=\"memtemplate\">" << endl;
}

void HtmlGenerator::endMemberDocPrefixItem()
{
  DBG_HTML(t << "<!-- endMemberDocPrefixItem -->" << endl;)
  t << "</div>" << endl;
}

void HtmlGenerator::startMemberDocName(bool /*align*/)
{
  DBG_HTML(t << "<!-- startMemberDocName -->" << endl;)

  t << "      <table class=\"memname\">" << endl;
    
  t << "        <tr>" << endl;
  t << "          <td class=\"memname\">";
}

void HtmlGenerator::endMemberDocName()
{
  DBG_HTML(t << "<!-- endMemberDocName -->" << endl;)
  t << "</td>" << endl;
}

void HtmlGenerator::startParameterList(bool openBracket)
{
  DBG_HTML(t << "<!-- startParameterList -->" << endl;)
  t << "          <td>";
  if (openBracket) t << "(";
  t << "</td>" << endl;
}

void HtmlGenerator::startParameterType(bool first,const char *key)
{
  if (first)
  {
    DBG_HTML(t << "<!-- startFirstParameterType -->" << endl;)
    t << "          <td class=\"paramtype\">";
  }
  else
  {
    DBG_HTML(t << "<!-- startParameterType -->" << endl;)
    t << "        <tr>" << endl;
    t << "          <td class=\"paramkey\">";
    if (key) t << key;
    t << "</td>" << endl;
    t << "          <td></td>" << endl;
    t << "          <td class=\"paramtype\">";
  }
}

void HtmlGenerator::endParameterType()
{
  DBG_HTML(t << "<!-- endParameterType -->" << endl;)
  t << "&#160;</td>" << endl;
}

void HtmlGenerator::startParameterName(bool /*oneArgOnly*/)
{
  DBG_HTML(t << "<!-- startParameterName -->" << endl;)
  t << "          <td class=\"paramname\">";
}

void HtmlGenerator::endParameterName(bool last,bool emptyList,bool closeBracket)
{
  DBG_HTML(t << "<!-- endParameterName -->" << endl;)
  if (last)
  {
    if (emptyList)
    {
      if (closeBracket) t << "</td><td>)";
      t << "</td>" << endl;
      t << "          <td>";
    }
    else
    {
      t << "&#160;</td>" << endl;
      t << "        </tr>" << endl;
      t << "        <tr>" << endl;
      t << "          <td></td>" << endl;
      t << "          <td>";
      if (closeBracket) t << ")";
      t << "</td>" << endl;
      t << "          <td></td><td>";
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

void HtmlGenerator::exceptionEntry(const char* prefix,bool closeBracket)
{
  DBG_HTML(t << "<!-- exceptionEntry -->" << endl;)
  t << "</td>" << endl;
  t << "        </tr>" << endl;
  t << "        <tr>" << endl;
  t << "          <td align=\"right\">";
  // colspan 2 so it gets both parameter type and parameter name columns
  if (prefix)
    t << prefix << "</td><td>(</td><td colspan=\"2\">";
  else if (closeBracket)
    t << "</td><td>)</td><td></td><td>";
  else
    t << "</td><td></td><td colspan=\"2\">";
}

void HtmlGenerator::endMemberDoc(bool hasArgs)     
{ 
  DBG_HTML(t << "<!-- endMemberDoc -->" << endl;)
  if (!hasArgs)
  {
    t << "        </tr>" << endl;
  }
  t << "      </table>" << endl;
 // t << "</div>" << endl;
}

void HtmlGenerator::startDotGraph()
{
  startSectionHeader(t,relPath,m_sectionCount);
}

void HtmlGenerator::endDotGraph(const DotClassGraph &g)
{
  bool generateLegend = Config_getBool(GENERATE_LEGEND);
  bool umlLook = Config_getBool(UML_LOOK);
  endSectionHeader(t);
  startSectionSummary(t,m_sectionCount);
  endSectionSummary(t);
  startSectionContent(t,m_sectionCount);

  g.writeGraph(t,GOF_BITMAP,EOF_Html,dir,fileName,relPath,TRUE,TRUE,m_sectionCount);
  if (generateLegend && !umlLook)
  {
    t << "<center><span class=\"legend\">[";
    startHtmlLink(relPath+"graph_legend"+Doxygen::htmlFileExtension);
    t << theTranslator->trLegend();
    endHtmlLink();
    t << "]</span></center>";
  }

  endSectionContent(t);
  m_sectionCount++;
}

void HtmlGenerator::startInclDepGraph()
{
  startSectionHeader(t,relPath,m_sectionCount);
}

void HtmlGenerator::endInclDepGraph(const DotInclDepGraph &g)
{
  endSectionHeader(t);
  startSectionSummary(t,m_sectionCount);
  endSectionSummary(t);
  startSectionContent(t,m_sectionCount);

  g.writeGraph(t,GOF_BITMAP,EOF_Html,dir,fileName,relPath,TRUE,m_sectionCount);

  endSectionContent(t);
  m_sectionCount++;
}

void HtmlGenerator::startGroupCollaboration()
{
  startSectionHeader(t,relPath,m_sectionCount);
}

void HtmlGenerator::endGroupCollaboration(const DotGroupCollaboration &g)
{
  endSectionHeader(t);
  startSectionSummary(t,m_sectionCount);
  endSectionSummary(t);
  startSectionContent(t,m_sectionCount);

  g.writeGraph(t,GOF_BITMAP,EOF_Html,dir,fileName,relPath,TRUE,m_sectionCount);

  endSectionContent(t);
  m_sectionCount++;
}

void HtmlGenerator::startCallGraph()
{
  startSectionHeader(t,relPath,m_sectionCount);
}

void HtmlGenerator::endCallGraph(const DotCallGraph &g)
{
  endSectionHeader(t);
  startSectionSummary(t,m_sectionCount);
  endSectionSummary(t);
  startSectionContent(t,m_sectionCount);

  g.writeGraph(t,GOF_BITMAP,EOF_Html,dir,fileName,relPath,TRUE,m_sectionCount);

  endSectionContent(t);
  m_sectionCount++;
}

void HtmlGenerator::startDirDepGraph()
{
  startSectionHeader(t,relPath,m_sectionCount);
}

void HtmlGenerator::endDirDepGraph(const DotDirDeps &g)
{
  endSectionHeader(t);
  startSectionSummary(t,m_sectionCount);
  endSectionSummary(t);
  startSectionContent(t,m_sectionCount);

  g.writeGraph(t,GOF_BITMAP,EOF_Html,dir,fileName,relPath,TRUE,m_sectionCount);

  endSectionContent(t);
  m_sectionCount++;
}

void HtmlGenerator::writeGraphicalHierarchy(const DotGfxHierarchyTable &g)
{
  g.writeGraph(t,dir,fileName);
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
  t << "</div></td></tr>" << endl;
}

void HtmlGenerator::startMemberGroup()
{
}

void HtmlGenerator::endMemberGroup(bool)
{
}

void HtmlGenerator::startIndent()        
{ 
  DBG_HTML(t << "<!-- startIndent -->" << endl;)

  t << "<div class=\"memdoc\">\n";
}

void HtmlGenerator::endIndent()          
{ 
  DBG_HTML(t << "<!-- endIndent -->" << endl;)
  t << endl << "</div>" << endl << "</div>" << endl; 
}

void HtmlGenerator::addIndexItem(const char *,const char *)
{
}

void HtmlGenerator::writeNonBreakableSpace(int n)
{
  int i;
  for (i=0;i<n;i++)
  {
    t << "&#160;";
  }
}

void HtmlGenerator::startSimpleSect(SectionTypes,
                                const char *filename,const char *anchor,
                                const char *title)
{
  t << "<dl><dt><b>";
  if (filename)
  {
    writeObjectLink(0,filename,anchor,title);
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
  t << "<dl><dt><b>";
  docify(title);
  t << "</b></dt>";
}

void HtmlGenerator::endParamList()
{
  t << "</dl>"; 
}

void HtmlGenerator::writeDoc(DocNode *n,Definition *ctx,MemberDef *)
{
  HtmlDocVisitor *visitor = new HtmlDocVisitor(t,m_codeGen,ctx);
  n->accept(visitor);
  delete visitor; 
}

//---------------- helpers for index generation -----------------------------

static void startQuickIndexList(FTextStream &t,bool compact,bool topLevel=TRUE)
{
  if (compact) 
  {
    if (topLevel)
    {
      t << "  <div id=\"navrow1\" class=\"tabs\">\n";
    }
    else
    {
      t << "  <div id=\"navrow2\" class=\"tabs2\">\n";
    }
    t << "    <ul class=\"tablist\">\n"; 
  }
  else 
  {
    t << "<ul>";
  }
}

static void endQuickIndexList(FTextStream &t,bool compact)
{
  if (compact) 
  {
    t << "    </ul>\n";
    t << "  </div>\n";
  }
  else 
  {
    t << "</ul>\n";
  }
}

static void startQuickIndexItem(FTextStream &t,const char *l,
                                bool hl,bool /*compact*/,
                                const QCString &relPath)
{
  t << "      <li"; 
  if (hl) 
  {
    t << " class=\"current\"";
  }
  t << ">";
  if (l) t << "<a href=\"" << correctURL(l,relPath) << "\">";
  t << "<span>";
}

static void endQuickIndexItem(FTextStream &t,const char *l)
{
  t << "</span>";
  if (l) t << "</a>";
  t << "</li>\n";
}

static bool quickLinkVisible(LayoutNavEntry::Kind kind)
{
  static bool showFiles = Config_getBool(SHOW_FILES);
  static bool showNamespaces = Config_getBool(SHOW_NAMESPACES);
  switch (kind)
  {
    case LayoutNavEntry::MainPage:         return TRUE; 
    case LayoutNavEntry::User:             return TRUE;                                           
    case LayoutNavEntry::UserGroup:        return TRUE;                                           
    case LayoutNavEntry::Pages:            return indexedPages>0;
    case LayoutNavEntry::Modules:          return documentedGroups>0;
    case LayoutNavEntry::Namespaces:       return documentedNamespaces>0 && showNamespaces;
    case LayoutNavEntry::NamespaceList:    return documentedNamespaces>0 && showNamespaces;
    case LayoutNavEntry::NamespaceMembers: return documentedNamespaceMembers[NMHL_All]>0;
    case LayoutNavEntry::Classes:          return annotatedClasses>0;
    case LayoutNavEntry::ClassList:        return annotatedClasses>0; 
    case LayoutNavEntry::ClassIndex:       return annotatedClasses>0; 
    case LayoutNavEntry::ClassHierarchy:   return hierarchyClasses>0;
    case LayoutNavEntry::ClassMembers:     return documentedClassMembers[CMHL_All]>0;
    case LayoutNavEntry::Files:            return documentedHtmlFiles>0 && showFiles;
    case LayoutNavEntry::FileList:         return documentedHtmlFiles>0 && showFiles;
    case LayoutNavEntry::FileGlobals:      return documentedFileMembers[FMHL_All]>0;
    //case LayoutNavEntry::Dirs:             return documentedDirs>0;
    case LayoutNavEntry::Examples:         return Doxygen::exampleSDict->count()>0;
  }
  return FALSE;
}

static void renderQuickLinksAsTree(FTextStream &t,const QCString &relPath,LayoutNavEntry *root)

{
  QListIterator<LayoutNavEntry> li(root->children());
  LayoutNavEntry *entry;
  int count=0;
  for (li.toFirst();(entry=li.current());++li)
  {
    if (entry->visible() && quickLinkVisible(entry->kind())) count++;
  }
  if (count>0) // at least one item is visible
  {
    startQuickIndexList(t,FALSE);
    for (li.toFirst();(entry=li.current());++li)
    {
      if (entry->visible() && quickLinkVisible(entry->kind()))
      {
        QCString url = entry->url();
        t << "<li><a href=\"" << relPath << url << "\"><span>";
        t << fixSpaces(entry->title());
        t << "</span></a>\n";
        // recursive into child list
        renderQuickLinksAsTree(t,relPath,entry);
        t << "</li>";
      }
    }
    endQuickIndexList(t,FALSE);
  }
}


static void renderQuickLinksAsTabs(FTextStream &t,const QCString &relPath,
                             LayoutNavEntry *hlEntry,LayoutNavEntry::Kind kind,
                             bool highlightParent,bool highlightSearch)
{
  if (hlEntry->parent()) // first draw the tabs for the parent of hlEntry
  {
    renderQuickLinksAsTabs(t,relPath,hlEntry->parent(),kind,highlightParent,highlightSearch);
  }
  if (hlEntry->parent() && hlEntry->parent()->children().count()>0) // draw tabs for row containing hlEntry
  {
    bool topLevel = hlEntry->parent()->parent()==0;
    QListIterator<LayoutNavEntry> li(hlEntry->parent()->children());
    LayoutNavEntry *entry;

    int count=0;
    for (li.toFirst();(entry=li.current());++li)
    {
      if (entry->visible() && quickLinkVisible(entry->kind())) count++;
    }
    if (count>0) // at least one item is visible
    {
      startQuickIndexList(t,TRUE,topLevel);
      for (li.toFirst();(entry=li.current());++li)
      {
        if (entry->visible() && quickLinkVisible(entry->kind()))
        {
          QCString url = entry->url();
          startQuickIndexItem(t,url,
              entry==hlEntry  && 
              (entry->children().count()>0 || 
               (entry->kind()==kind && !highlightParent)
              ),
              TRUE,relPath);
          t << fixSpaces(entry->title());
          endQuickIndexItem(t,url);
        }
      }
      if (hlEntry->parent()==LayoutDocManager::instance().rootNavEntry()) // first row is special as it contains the search box
      {
        static bool searchEngine      = Config_getBool(SEARCHENGINE);
        static bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
        if (searchEngine)
        {
          t << "      <li>\n";
          if (!serverBasedSearch) // pure client side search
          {
            writeClientSearchBox(t,relPath);
            t << "      </li>\n";
          }
          else // server based search
          {
            writeServerSearchBox(t,relPath,highlightSearch);
            if (!highlightSearch)
            {
              t << "      </li>\n";
            }
          }
        }
        if (!highlightSearch) // on the search page the index will be ended by the
          // page itself
        {
          endQuickIndexList(t,TRUE);
        }
      }
      else // normal case for other rows than first one
      {
        endQuickIndexList(t,TRUE);
      }
    }
  }
}

static void writeDefaultQuickLinks(FTextStream &t,bool compact,
                                   HighlightedItem hli,
                                   const char *file,
                                   const QCString &relPath)
{
  LayoutNavEntry *root = LayoutDocManager::instance().rootNavEntry();
  LayoutNavEntry::Kind kind = (LayoutNavEntry::Kind)-1;
  LayoutNavEntry::Kind altKind = (LayoutNavEntry::Kind)-1; // fall back for the old layout file
  bool highlightParent=FALSE;
  switch (hli) // map HLI enums to LayoutNavEntry::Kind enums
  {
    case HLI_Main:             kind = LayoutNavEntry::MainPage;         break;
    case HLI_Modules:          kind = LayoutNavEntry::Modules;          break;
    //case HLI_Directories:      kind = LayoutNavEntry::Dirs;             break;
    case HLI_Namespaces:       kind = LayoutNavEntry::NamespaceList;    altKind = LayoutNavEntry::Namespaces;  break;
    case HLI_Hierarchy:        kind = LayoutNavEntry::ClassHierarchy;   break;
    case HLI_Classes:          kind = LayoutNavEntry::ClassIndex;       altKind = LayoutNavEntry::Classes;     break;
    case HLI_Annotated:        kind = LayoutNavEntry::ClassList;        altKind = LayoutNavEntry::Classes;     break;
    case HLI_Files:            kind = LayoutNavEntry::FileList;         altKind = LayoutNavEntry::Files;       break;
    case HLI_NamespaceMembers: kind = LayoutNavEntry::NamespaceMembers; break;
    case HLI_Functions:        kind = LayoutNavEntry::ClassMembers;     break;
    case HLI_Globals:          kind = LayoutNavEntry::FileGlobals;      break;
    case HLI_Pages:            kind = LayoutNavEntry::Pages;            break;
    case HLI_Examples:         kind = LayoutNavEntry::Examples;         break;
    case HLI_UserGroup:        kind = LayoutNavEntry::UserGroup;        break;
    case HLI_ClassVisible:     kind = LayoutNavEntry::ClassList;        altKind = LayoutNavEntry::Classes;          
                               highlightParent = TRUE; break;
    case HLI_NamespaceVisible: kind = LayoutNavEntry::NamespaceList;    altKind = LayoutNavEntry::Namespaces;       
                               highlightParent = TRUE; break;
    case HLI_FileVisible:      kind = LayoutNavEntry::FileList;         altKind = LayoutNavEntry::Files;            
                               highlightParent = TRUE; break;
    case HLI_None:   break;
    case HLI_Search: break;
  }
  
  if (compact)
  {
    // find highlighted index item
    LayoutNavEntry *hlEntry = root->find(kind,kind==LayoutNavEntry::UserGroup ? file : 0);
    if (!hlEntry && altKind!=(LayoutNavEntry::Kind)-1) { hlEntry=root->find(altKind); kind=altKind; }
    if (!hlEntry) // highlighted item not found in the index! -> just show the level 1 index...
    {
      highlightParent=TRUE;
      hlEntry = root->children().getFirst();
      if (hlEntry==0) 
      {
        return; // argl, empty index!
      }
    }
    if (kind==LayoutNavEntry::UserGroup)
    {
      LayoutNavEntry *e = hlEntry->children().getFirst();
      if (e)
      {
        hlEntry = e; 
      }
    }
    renderQuickLinksAsTabs(t,relPath,hlEntry,kind,highlightParent,hli==HLI_Search);
  }
  else
  {
    renderQuickLinksAsTree(t,relPath,root);
  }
}

void HtmlGenerator::endQuickIndices()
{
  t << "</div><!-- top -->" << endl;
}

QCString HtmlGenerator::writeSplitBarAsString(const char *name,const char *relpath)
{
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  QCString result;
  // write split bar
  if (generateTreeView)
  {
    result = QCString(
    "<div id=\"side-nav\" class=\"ui-resizable side-nav-resizable\">\n"
    "  <div id=\"nav-tree\">\n"
    "    <div id=\"nav-tree-contents\">\n"
    "      <div id=\"nav-sync\" class=\"sync\"></div>\n"
    "    </div>\n"
    "  </div>\n"
    "  <div id=\"splitbar\" style=\"-moz-user-select:none;\" \n"
    "       class=\"ui-resizable-handle\">\n"
    "  </div>\n"
    "</div>\n"
    "<script type=\"text/javascript\">\n"
    "$(document).ready(function(){initNavTree('") + 
    QCString(name) + Doxygen::htmlFileExtension + 
    QCString("','") + relpath +
    QCString("');});\n"
    "</script>\n"
    "<div id=\"doc-content\">\n");
  }
  return result;
}

void HtmlGenerator::writeSplitBar(const char *name)
{
  t << writeSplitBarAsString(name,relPath);
}

void HtmlGenerator::writeNavigationPath(const char *s)
{
  t << substitute(s,"$relpath^",relPath);
}

void HtmlGenerator::startContents()
{
  t << "<div class=\"contents\">" << endl;
}

void HtmlGenerator::endContents()
{
  t << "</div><!-- contents -->" << endl;
}

void HtmlGenerator::writeQuickLinks(bool compact,HighlightedItem hli,const char *file)
{
  writeDefaultQuickLinks(t,compact,hli,file,relPath);
}

// PHP based search script
void HtmlGenerator::writeSearchPage()
{
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  static bool disableIndex = Config_getBool(DISABLE_INDEX);
  static QCString projectName = Config_getString(PROJECT_NAME);
  static QCString htmlOutput = Config_getString(HTML_OUTPUT);

  // OPENSEARCH_PROVIDER {
  QCString configFileName = htmlOutput+"/search_config.php";
  QFile cf(configFileName);
  if (cf.open(IO_WriteOnly))
  {
    FTextStream t(&cf);
    t << "<script language=\"php\">\n\n";
    t << "$config = array(\n";
    t << "  'PROJECT_NAME' => \"" << convertToHtml(projectName) << "\",\n";
    t << "  'GENERATE_TREEVIEW' => " << (generateTreeView?"true":"false") << ",\n";
    t << "  'DISABLE_INDEX' => " << (disableIndex?"true":"false") << ",\n";
    t << ");\n\n";
    t << "$translator = array(\n";
    t << "  'search_results_title' => \"" << theTranslator->trSearchResultsTitle() << "\",\n";
    t << "  'search_results' => array(\n";  
    t << "    0 => \"" << theTranslator->trSearchResults(0) << "\",\n";     
    t << "    1 => \"" << theTranslator->trSearchResults(1) << "\",\n";     
    t << "    2 => \"" << substitute(theTranslator->trSearchResults(2), "$", "\\$") << "\",\n";     
    t << "  ),\n";
    t << "  'search_matches' => \"" << theTranslator->trSearchMatches() << "\",\n";
    t << "  'search' => \"" << theTranslator->trSearch() << "\",\n";
    t << "  'split_bar' => \"" << substitute(substitute(writeSplitBarAsString("search",""), "\"","\\\""), "\n","\\n") << "\",\n";
    t << "  'logo' => \"" << substitute(substitute(writeLogoAsString(""), "\"","\\\""), "\n","\\n") << "\",\n";
    t << ");\n\n";
    t << "</script>\n";
  }

  ResourceMgr::instance().copyResource("search_functions.php",htmlOutput);
  ResourceMgr::instance().copyResource("search_opensearch.php",htmlOutput);
  // OPENSEARCH_PROVIDER }

  QCString fileName = htmlOutput+"/search.php";
  QFile f(fileName);
  if (f.open(IO_WriteOnly))
  {
    FTextStream t(&f);
    t << substituteHtmlKeywords(g_header,"Search","");

    t << "<!-- " << theTranslator->trGeneratedBy() << " Doxygen " 
      << versionString << " -->" << endl;
    t << "<script type=\"text/javascript\">\n";
    t << "var searchBox = new SearchBox(\"searchBox\", \""
      << "search\",false,'" << theTranslator->trSearch() << "');\n";
    t << "</script>\n";
    if (!Config_getBool(DISABLE_INDEX))
    {
      writeDefaultQuickLinks(t,TRUE,HLI_Search,0,"");
    }
    else
    {
      t << "</div>" << endl;
    }

    t << "<script language=\"php\">\n";
    t << "require_once \"search_functions.php\";\n";
    t << "main();\n";
    t << "</script>\n";

    // Write empty navigation path, to make footer connect properly
    if (generateTreeView)
    {
      t << "</div><!-- doc-contents -->\n";
      //t << "<div id=\"nav-path\" class=\"navpath\">\n";
      //t << "  <ul>\n";
    }

    writePageFooter(t,"Search","","");
  }
  QCString scriptName = htmlOutput+"/search/search.js";
  QFile sf(scriptName);
  if (sf.open(IO_WriteOnly))
  {
    FTextStream t(&sf);
    t << ResourceMgr::instance().getAsString("extsearch.js");
  }
  else
  {
     err("Failed to open file '%s' for writing...\n",scriptName.data());
  }
}

void HtmlGenerator::writeExternalSearchPage()
{
  static bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  QCString fileName = Config_getString(HTML_OUTPUT)+"/search"+Doxygen::htmlFileExtension;
  QFile f(fileName);
  if (f.open(IO_WriteOnly))
  {
    FTextStream t(&f);
    t << substituteHtmlKeywords(g_header,"Search","");

    t << "<!-- " << theTranslator->trGeneratedBy() << " Doxygen " 
      << versionString << " -->" << endl;
    t << "<script type=\"text/javascript\">\n";
    t << "var searchBox = new SearchBox(\"searchBox\", \""
      << "search\",false,'" << theTranslator->trSearch() << "');\n";
    t << "</script>\n";
    if (!Config_getBool(DISABLE_INDEX))
    {
      writeDefaultQuickLinks(t,TRUE,HLI_Search,0,"");
      t << "            <input type=\"text\" id=\"MSearchField\" name=\"query\" value=\"\" size=\"20\" accesskey=\"S\" onfocus=\"searchBox.OnSearchFieldFocus(true)\" onblur=\"searchBox.OnSearchFieldFocus(false)\"/>\n";
      t << "            </form>\n";
      t << "          </div><div class=\"right\"></div>\n";
      t << "        </div>\n";
      t << "      </li>\n";
      t << "    </ul>\n";
      t << "  </div>\n";
      t << "</div>\n";
    }
    else
    {
      t << "</div>" << endl;
    }
    t << writeSplitBarAsString("search","");
    t << "<div class=\"header\">" << endl;
    t << "  <div class=\"headertitle\">" << endl;
    t << "    <div class=\"title\">" << theTranslator->trSearchResultsTitle() << "</div>" << endl;
    t << "  </div>" << endl;
    t << "</div>" << endl;
    t << "<div class=\"contents\">" << endl;

    t << "<div id=\"searchresults\"></div>" << endl;
    t << "</div>" << endl;

    if (generateTreeView)
    {
      t << "</div><!-- doc-contents -->" << endl;
    }

    writePageFooter(t,"Search","","");
  }
  QCString scriptName = Config_getString(HTML_OUTPUT)+"/search/search.js";
  QFile sf(scriptName);
  if (sf.open(IO_WriteOnly))
  {
    FTextStream t(&sf);
    t << "var searchResultsText=["
      << "\"" << theTranslator->trSearchResults(0) << "\","
      << "\"" << theTranslator->trSearchResults(1) << "\","
      << "\"" << theTranslator->trSearchResults(2) << "\"];" << endl;
    t << "var serverUrl=\"" << Config_getString(SEARCHENGINE_URL) << "\";" << endl;
    t << "var tagMap = {" << endl;
    bool first=TRUE;
    // add search mappings
    QStrList &extraSearchMappings = Config_getList(EXTRA_SEARCH_MAPPINGS);
    char *ml=extraSearchMappings.first();
    while (ml)
    {
      QCString mapLine = ml;
      int eqPos = mapLine.find('=');
      if (eqPos!=-1) // tag command contains a destination
      {
        QCString tagName = mapLine.left(eqPos).stripWhiteSpace();
        QCString destName = mapLine.right(mapLine.length()-eqPos-1).stripWhiteSpace();
        if (!tagName.isEmpty())
        {
          if (!first) t << "," << endl;
          t << "  \"" << tagName << "\": \"" << destName << "\"";
          first=FALSE;
        }
      }
      ml=extraSearchMappings.next();
    }
    if (!first) t << endl;
    t << "};" << endl << endl;
    t << ResourceMgr::instance().getAsString("extsearch.js");
    t << endl;
    t << "$(document).ready(function() {" << endl;
    t << "  var query = trim(getURLParameter('query'));" << endl;
    t << "  if (query) {" << endl;
    t << "    searchFor(query,0,20);" << endl;
    t << "  } else {" << endl;
    t << "    var results = $('#results');" << endl;
    t << "    results.html('<p>" << theTranslator->trSearchResults(0) << "</p>');" << endl;
    t << "  }" << endl;
    t << "});" << endl;
  }
  else
  {
     err("Failed to open file '%s' for writing...\n",scriptName.data());
  }
}

void HtmlGenerator::startConstraintList(const char *header)
{
  t << "<div class=\"typeconstraint\">" << endl;
  t << "<dl><dt><b>" << header << "</b></dt><dd>" << endl;
  t << "<table border=\"0\" cellspacing=\"2\" cellpadding=\"0\">" << endl;
}

void HtmlGenerator::startConstraintParam()
{
  t << "<tr><td valign=\"top\"><em>";
}

void HtmlGenerator::endConstraintParam()
{
  t << "</em></td>";
}

void HtmlGenerator::startConstraintType()
{
  t << "<td>&#160;:</td><td valign=\"top\"><em>";
}

void HtmlGenerator::endConstraintType()
{
  t << "</em></td>";
}

void HtmlGenerator::startConstraintDocs()
{
  t << "<td>&#160;";
}

void HtmlGenerator::endConstraintDocs()
{
  t << "</td></tr>" << endl;
}

void HtmlGenerator::endConstraintList()
{
  t << "</table>" << endl;
  t << "</dd>" << endl;
  t << "</dl>" << endl;
  t << "</div>" << endl;
}

void HtmlGenerator::lineBreak(const char *style)
{
  if (style)
  {
    t << "<br class=\"" << style << "\" />" << endl;
  }
  else
  {
    t << "<br />" << endl;
  }
}

void HtmlGenerator::startHeaderSection()
{
  t << "<div class=\"header\">" << endl;
}

void HtmlGenerator::startTitleHead(const char *) 
{ 
  t << "  <div class=\"headertitle\">" << endl;
  startTitle(); 
}

void HtmlGenerator::endTitleHead(const char *,const char *) 
{ 
  endTitle(); 
  t << "  </div>" << endl;
}

void HtmlGenerator::endHeaderSection()
{
  t << "</div><!--header-->" << endl;
}

void HtmlGenerator::startInlineHeader()
{
  if (m_emptySection)
  {
    t << "<table class=\"memberdecls\">" << endl;
    m_emptySection=FALSE;
  }
  t << "<tr><td colspan=\"2\"><h3>";
}

void HtmlGenerator::endInlineHeader()
{
  t << "</h3></td></tr>" << endl;
}

void HtmlGenerator::startMemberDocSimple()
{
  DBG_HTML(t << "<!-- startMemberDocSimple -->" << endl;)
  t << "<table class=\"fieldtable\">" << endl;
  t << "<tr><th colspan=\"3\">" << theTranslator->trCompoundMembers() << "</th></tr>" << endl;
}

void HtmlGenerator::endMemberDocSimple()
{
  DBG_HTML(t << "<!-- endMemberDocSimple -->" << endl;)
  t << "</table>" << endl;
}

void HtmlGenerator::startInlineMemberType()
{
  DBG_HTML(t << "<!-- startInlineMemberType -->" << endl;)
  t << "<tr><td class=\"fieldtype\">" << endl;
}

void HtmlGenerator::endInlineMemberType()
{
  DBG_HTML(t << "<!-- endInlineMemberType -->" << endl;)
  t << "</td>" << endl;
}

void HtmlGenerator::startInlineMemberName()
{
  DBG_HTML(t << "<!-- startInlineMemberName -->" << endl;)
  t << "<td class=\"fieldname\">" << endl;
}

void HtmlGenerator::endInlineMemberName()
{
  DBG_HTML(t << "<!-- endInlineMemberName -->" << endl;)
  t << "</td>" << endl;
}

void HtmlGenerator::startInlineMemberDoc()
{
  DBG_HTML(t << "<!-- startInlineMemberDoc -->" << endl;)
  t << "<td class=\"fielddoc\">" << endl;
}

void HtmlGenerator::endInlineMemberDoc()
{
  DBG_HTML(t << "<!-- endInlineMemberDoc -->" << endl;)
  t << "</td></tr>" << endl;
}

void HtmlGenerator::startLabels()
{
  DBG_HTML(t << "<!-- startLabels -->" << endl;)
  t << "<span class=\"mlabels\">";
}

void HtmlGenerator::writeLabel(const char *l,bool /*isLast*/)
{
  DBG_HTML(t << "<!-- writeLabel(" << l << ") -->" << endl;)
  //t << "<tt>[" << l << "]</tt>";
  //if (!isLast) t << ", ";
  t << "<span class=\"mlabel\">" << l << "</span>";
}

void HtmlGenerator::endLabels()
{
  DBG_HTML(t << "<!-- endLabels -->" << endl;)
  t << "</span>";
}

void HtmlGenerator::writeInheritedSectionTitle(
                  const char *id,    const char *ref, 
                  const char *file,  const char *anchor,
                  const char *title, const char *name)
{
  DBG_HTML(t << "<!-- writeInheritedSectionTitle -->" << endl;)
  QCString a = anchor;
  if (!a.isEmpty()) a.prepend("#");
  QCString classLink = QCString("<a class=\"el\" href=\"");
  if (ref)
  {
    classLink+= externalLinkTarget() + externalRef(relPath,ref,TRUE);
  }
  else
  {
    classLink+=relPath;
  }
  classLink+=file+Doxygen::htmlFileExtension+a;
  classLink+=QCString("\">")+convertToHtml(name,FALSE)+"</a>";
  t << "<tr class=\"inherit_header " << id << "\">"
    << "<td colspan=\"2\" onclick=\"javascript:toggleInherit('" << id << "')\">"
    << "<img src=\"" << relPath << "closed.png\" alt=\"-\"/>&#160;" 
    << theTranslator->trInheritedFrom(convertToHtml(title,FALSE),classLink)
    << "</td></tr>" << endl;
}

void HtmlGenerator::writeSummaryLink(const char *file,const char *anchor,const char *title,bool first)
{
  if (first)
  {
    t << "  <div class=\"summary\">\n";
  }
  else
  {
    t << " &#124;\n";
  }
  t << "<a href=\"";
  if (file)
  {
    t << relPath << file;
    t << Doxygen::htmlFileExtension;
  }
  else
  {
    t << "#";
    t << anchor;
  }
  t << "\">";
  t << title;
  t << "</a>";
}

void HtmlGenerator::endMemberDeclaration(const char *anchor,const char *inheritId)
{
  t << "<tr class=\"separator:" << anchor;
  if (inheritId)
  {
    t << " inherit " << inheritId;
  }
  t << "\"><td class=\"memSeparator\" colspan=\"2\">&#160;</td></tr>\n";
}

void HtmlGenerator::setCurrentDoc(Definition *context,const char *anchor,bool isSourceFile)
{
  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->setCurrentDoc(context,anchor,isSourceFile);
  }
}

void HtmlGenerator::addWord(const char *word,bool hiPriority)
{
  if (Doxygen::searchIndex)
  {
    Doxygen::searchIndex->addWord(word,hiPriority);
  }
}

