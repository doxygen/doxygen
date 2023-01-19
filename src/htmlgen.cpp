/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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
#include <assert.h>

#include <mutex>

#include "message.h"
#include "htmlgen.h"
#include "config.h"
#include "util.h"
#include "doxygen.h"
#include "diagram.h"
#include "version.h"
#include "dot.h"
#include "dotcallgraph.h"
#include "dotclassgraph.h"
#include "dotdirdeps.h"
#include "dotgfxhierarchytable.h"
#include "dotgroupcollaboration.h"
#include "dotincldepgraph.h"
#include "language.h"
#include "htmlhelp.h"
#include "docparser.h"
#include "docnode.h"
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
#include "tooltip.h"
#include "growbuf.h"
#include "fileinfo.h"
#include "dir.h"
#include "utf8.h"
#include "textstream.h"
#include "indexlist.h"
#include "datetime.h"
#include "portable.h"

//#define DBG_HTML(x) x;
#define DBG_HTML(x)

static QCString g_header;
static QCString g_footer;
static QCString g_mathjax_code;
static QCString g_latex_macro;
static constexpr auto hex="0123456789ABCDEF";

// note: this is only active if DISABLE_INDEX=YES, if DISABLE_INDEX is disabled, this
// part will be rendered inside menu.js
static void writeClientSearchBox(TextStream &t,const QCString &relPath)
{
  t << "        <div id=\"MSearchBox\" class=\"MSearchBoxInactive\">\n";
  t << "        <span class=\"left\">\n";
  t << "          <span id=\"MSearchSelect\" ";
  t << "               onmouseover=\"return searchBox.OnSearchSelectShow()\" ";
  t << "               onmouseout=\"return searchBox.OnSearchSelectHide()\">&#160;</span>\n";
  t << "          <input type=\"text\" id=\"MSearchField\" value=\"\" placeholder=\""
    << theTranslator->trSearch() << "\" accesskey=\"S\"\n";
  t << "               onfocus=\"searchBox.OnSearchFieldFocus(true)\" \n";
  t << "               onblur=\"searchBox.OnSearchFieldFocus(false)\" \n";
  t << "               onkeyup=\"searchBox.OnSearchFieldChange(event)\"/>\n";
  t << "          </span><span class=\"right\">\n";
  t << "            <a id=\"MSearchClose\" href=\"javascript:searchBox.CloseResultsWindow()\">"
    << "<img id=\"MSearchCloseImg\" border=\"0\" src=\"" << relPath << "search/close.svg\" alt=\"\"/></a>\n";
  t << "          </span>\n";
  t << "        </div>\n";
}

// note: this is only active if DISABLE_INDEX=YES. if DISABLE_INDEX is disabled, this
// part will be rendered inside menu.js
static void writeServerSearchBox(TextStream &t,const QCString &relPath,bool highlightSearch)
{
  bool externalSearch = Config_getBool(EXTERNAL_SEARCH);
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
  t << "              <span id=\"MSearchSelectExt\">&#160;</span>\n";
  if (!highlightSearch)
  {
    t << "              <input type=\"text\" id=\"MSearchField\" name=\"query\" value=\"\" placeholder=\""
      << theTranslator->trSearch() << "\" size=\"20\" accesskey=\"S\" \n";
    t << "                     onfocus=\"searchBox.OnSearchFieldFocus(true)\" \n";
    t << "                     onblur=\"searchBox.OnSearchFieldFocus(false)\"/>\n";
    t << "            </form>\n";
    t << "          </div><div class=\"right\"></div>\n";
    t << "        </div>\n";
  }
}

//------------------------------------------------------------------------
/// Convert a set of LaTeX  commands `\(re)newcommand`  to a form readable by MathJax
/// LaTeX syntax:
/// ```
///       \newcommand{\cmd}{replacement}
///         or
///       \renewcommand{\cmd}{replacement}
/// ```
/// MathJax syntax:
/// ```
///        cmd: "{replacement}"
/// ```
///
/// LaTeX syntax:
/// ```
///       \newcommand{\cmd}[nr]{replacement}
///         or
///       \renewcommand{\cmd}[nr]{replacement}
/// ```
/// MathJax syntax:
/// ```
///        cmd: ["{replacement}",nr]
/// ```
static QCString getConvertLatexMacro()
{
  QCString macrofile = Config_getString(FORMULA_MACROFILE);
  if (macrofile.isEmpty()) return "";
  QCString s = fileToString(macrofile);
  macrofile = FileInfo(macrofile.str()).absFilePath();
  size_t size = s.length();
  GrowBuf out(size);
  const char *data = s.data();
  int line = 1;
  int cnt = 0;
  size_t i = 0;
  QCString nr;
  while (i < size)
  {
    nr = "";
    // skip initial white space, but count lines
    while (i < size && (data[i] == ' ' || data[i] == '\t' || data[i] == '\n'))
    {
      if (data[i] == '\n') line++;
      i++;
    }
    if (i >= size) break;
    // check for \newcommand or \renewcommand
    if (data[i] != '\\')
    {
      warn(macrofile,line, "file contains non valid code, expected '\\' got '%c'\n",data[i]);
      return "";
    }
    i++;
    if (!qstrncmp(data + i, "newcommand", strlen("newcommand")))
    {
      i += strlen("newcommand");
    }
    else if (!qstrncmp(data + i, "renewcommand", strlen("renewcommand")))
    {
      i += strlen("renewcommand");
    }
    else
    {
      warn(macrofile,line, "file contains non valid code, expected 'newcommand' or 'renewcommand'");
      return "";
    }
    // handle {cmd}
    if (data[i] != '{')
    {
      warn(macrofile,line, "file contains non valid code, expected '{' got '%c'\n",data[i]);
      return "";
    }
    i++;
    if (data[i] != '\\')
    {
      warn(macrofile,line, "file contains non valid code, expected '\\' got '%c'\n",data[i]);
      return "";
    }
    i++;
    // run till }, i.e. cmd
    out.addStr("    ");
    while (i < size && (data[i] != '}')) out.addChar(data[i++]);
    if (i >= size)
    {
      warn(macrofile,line, "file contains non valid code, no closing '}' for command");
      return "";
    }
    out.addChar(':');
    out.addChar(' ');
    i++;

    if (data[i] == '[')
    {
      // handle [nr]
      // run till ]
      out.addChar('[');
      i++;
      while (i < size && (data[i] != ']')) nr += data[i++];
      if (i >= size)
      {
        warn(macrofile,line, "file contains non valid code, no closing ']'");
        return "";
      }
      i++;
    }
    else if (data[i] != '{')
    {
      warn(macrofile,line, "file contains non valid code, expected '[' or '{' got '%c'\n",data[i]);
      return "";
    }
    // handle {replacement}
    // retest as the '[' part might have advanced so we can have a new '{'
    if (data[i] != '{')
    {
      warn(macrofile,line, "file contains non valid code, expected '{' got '%c'\n",data[i]);
      return "";
    }
    out.addChar('"');
    out.addChar('{');
    i++;
    // run till }
    cnt = 1;
    while (i < size && cnt)
    {
      switch(data[i])
      {
        case '\\':
          out.addChar('\\'); // need to escape it for MathJax js code
          out.addChar('\\');
          i++;
          if (data[i] == '\\') // we have an escaped backslash
          {
            out.addChar('\\');
            out.addChar('\\');
            i++;
          }
          else if (data[i] != '"') out.addChar(data[i++]); // double quote handled separately
          break;
        case '{':
          cnt++;
          out.addChar(data[i++]);
          break;
        case '}':
          cnt--;
          if (cnt) out.addChar(data[i]);
          i++;
          break;
        case '"':
          out.addChar('\\'); // need to escape it for MathJax js code
          out.addChar(data[i++]);
          break;
        case '\n':
          line++;
          out.addChar(data[i++]);
          break;
        default:
          out.addChar(data[i++]);
          break;
      }
    }
    if (i > size)
    {
      warn(macrofile,line, "file contains non valid code, no closing '}' for replacement");
      return "";
    }
    out.addChar('}');
    out.addChar('"');
    if (!nr.isEmpty())
    {
      out.addChar(',');
      out.addStr(nr);
    }
    if (!nr.isEmpty())
    {
      out.addChar(']');
    }
    out.addChar(',');
    out.addChar('\n');
  }
  out.addChar(0);
  return out.get();
}

static QCString getSearchBox(bool serverSide, QCString relPath, bool highlightSearch)
{
  TextStream t;
  if (serverSide)
  {
    writeServerSearchBox(t, relPath, highlightSearch);
  }
  else
  {
    writeClientSearchBox(t, relPath);
  }
  return t.str();
}

static QCString substituteHtmlKeywords(const QCString &str,
                                       const QCString &title,
                                       const QCString &relPath,
                                       const QCString &navPath=QCString())
{
  // Build CSS/JavaScript tags depending on treeview, search engine settings
  QCString cssFile;
  QCString generatedBy;
  QCString treeViewCssJs;
  QCString searchCssJs;
  QCString searchBox;
  QCString mathJaxJs;
  QCString extraCssText;

  QCString projectName = Config_getString(PROJECT_NAME);
  bool timeStamp = Config_getBool(HTML_TIMESTAMP);
  bool treeView = Config_getBool(GENERATE_TREEVIEW);
  bool searchEngine = Config_getBool(SEARCHENGINE);
  bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
  bool mathJax = Config_getBool(USE_MATHJAX);
  QCString mathJaxFormat = Config_getEnumAsString(MATHJAX_FORMAT);
  bool disableIndex = Config_getBool(DISABLE_INDEX);
  bool hasProjectName = !projectName.isEmpty();
  bool hasProjectNumber = !Config_getString(PROJECT_NUMBER).isEmpty();
  bool hasProjectBrief = !Config_getString(PROJECT_BRIEF).isEmpty();
  bool hasProjectLogo = !Config_getString(PROJECT_LOGO).isEmpty();
  bool hasFullSideBar = Config_getBool(FULL_SIDEBAR) && disableIndex && treeView;
  static bool titleArea = (hasProjectName || hasProjectBrief || hasProjectLogo || (disableIndex && searchEngine));

  cssFile = Config_getString(HTML_STYLESHEET);
  if (cssFile.isEmpty())
  {
    cssFile = "doxygen.css";
  }
  else
  {
    if (!cssFile.startsWith("http:") && !cssFile.startsWith("https:"))
    {
      FileInfo cssfi(cssFile.str());
      if (cssfi.exists())
      {
        cssFile = cssfi.fileName();
      }
      else
      {
        cssFile = "doxygen.css";
      }
    }
  }

  extraCssText = "";
  const StringVector &extraCssFile = Config_getList(HTML_EXTRA_STYLESHEET);
  for (const auto &fileName : extraCssFile)
  {
    if (!fileName.empty())
    {
      QCString htmlStyleSheet = fileName.c_str();
      if (htmlStyleSheet.startsWith("http:") || htmlStyleSheet.startsWith("https:"))
      {
        extraCssText += "<link href=\""+htmlStyleSheet+"\" rel=\"stylesheet\" type=\"text/css\"/>\n";
      }
      else
      {
        FileInfo fi(fileName);
        if (fi.exists())
        {
          extraCssText += "<link href=\"$relpath^"+stripPath(fileName.c_str())+"\" rel=\"stylesheet\" type=\"text/css\"/>\n";
        }
      }
    }
  }

  if (timeStamp)
  {
    generatedBy = theTranslator->trGeneratedAt(dateToString(DateTimeType::DateTime),
                                convertToHtml(Config_getString(PROJECT_NAME)));
  }
  else
  {
    generatedBy = theTranslator->trGeneratedBy();
  }

  if (treeView)
  {
    treeViewCssJs = "<link href=\"$relpath^navtree.css\" rel=\"stylesheet\" type=\"text/css\"/>\n"
    //                    "<script type=\"text/javascript\">var page_layout=";
    //treeViewCssJs += Config_getBool(DISABLE_INDEX) ? "1" : "0";
    //treeViewCssJs += ";</script>\n"
			"<script type=\"text/javascript\" src=\"$relpath^resize.js\"></script>\n"
			"<script type=\"text/javascript\" src=\"$relpath^navtreedata.js\"></script>\n"
			"<script type=\"text/javascript\" src=\"$relpath^navtree.js\"></script>\n";
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
      if (disableIndex || !Config_getBool(HTML_DYNAMIC_MENUS))
      {
        searchCssJs += "<script type=\"text/javascript\">\n"
					"/* @license magnet:?xt=urn:btih:d3d9a9a6595521f9666a5e94cc830dab83b65699&amp;dn=expat.txt MIT */\n"
				"  $(document).ready(function() { init_search(); });\n"
					"/* @license-end */\n"
					"</script>";
      }
    }
    else
    {
      if (disableIndex || !Config_getBool(HTML_DYNAMIC_MENUS))
      {
        searchCssJs += "<script type=\"text/javascript\">\n"
					"/* @license magnet:?xt=urn:btih:d3d9a9a6595521f9666a5e94cc830dab83b65699&amp;dn=expat.txt MIT */\n"
					"  $(document).ready(function() {\n"
					"    if ($('.searchresults').length > 0) { searchBox.DOMSearchField().focus(); }\n"
					"  });\n"
					"  /* @license-end */\n"
					"</script>\n";
      }

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
    auto mathJaxVersion = Config_getEnum(MATHJAX_VERSION);
    QCString path = Config_getString(MATHJAX_RELPATH);
    if (path.isEmpty() || path.startsWith("..")) // relative path
    {
      path.prepend(relPath);
    }

    switch (mathJaxVersion)
    {
      case MATHJAX_VERSION_t::MathJax_3:
        {
          mathJaxJs += "<script src=\"https://polyfill.io/v3/polyfill.min.js?features=es6\"></script>\n"
                       "<script type=\"text/javascript\">\n"
                       "window.MathJax = {\n"
                       "  options: {\n"
                       "    ignoreHtmlClass: 'tex2jax_ignore',\n"
                       "    processHtmlClass: 'tex2jax_process'\n"
                       "  }";
         const StringVector &mathJaxExtensions = Config_getList(MATHJAX_EXTENSIONS);
         if (!mathJaxExtensions.empty() || !g_latex_macro.isEmpty())
         {
           mathJaxJs+= ",\n";
           if (!mathJaxExtensions.empty())
           {
             bool first = true;
             mathJaxJs+= "  loader: {\n"
                         "    load: [";
             for (const auto &s : mathJaxExtensions)
             {
               if (!first) mathJaxJs+= ",";
               mathJaxJs+= "'[tex]/"+QCString(s.c_str())+"'";
               first = false;
             }
             mathJaxJs+= "]\n"
                         "  },\n";
           }
           mathJaxJs+= "  tex: {\n"
                       "    macros: {";
           if (!g_latex_macro.isEmpty())
           {
             mathJaxJs += g_latex_macro+"    ";
           }
           mathJaxJs+="},\n"
                       "    packages: ['base','configmacros'";
           if (!g_latex_macro.isEmpty())
           {
             mathJaxJs+= ",'newcommand'";
           }
           for (const auto &s : mathJaxExtensions)
           {
             mathJaxJs+= ",'"+QCString(s.c_str())+"'";
           }
           mathJaxJs += "]\n"
                         "  }\n";
         }
         else
         {
           mathJaxJs += "\n";
         }
         mathJaxJs += "};\n";
         // MATHJAX_CODEFILE
         if (!g_mathjax_code.isEmpty())
         {
           mathJaxJs += g_mathjax_code;
           mathJaxJs += "\n";
         }
         mathJaxJs += "</script>\n";
         mathJaxJs += "<script type=\"text/javascript\" id=\"MathJax-script\" async=\"async\" src=\"" +
                      path + "es5/tex-" + mathJaxFormat.lower() + ".js\">";
         mathJaxJs+="</script>\n";
        }
        break;
      case MATHJAX_VERSION_t::MathJax_2:
        {
          mathJaxJs = "<script type=\"text/x-mathjax-config\">\n"
                      "MathJax.Hub.Config({\n"
                      "  extensions: [\"tex2jax.js\"";
          const StringVector &mathJaxExtensions = Config_getList(MATHJAX_EXTENSIONS);
          for (const auto &s : mathJaxExtensions)
          {
            mathJaxJs+= ", \""+QCString(s.c_str())+".js\"";
          }
          if (mathJaxFormat.isEmpty())
          {
            mathJaxFormat = "HTML-CSS";
          }
          mathJaxJs += "],\n"
                       "  jax: [\"input/TeX\",\"output/"+mathJaxFormat+"\"],\n";
          if (!g_latex_macro.isEmpty())
          {
            mathJaxJs += "   TeX: { Macros: {\n";
            mathJaxJs += g_latex_macro;
            mathJaxJs += "\n"
                         "  } }\n";
          }
          mathJaxJs +=   "});\n";
          if (!g_mathjax_code.isEmpty())
          {
            mathJaxJs += g_mathjax_code;
            mathJaxJs += "\n";
          }
          mathJaxJs += "</script>\n";
          mathJaxJs += "<script type=\"text/javascript\" async=\"async\" src=\"" + path + "MathJax.js\"></script>\n";
        }
        break;
    }
  }

  QCString darkModeJs;
  if (Config_getEnum(HTML_COLORSTYLE)==HTML_COLORSTYLE_t::TOGGLE)
  {
    darkModeJs="<script type=\"text/javascript\" src=\"$relpath^darkmode_toggle.js\"></script>\n";
  }

  // first substitute generic keywords
  QCString result = substituteKeywords(str,title,
        convertToHtml(Config_getString(PROJECT_NAME)),
        convertToHtml(Config_getString(PROJECT_NUMBER)),
        convertToHtml(Config_getString(PROJECT_BRIEF)));

  // then do the HTML specific keywords
  result = substituteKeywords(result,
  {
    // keyword           value getter
    { "$navpath",        [&]() { return navPath;        } },
    { "$stylesheet",     [&]() { return cssFile;        } },
    { "$treeview",       [&]() { return treeViewCssJs;  } },
    { "$searchbox",      [&]() { return searchBox;      } },
    { "$search",         [&]() { return searchCssJs;    } },
    { "$mathjax",        [&]() { return mathJaxJs;      } },
    { "$darkmode",       [&]() { return darkModeJs;     } },
    { "$generatedby",    [&]() { return generatedBy;    } },
    { "$extrastylesheet",[&]() { return extraCssText;   } },
    { "$relpath$",       [&]() { return relPath;        } } //<-- obsolete: for backwards compatibility only
  });

  result = substitute(result,"$relpath^",relPath); //<-- must be done after the previous substitutions

  // additional HTML only conditional blocks
  result = selectBlock(result,"FULL_SIDEBAR",hasFullSideBar,OutputType::Html);
  result = selectBlock(result,"DISABLE_INDEX",disableIndex,OutputType::Html);
  result = selectBlock(result,"GENERATE_TREEVIEW",treeView,OutputType::Html);
  result = selectBlock(result,"SEARCHENGINE",searchEngine,OutputType::Html);
  result = selectBlock(result,"TITLEAREA",titleArea,OutputType::Html);
  result = selectBlock(result,"PROJECT_NAME",hasProjectName,OutputType::Html);
  result = selectBlock(result,"PROJECT_NUMBER",hasProjectNumber,OutputType::Html);
  result = selectBlock(result,"PROJECT_BRIEF",hasProjectBrief,OutputType::Html);
  result = selectBlock(result,"PROJECT_LOGO",hasProjectLogo,OutputType::Html);

  result = removeEmptyLines(result);

  return result;
}

//---------------------------------------------------------------------------------------------

static StringUnorderedMap g_lightMap;
static StringUnorderedMap g_darkMap;

static void fillColorStyleMap(const QCString &definitions,StringUnorderedMap &map)
{
  int p=0,i=0;
  while ((i=definitions.find('\n',p))!=-1)
  {
    QCString line = definitions.mid(p,i-p);
    if (line.startsWith("--"))
    {
      int separator = line.find(':');
      assert(separator!=-1);
      std::string key = line.left(separator).str();
      int semi = line.find(';');
      assert(semi!=-1);
      std::string value = line.mid(separator+1,semi-separator-1).stripWhiteSpace().str();
      map.insert(std::make_pair(key,value));
      //printf("var(%s)=%s\n",qPrint(key),qPrint(value));
    }
    p=i+1;
  }
}

static void fillColorStyleMaps()
{
  ResourceMgr &mgr = ResourceMgr::instance();
  auto colorStyle = Config_getEnum(HTML_COLORSTYLE);
  if (colorStyle==HTML_COLORSTYLE_t::LIGHT)
  {
    fillColorStyleMap(replaceColorMarkers(mgr.getAsString("lightmode_settings.css")),g_lightMap);
  }
  else if (colorStyle==HTML_COLORSTYLE_t::DARK)
  {
    fillColorStyleMap(replaceColorMarkers(mgr.getAsString("darkmode_settings.css")),g_darkMap);
  }
}

static QCString replaceVariables(const QCString &input)
{
  auto doReplacements = [&input](const StringUnorderedMap &mapping) -> QCString
  {
    GrowBuf output;
    int p=0,i=0;
    while ((i=input.find("var(",p))!=-1)
    {
      output.addStr(input.data()+p,i-p);
      int j=input.find(")",i+4);
      assert(j!=-1);
      auto it = mapping.find(input.mid(i+4,j-i-4).str()); // find variable
      assert(it!=mapping.end());                          // should be found
      output.addStr(it->second);                          // add it value
      //printf("replace '%s' by '%s'\n",qPrint(input.mid(i+4,j-i-4)),qPrint(it->second));
      p=j+1;
    }
    output.addStr(input.data()+p,input.length()-p);
    output.addChar(0);
    return output.get();
  };

  auto colorStyle = Config_getEnum(HTML_COLORSTYLE);
  if (colorStyle==HTML_COLORSTYLE_t::LIGHT)
  {
    return doReplacements(g_lightMap);
  }
  else if (colorStyle==HTML_COLORSTYLE_t::DARK)
  {
    return doReplacements(g_darkMap);
  }
  else
  {
    return input;
  }
}

//----------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------

HtmlCodeGenerator::HtmlCodeGenerator(TextStream &t) : m_t(t)
{
}

HtmlCodeGenerator::HtmlCodeGenerator(TextStream &t,const QCString &relPath)
  : m_t(t), m_relPath(relPath)
{
}

void HtmlCodeGenerator::setRelativePath(const QCString &path)
{
  m_relPath = path;
}

void HtmlCodeGenerator::codify(const QCString &str)
{
  int tabSize = Config_getInt(TAB_SIZE);
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
		   else if (*p=='(')
                     { m_t << "\\&zwj;("; m_col++;p++; }
                   else if (*p==')')
                     { m_t << "\\&zwj;)"; m_col++;p++; }
                   else
                     m_t << "\\";
                   m_col++;
                   break;
        default:
          {
            uint8_t uc = static_cast<uint8_t>(c);
            if (uc<32)
            {
              m_t << "&#x24" << hex[uc>>4] << hex[uc&0xF] << ";";
              m_col++;
            }
            else
            {
              p=writeUTF8Char(m_t,p-1);
              m_col++;
            }
          }
          break;
      }
    }
  }
}

void HtmlCodeGenerator::docify(const QCString &str)
{
  if (!str.isEmpty())
  {
    const char *p=str.data();
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
	  else if (*p=='(')
            { m_t << "\\&zwj;("; p++; }
          else if (*p==')')
            { m_t << "\\&zwj;)"; p++; }
          else
            m_t << "\\";
          break;
        default:
          {
            uint8_t uc = static_cast<uint8_t>(c);
            if (uc<32 && !isspace(c))
            {
              m_t << "&#x24" << hex[uc>>4] << hex[uc&0xF] << ";";
            }
            else
            {
              m_t << c;
            }
          }
          break;
      }
    }
  }
}


void HtmlCodeGenerator::writeLineNumber(const QCString &ref,const QCString &filename,
                                    const QCString &anchor,int l,bool writeLineAnchor)
{
  const int maxLineNrStr = 10;
  char lineNumber[maxLineNrStr];
  char lineAnchor[maxLineNrStr];
  qsnprintf(lineNumber,maxLineNrStr,"%5d",l);
  qsnprintf(lineAnchor,maxLineNrStr,"l%05d",l);

  if (!m_lineOpen)
  {
    m_t << "<div class=\"line\">";
    m_lineOpen = TRUE;
  }

  if (writeLineAnchor) m_t << "<a id=\"" << lineAnchor << "\" name=\"" << lineAnchor << "\"></a>";
  m_t << "<span class=\"lineno\">";
  if (!filename.isEmpty())
  {
    _writeCodeLink("line",ref,filename,anchor,lineNumber,QCString());
  }
  else
  {
    codify(lineNumber);
  }
  m_t << "</span>";
  m_col=0;
}

void HtmlCodeGenerator::writeCodeLink(CodeSymbolType type,
                                      const QCString &ref,const QCString &f,
                                      const QCString &anchor, const QCString &name,
                                      const QCString &tooltip)
{
  const char *hl = codeSymbolType2Str(type);
  QCString hlClass = "code";
  if (hl)
  {
    hlClass+=" hl_";
    hlClass+=hl;
  }
  _writeCodeLink(hlClass,ref,f,anchor,name,tooltip);
}

void HtmlCodeGenerator::_writeCodeLink(const QCString &className,
                                      const QCString &ref,const QCString &f,
                                      const QCString &anchor, const QCString &name,
                                      const QCString &tooltip)
{
  if (!ref.isEmpty())
  {
    m_t << "<a class=\"" << className << "Ref\" ";
    m_t << externalLinkTarget();
  }
  else
  {
    m_t << "<a class=\"" << className << "\" ";
  }
  m_t << "href=\"";
  m_t << externalRef(m_relPath,ref,TRUE);
  if (!f.isEmpty()) m_t << addHtmlExtensionIfMissing(f);
  if (!anchor.isEmpty()) m_t << "#" << anchor;
  m_t << "\"";
  if (!tooltip.isEmpty()) m_t << " title=\"" << convertToHtml(tooltip) << "\"";
  m_t << ">";
  docify(name);
  m_t << "</a>";
  m_col+=name.length();
}

void HtmlCodeGenerator::writeTooltip(const QCString &id, const DocLinkInfo &docInfo,
                                     const QCString &decl, const QCString &desc,
                                     const SourceLinkInfo &defInfo,
                                     const SourceLinkInfo &declInfo)
{
  m_t << "<div class=\"ttc\" id=\"" << id << "\">";
  m_t << "<div class=\"ttname\">";
  if (!docInfo.url.isEmpty())
  {
    m_t << "<a href=\"";
    m_t << externalRef(m_relPath,docInfo.ref,TRUE);
    m_t << addHtmlExtensionIfMissing(docInfo.url);
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
  if (!decl.isEmpty())
  {
    m_t << "<div class=\"ttdeci\">";
    docify(decl);
    m_t << "</div>";
  }
  if (!desc.isEmpty())
  {
    m_t << "<div class=\"ttdoc\">";
    docify(desc);
    m_t << "</div>";
  }
  if (!defInfo.file.isEmpty())
  {
    m_t << "<div class=\"ttdef\"><b>Definition:</b> ";
    if (!defInfo.url.isEmpty())
    {
      m_t << "<a href=\"";
      m_t << externalRef(m_relPath,defInfo.ref,TRUE);
      m_t << addHtmlExtensionIfMissing(defInfo.url);
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
      m_t << addHtmlExtensionIfMissing(declInfo.url);
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
  m_t << "</div>\n";
}


void HtmlCodeGenerator::startCodeLine(bool)
{
  m_col=0;
  if (!m_lineOpen)
  {
    m_t << "<div class=\"line\">";
    m_lineOpen = TRUE;
  }
}

void HtmlCodeGenerator::endCodeLine()
{
  if (m_col == 0)
  {
    m_t << " ";
    m_col++;
  }
  if (m_lineOpen)
  {
    m_t << "</div>\n";
    m_lineOpen = FALSE;
  }
}

void HtmlCodeGenerator::startFontClass(const QCString &s)
{
  m_t << "<span class=\"" << s << "\">";
}

void HtmlCodeGenerator::endFontClass()
{
  m_t << "</span>";
}

void HtmlCodeGenerator::writeCodeAnchor(const QCString &anchor)
{
  m_t << "<a id=\"" << anchor << "\" name=\"" << anchor << "\"></a>";
}

void HtmlCodeGenerator::startCodeFragment(const QCString &)
{
  m_t << "<div class=\"fragment\">";
}

void HtmlCodeGenerator::endCodeFragment(const QCString &)
{
  //endCodeLine checks is there is still an open code line, if so closes it.
  endCodeLine();

  m_t << "</div><!-- fragment -->";
}


//--------------------------------------------------------------------------

HtmlGenerator::HtmlGenerator()
  : OutputGenerator(Config_getString(HTML_OUTPUT))
  , m_codeGen(m_t)
{
}

HtmlGenerator::HtmlGenerator(const HtmlGenerator &og)
  : OutputGenerator(og)
  , m_codeGen(m_t)
{
}

HtmlGenerator &HtmlGenerator::operator=(const HtmlGenerator &og)
{
  OutputGenerator::operator=(og);
  return *this;
}

std::unique_ptr<OutputGenerator> HtmlGenerator::clone() const
{
  return std::make_unique<HtmlGenerator>(*this);
}

HtmlGenerator::~HtmlGenerator()
{
  //printf("HtmlGenerator::~HtmlGenerator()\n");
}

void HtmlGenerator::init()
{
  QCString dname = Config_getString(HTML_OUTPUT);
  Dir d(dname.str());
  if (!d.exists() && !d.mkdir(dname.str()))
  {
    term("Could not create output directory %s\n",qPrint(dname));
  }
  //writeLogo(dname);
  if (!Config_getString(HTML_HEADER).isEmpty())
  {
    g_header=fileToString(Config_getString(HTML_HEADER));
    //printf("g_header='%s'\n",qPrint(g_header));
  }
  else
  {
    g_header = ResourceMgr::instance().getAsString("header.html");
  }

  if (!Config_getString(HTML_FOOTER).isEmpty())
  {
    g_footer=fileToString(Config_getString(HTML_FOOTER));
    //printf("g_footer='%s'\n",qPrint(g_footer));
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
      //printf("g_mathjax_code='%s'\n",qPrint(g_mathjax_code));
    }
    g_latex_macro=getConvertLatexMacro();
    //printf("converted g_latex_macro='%s'\n",qPrint(g_latex_macro));
  }
  createSubDirs(d);

  fillColorStyleMaps();

  ResourceMgr &mgr = ResourceMgr::instance();

  {
    QCString tabsCss;
    if (Config_getBool(HTML_DYNAMIC_MENUS))
    {
      tabsCss = mgr.getAsString("tabs.css");
    }
    else // stylesheet for the 'old' static tabs
    {
      tabsCss = mgr.getAsString("fixed_tabs.css");
    }

    std::ofstream f = Portable::openOutputStream(dname+"/tabs.css");
    if (f.is_open())
    {
      TextStream t(&f);
      t << replaceVariables(tabsCss);
    }
  }


  mgr.copyResource("jquery.js",dname);
  if (Config_getBool(INTERACTIVE_SVG))
  {
    mgr.copyResource("svgpan.js",dname);
  }

  if (!Config_getBool(DISABLE_INDEX) && Config_getBool(HTML_DYNAMIC_MENUS))
  {
    mgr.copyResource("menu.js",dname);
  }

  if (Config_getBool(HTML_COLORSTYLE)==HTML_COLORSTYLE_t::TOGGLE)
  {
    //mgr.copyResource("darkmode_toggle.js",dname);
    std::ofstream f = Portable::openOutputStream(dname+"/darkmode_toggle.js");
    if (f.is_open())
    {
      TextStream t(&f);
      t << replaceColorMarkers(mgr.getAsString("darkmode_toggle.js"));
    }
  }

  {
    std::ofstream f = Portable::openOutputStream(dname+"/dynsections.js");
    if (f.is_open())
    {
      TextStream t(&f);
      t << mgr.getAsString("dynsections.js");
      if (Config_getBool(SOURCE_BROWSER) && Config_getBool(SOURCE_TOOLTIPS))
      {
        t << mgr.getAsString("dynsections_tooltips.js");
      }
    }
  }
}

void HtmlGenerator::cleanup()
{
  QCString dname = Config_getString(HTML_OUTPUT);
  Dir d(dname.str());
  clearSubDirs(d);
}

/// Additional initialization after indices have been created
void HtmlGenerator::writeTabData()
{
  Doxygen::indexList->addStyleSheetFile("tabs.css");
  QCString dname=Config_getString(HTML_OUTPUT);
  ResourceMgr &mgr = ResourceMgr::instance();
  //writeColoredImgData(dname,colored_tab_data);
  mgr.copyResource("tab_a.lum",dname);  // active, light mode
  mgr.copyResource("tab_b.lum",dname);  // normal, light mode
  mgr.copyResource("tab_h.lum",dname);  // highlight, light mode
  mgr.copyResource("tab_s.lum",dname);  // separator, light mode
  mgr.copyResource("tab_ad.lum",dname); // active, dark mode
  mgr.copyResource("tab_bd.lum",dname); // normal, dark mode
  mgr.copyResource("tab_hd.lum",dname); // highlight, dark mode
  mgr.copyResource("tab_sd.lum",dname); // separator, light mode
  mgr.copyResource("nav_h.lum",dname);  // header gradient, light mode
  mgr.copyResource("nav_hd.lum",dname); // header gradient, dark mode
  mgr.copyResource("nav_f.lum",dname); // member definition header, light mode
  mgr.copyResource("nav_fd.lum",dname); // member definition header, dark mode
  mgr.copyResource("bc_s.luma",dname); // breadcrumb separator, light mode
  mgr.copyResource("bc_sd.luma",dname); // breadcrumb separator, dark mode
  mgr.copyResource("doxygen.svg",dname);
  Doxygen::indexList->addImageFile("doxygen.svg");
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
  Doxygen::indexList->addImageFile("nav_g.png");
}

void HtmlGenerator::writeSearchData(const QCString &dname)
{
  //bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
  //writeImgData(dname,serverBasedSearch ? search_server_data : search_client_data);
  ResourceMgr &mgr = ResourceMgr::instance();

  // server side search resources
  mgr.copyResource("mag.svg",dname);
  mgr.copyResource("mag_d.svg",dname);
  Doxygen::indexList->addImageFile("search/mag.svg");
  Doxygen::indexList->addImageFile("search/mag_d.svg");

  // client side search resources
  mgr.copyResource("close.svg",dname);
  Doxygen::indexList->addImageFile("search/close.svg");
  mgr.copyResource("mag_sel.svg",dname);
  mgr.copyResource("mag_seld.svg",dname);
  Doxygen::indexList->addImageFile("search/mag_sel.svg");
  Doxygen::indexList->addImageFile("search/mag_seld.svg");

  QCString searchDirName = dname;
  std::ofstream f = Portable::openOutputStream(searchDirName+"/search.css");
  if (f.is_open())
  {
    TextStream t(&f);
    QCString searchCss;
    // the position of the search box depends on a number of settings.
    // Insert the right piece of CSS code depending on which options are selected
    if (Config_getBool(DISABLE_INDEX))
    {
      if (Config_getBool(GENERATE_TREEVIEW) && Config_getBool(FULL_SIDEBAR))
      {
        searchCss = mgr.getAsString("search_sidebar.css"); // we have a full height side bar
      }
      else if (Config_getBool(HTML_COLORSTYLE)==HTML_COLORSTYLE_t::TOGGLE)
      {
        searchCss = mgr.getAsString("search_nomenu_toggle.css"); // we have no tabs but do have a darkmode button
      }
      else
      {
        searchCss = mgr.getAsString("search_nomenu.css"); // we have no tabs and no darkmode button
      }
    }
    else if (!Config_getBool(HTML_DYNAMIC_MENUS))
    {
      searchCss = mgr.getAsString("search_fixedtabs.css"); // we have tabs, but they are static
    }
    else
    {
      searchCss = mgr.getAsString("search.css"); // default case with a dynamic menu bar
    }
    // and then add the option independent part of the styling
    searchCss += mgr.getAsString("search_common.css");
    searchCss = substitute(searchCss,"$doxygenversion",getDoxygenVersion());
    t << replaceVariables(searchCss);
    Doxygen::indexList->addStyleSheetFile("search/search.css");
  }
}

static void writeDefaultStyleSheet(TextStream &t)
{
  t << "/* The standard CSS for doxygen " << getDoxygenVersion() << "*/\n\n";
  switch (Config_getEnum(HTML_COLORSTYLE))
  {
    case HTML_COLORSTYLE_t::LIGHT:
    case HTML_COLORSTYLE_t::DARK:
      /* variables will be resolved while writing to the CSS file */
      break;
    case HTML_COLORSTYLE_t::AUTO_LIGHT:
    case HTML_COLORSTYLE_t::TOGGLE:
      t << "html {\n";
      t << replaceColorMarkers(ResourceMgr::instance().getAsString("lightmode_settings.css"));
      t << "}\n\n";
      break;
    case HTML_COLORSTYLE_t::AUTO_DARK:
      t << "html {\n";
      t << replaceColorMarkers(ResourceMgr::instance().getAsString("darkmode_settings.css"));
      t << "}\n\n";
      break;
  }
  if (Config_getEnum(HTML_COLORSTYLE)==HTML_COLORSTYLE_t::AUTO_LIGHT)
  {
    t << "@media (prefers-color-scheme: dark) {\n";
    t << "  html:not(.dark-mode) {\n";
    t << "    color-scheme: dark;\n\n";
    t << replaceColorMarkers(ResourceMgr::instance().getAsString("darkmode_settings.css"));
    t << "}}\n";
  }
  else if (Config_getEnum(HTML_COLORSTYLE)==HTML_COLORSTYLE_t::AUTO_DARK)
  {
    t << "@media (prefers-color-scheme: light) {\n";
    t << "  html:not(.light-mode) {\n";
    t << "    color-scheme: light;\n\n";
    t << replaceColorMarkers(ResourceMgr::instance().getAsString("lightmode_settings.css"));
    t << "}}\n";
  }
  else if (Config_getEnum(HTML_COLORSTYLE)==HTML_COLORSTYLE_t::TOGGLE)
  {
    t << "html.dark-mode {\n";
    t << replaceColorMarkers(ResourceMgr::instance().getAsString("darkmode_settings.css"));
    t << "}\n\n";
  }

  t << replaceVariables(ResourceMgr::instance().getAsString("doxygen.css"));

  // For Webkit based the scrollbar styling cannot be overruled (bug in chromium?).
  // To allow the user to style the scrollbars differently we should only add it in case
  // the user did not specify any extra stylesheets.
  bool addScrollbarStyling = Config_getList(HTML_EXTRA_STYLESHEET).empty();
  if (addScrollbarStyling)
  {
    t << replaceVariables(ResourceMgr::instance().getAsString("scrollbar.css"));
  }

}

void HtmlGenerator::writeStyleSheetFile(TextStream &t)
{
  fillColorStyleMaps();
  writeDefaultStyleSheet(t);
}

void HtmlGenerator::writeHeaderFile(TextStream &t, const QCString & /*cssname*/)
{
  t << "<!-- HTML header for doxygen " << getDoxygenVersion() << "-->\n";
  t << ResourceMgr::instance().getAsString("header.html");
}

void HtmlGenerator::writeFooterFile(TextStream &t)
{
  t << "<!-- HTML footer for doxygen " << getDoxygenVersion() << "-->\n";
  t << ResourceMgr::instance().getAsString("footer.html");
}

static std::mutex g_indexLock;

void HtmlGenerator::startFile(const QCString &name,const QCString &,
                              const QCString &title,int id)
{
  //printf("HtmlGenerator::startFile(%s)\n",qPrint(name));
  m_relPath = relativePathToRoot(name);
  QCString fileName = addHtmlExtensionIfMissing(name);
  m_lastTitle=title;

  startPlainFile(fileName);
  m_codeGen.setRelativePath(m_relPath);
  {
    std::lock_guard<std::mutex> lock(g_indexLock);
    Doxygen::indexList->addIndexFile(fileName);
  }

  m_lastFile = fileName;
  m_t << substituteHtmlKeywords(g_header,convertToHtml(filterTitle(title)),m_relPath);

  m_t << "<!-- " << theTranslator->trGeneratedBy() << " Doxygen "
      << getDoxygenVersion() << " -->\n";
  bool searchEngine = Config_getBool(SEARCHENGINE);
  if (searchEngine /*&& !generateTreeView*/)
  {
    m_t << "<script type=\"text/javascript\">\n";
    m_t << "/* @license magnet:?xt=urn:btih:d3d9a9a6595521f9666a5e94cc830dab83b65699&amp;dn=expat.txt MIT */\n";
    m_t << "var searchBox = new SearchBox(\"searchBox\", \""
        << m_relPath<< "search/\",'" << Doxygen::htmlFileExtension << "');\n";
    m_t << "/* @license-end */\n";
    m_t << "</script>\n";
  }
  m_sectionCount=0;
}

void HtmlGenerator::writeSearchInfo(TextStream &t,const QCString &)
{
  bool searchEngine      = Config_getBool(SEARCHENGINE);
  bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
  if (searchEngine && !serverBasedSearch)
  {
    t << "<!-- window showing the filter options -->\n";
    t << "<div id=\"MSearchSelectWindow\"\n";
    t << "     onmouseover=\"return searchBox.OnSearchSelectShow()\"\n";
    t << "     onmouseout=\"return searchBox.OnSearchSelectHide()\"\n";
    t << "     onkeydown=\"return searchBox.OnSearchSelectKey(event)\">\n";
    t << "</div>\n";
    t << "\n";
    t << "<!-- iframe showing the search results (closed by default) -->\n";
    t << "<div id=\"MSearchResultsWindow\">\n";
    t << "<div id=\"MSearchResults\">\n";
    t << "<div class=\"SRPage\">\n";
    t << "<div id=\"SRIndex\">\n";
    t << "<div id=\"SRResults\"></div>\n"; // here the results will be inserted
    t << "<div class=\"SRStatus\" id=\"Loading\">"   << theTranslator->trLoading()   << "</div>\n";
    t << "<div class=\"SRStatus\" id=\"Searching\">" << theTranslator->trSearching() << "</div>\n";
    t << "<div class=\"SRStatus\" id=\"NoMatches\">" << theTranslator->trNoMatches() << "</div>\n";
    t << "</div>\n"; // SRIndex
    t << "</div>\n"; // SRPage
    t << "</div>\n"; // MSearchResults
    t << "</div>\n"; // MSearchResultsWindow
    t << "\n";
  }
}

void HtmlGenerator::writeSearchInfo()
{
  writeSearchInfo(m_t,m_relPath);
}


QCString HtmlGenerator::writeLogoAsString(const QCString &path)
{
  bool timeStamp = Config_getBool(HTML_TIMESTAMP);
  QCString result;
  if (timeStamp)
  {
    result += theTranslator->trGeneratedAt(
               dateToString(DateTimeType::DateTime),
               Config_getString(PROJECT_NAME)
              );
  }
  else
  {
    result += theTranslator->trGeneratedBy();
  }
  result += "&#160;\n<a href=\"https://www.doxygen.org/index.html\">\n"
            "<img class=\"footer\" src=\"";
  result += path;
  result += "doxygen.svg\" width=\"104\" height=\"31\" alt=\"doxygen\"/></a> ";
  result += getDoxygenVersion();
  result += " ";
  return result;
}

void HtmlGenerator::writeLogo()
{
  m_t << writeLogoAsString(m_relPath);
}

void HtmlGenerator::writePageFooter(TextStream &t,const QCString &lastTitle,
                              const QCString &relPath,const QCString &navPath)
{
  t << substituteHtmlKeywords(g_footer,convertToHtml(lastTitle),relPath,navPath);
}

void HtmlGenerator::writeFooter(const QCString &navPath)
{
  writePageFooter(m_t,m_lastTitle,m_relPath,navPath);
}

void HtmlGenerator::endFile()
{
  endPlainFile();
}

void HtmlGenerator::startProjectNumber()
{
  m_t << "<h3 class=\"version\">";
}

void HtmlGenerator::endProjectNumber()
{
  m_t << "</h3>";
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
      writeDefaultStyleSheet(m_t);
      endPlainFile();
      Doxygen::indexList->addStyleSheetFile("doxygen.css");
    }
    else // write user defined style sheet
    {
      QCString cssName=Config_getString(HTML_STYLESHEET);
      if (!cssName.startsWith("http:") && !cssName.startsWith("https:"))
      {
        FileInfo cssfi(cssName.str());
        if (!cssfi.exists() || !cssfi.isFile() || !cssfi.isReadable())
        {
          err("style sheet %s does not exist or is not readable!", qPrint(Config_getString(HTML_STYLESHEET)));
        }
        else
        {
          // convert style sheet to string
          QCString fileStr = fileToString(cssName);
          // write the string into the output dir
          startPlainFile(cssfi.fileName().c_str());
          m_t << fileStr;
          endPlainFile();
        }
        Doxygen::indexList->addStyleSheetFile(cssfi.fileName().c_str());
      }
    }
    const StringVector &extraCssFiles = Config_getList(HTML_EXTRA_STYLESHEET);
    for (const auto &fileName : extraCssFiles)
    {
      if (!fileName.empty())
      {
        FileInfo fi(fileName);
        if (fi.exists())
        {
          Doxygen::indexList->addStyleSheetFile(fi.fileName().c_str());
        }
      }
    }

    Doxygen::indexList->addStyleSheetFile("jquery.js");

    Doxygen::indexList->addStyleSheetFile("dynsections.js");

    if (Config_getEnum(HTML_COLORSTYLE)==HTML_COLORSTYLE_t::TOGGLE)
    {
      Doxygen::indexList->addStyleSheetFile("darkmode_toggle.js");
    }

    if (Config_getBool(INTERACTIVE_SVG))
    {
      Doxygen::indexList->addStyleSheetFile("svgpan.js");
    }

    if (!Config_getBool(DISABLE_INDEX) && Config_getBool(HTML_DYNAMIC_MENUS))
    {
      Doxygen::indexList->addStyleSheetFile("menu.js");
      Doxygen::indexList->addStyleSheetFile("menudata.js");
    }
  }
}

void HtmlGenerator::startDoxyAnchor(const QCString &,const QCString &,
                                    const QCString &anchor, const QCString &,
                                    const QCString &)
{
  m_t << "<a id=\"" << anchor << "\" name=\"" << anchor << "\"></a>";
}

void HtmlGenerator::endDoxyAnchor(const QCString &,const QCString &)
{
}

//void HtmlGenerator::newParagraph()
//{
//  t << "\n<p>\n";
//}

void HtmlGenerator::startParagraph(const QCString &classDef)
{
  if (!classDef.isEmpty())
    m_t << "\n<p class=\"" << classDef << "\">";
  else
    m_t << "\n<p>";
}

void HtmlGenerator::endParagraph()
{
  m_t << "</p>\n";
}

void HtmlGenerator::writeString(const QCString &text)
{
  m_t << text;
}

void HtmlGenerator::startIndexListItem()
{
  m_t << "<li>";
}

void HtmlGenerator::endIndexListItem()
{
  m_t << "</li>\n";
}

void HtmlGenerator::startIndexItem(const QCString &ref,const QCString &f)
{
  //printf("HtmlGenerator::startIndexItem(%s,%s)\n",ref,f);
  if (!ref.isEmpty() || !f.isEmpty())
  {
    if (!ref.isEmpty())
    {
      m_t << "<a class=\"elRef\" ";
      m_t << externalLinkTarget();
    }
    else
    {
      m_t << "<a class=\"el\" ";
    }
    m_t << "href=\"";
    m_t << externalRef(m_relPath,ref,TRUE);
    if (!f.isEmpty()) m_t << addHtmlExtensionIfMissing(f);
    m_t << "\">";
  }
  else
  {
    m_t << "<b>";
  }
}

void HtmlGenerator::endIndexItem(const QCString &ref,const QCString &f)
{
  //printf("HtmlGenerator::endIndexItem(%s,%s,%s)\n",ref,f,name);
  if (!ref.isEmpty() || !f.isEmpty())
  {
    m_t << "</a>";
  }
  else
  {
    m_t << "</b>";
  }
}

void HtmlGenerator::writeStartAnnoItem(const QCString &,const QCString &f,
                                       const QCString &path,const QCString &name)
{
  m_t << "<li>";
  if (!path.isEmpty()) docify(path);
  m_t << "<a class=\"el\" href=\"" << addHtmlExtensionIfMissing(f) << "\">";
  docify(name);
  m_t << "</a> ";
}

void HtmlGenerator::writeObjectLink(const QCString &ref,const QCString &f,
                                    const QCString &anchor, const QCString &name)
{
  if (!ref.isEmpty())
  {
    m_t << "<a class=\"elRef\" ";
    m_t << externalLinkTarget();
  }
  else
  {
    m_t << "<a class=\"el\" ";
  }
  m_t << "href=\"";
  m_t << externalRef(m_relPath,ref,TRUE);
  if (!f.isEmpty()) m_t << addHtmlExtensionIfMissing(f);
  if (!anchor.isEmpty()) m_t << "#" << anchor;
  m_t << "\">";
  docify(name);
  m_t << "</a>";
}

void HtmlGenerator::startTextLink(const QCString &f,const QCString &anchor)
{
  m_t << "<a href=\"";
  if (!f.isEmpty())   m_t << m_relPath << addHtmlExtensionIfMissing(f);
  if (!anchor.isEmpty()) m_t << "#" << anchor;
  m_t << "\">";
}

void HtmlGenerator::endTextLink()
{
  m_t << "</a>";
}

void HtmlGenerator::startHtmlLink(const QCString &url)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  m_t << "<a ";
  if (generateTreeView) m_t << "target=\"top\" ";
  m_t << "href=\"";
  if (!url.isEmpty()) m_t << url;
  m_t << "\">";
}

void HtmlGenerator::endHtmlLink()
{
  m_t << "</a>";
}

void HtmlGenerator::startGroupHeader(int extraIndentLevel)
{
  if (extraIndentLevel==2)
  {
    m_t << "<h4 class=\"groupheader\">";
  }
  else if (extraIndentLevel==1)
  {
    m_t << "<h3 class=\"groupheader\">";
  }
  else // extraIndentLevel==0
  {
    m_t << "<h2 class=\"groupheader\">";
  }
}

void HtmlGenerator::endGroupHeader(int extraIndentLevel)
{
  if (extraIndentLevel==2)
  {
    m_t << "</h4>\n";
  }
  else if (extraIndentLevel==1)
  {
    m_t << "</h3>\n";
  }
  else
  {
    m_t << "</h2>\n";
  }
}

void HtmlGenerator::startSection(const QCString &lab,const QCString &,SectionType type)
{
  switch(type)
  {
    case SectionType::Page:          m_t << "\n\n<h1>"; break;
    case SectionType::Section:       m_t << "\n\n<h2>"; break;
    case SectionType::Subsection:    m_t << "\n\n<h3>"; break;
    case SectionType::Subsubsection: m_t << "\n\n<h4>"; break;
    case SectionType::Paragraph:     m_t << "\n\n<h5>"; break;
    default: ASSERT(0); break;
  }
  m_t << "<a id=\"" << lab << "\" name=\"" << lab << "\"></a>";
}

void HtmlGenerator::endSection(const QCString &,SectionType type)
{
  switch(type)
  {
    case SectionType::Page:          m_t << "</h1>"; break;
    case SectionType::Section:       m_t << "</h2>"; break;
    case SectionType::Subsection:    m_t << "</h3>"; break;
    case SectionType::Subsubsection: m_t << "</h4>"; break;
    case SectionType::Paragraph:     m_t << "</h5>"; break;
    default: ASSERT(0); break;
  }
}

void HtmlGenerator::docify(const QCString &str)
{
  docify(str,FALSE);
}

void HtmlGenerator::docify(const QCString &str,bool inHtmlComment)
{
  if (!str.isEmpty())
  {
    const char *p=str.data();
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
        case '-':  if (inHtmlComment) m_t << "&#45;"; else m_t << "-"; break;
        case '\\':
                   if (*p=='<')
                     { m_t << "&lt;"; p++; }
                   else if (*p=='>')
                     { m_t << "&gt;"; p++; }
		   else if (*p=='(')
                     { m_t << "\\&zwj;("; p++; }
                   else if (*p==')')
                     { m_t << "\\&zwj;)"; p++; }
                   else
                     m_t << "\\";
                   break;
        default:   m_t << c;
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

static void startSectionHeader(TextStream &t,
                               const QCString &relPath,int sectionCount)
{
  //t << "<!-- startSectionHeader -->";
  bool dynamicSections = Config_getBool(HTML_DYNAMIC_SECTIONS);
  if (dynamicSections)
  {
    t << "<div id=\"dynsection-" << sectionCount << "\" "
         "onclick=\"return toggleVisibility(this)\" "
         "class=\"dynheader closed\" "
         "style=\"cursor:pointer;\">\n";
    t << "  <img id=\"dynsection-" << sectionCount << "-trigger\" src=\""
      << relPath << "closed.png\" alt=\"+\"/> ";
  }
  else
  {
    t << "<div class=\"dynheader\">\n";
  }
}

static void endSectionHeader(TextStream &t)
{
  //t << "<!-- endSectionHeader -->";
  t << "</div>\n";
}

static void startSectionSummary(TextStream &t,int sectionCount)
{
  //t << "<!-- startSectionSummary -->";
  bool dynamicSections = Config_getBool(HTML_DYNAMIC_SECTIONS);
  if (dynamicSections)
  {
    t << "<div id=\"dynsection-" << sectionCount << "-summary\" "
         "class=\"dynsummary\" "
         "style=\"display:block;\">\n";
  }
}

static void endSectionSummary(TextStream &t)
{
  //t << "<!-- endSectionSummary -->";
  bool dynamicSections = Config_getBool(HTML_DYNAMIC_SECTIONS);
  if (dynamicSections)
  {
    t << "</div>\n";
  }
}

static void startSectionContent(TextStream &t,int sectionCount)
{
  //t << "<!-- startSectionContent -->";
  bool dynamicSections = Config_getBool(HTML_DYNAMIC_SECTIONS);
  if (dynamicSections)
  {
    t << "<div id=\"dynsection-" << sectionCount << "-content\" "
         "class=\"dyncontent\" "
         "style=\"display:none;\">\n";
  }
  else
  {
    t << "<div class=\"dyncontent\">\n";
  }
}

static void endSectionContent(TextStream &t)
{
  //t << "<!-- endSectionContent -->";
  t << "</div>\n";
}

//----------------------------

void HtmlGenerator::startClassDiagram()
{
  startSectionHeader(m_t,m_relPath,m_sectionCount);
}

void HtmlGenerator::endClassDiagram(const ClassDiagram &d,
                                const QCString &fileName,const QCString &name)
{
  endSectionHeader(m_t);
  startSectionSummary(m_t,m_sectionCount);
  endSectionSummary(m_t);
  startSectionContent(m_t,m_sectionCount);
  TextStream tt;
  d.writeImage(tt,dir(),m_relPath,fileName);
  if (!tt.empty())
  {
    m_t << " <div class=\"center\">\n";
    m_t << "  <img src=\"";
    m_t << m_relPath << fileName << ".png\" usemap=\"#" << convertToId(name);
    m_t << "_map\" alt=\"\"/>\n";
    m_t << "  <map id=\"" << convertToId(name);
    m_t << "_map\" name=\"" << convertToId(name);
    m_t << "_map\">\n";
    m_t << tt.str();
    m_t << "  </map>\n";
    m_t << "</div>";
  }
  else
  {
    m_t << " <div class=\"center\">\n";
    m_t << "  <img src=\"";
    m_t << m_relPath << fileName << ".png\" alt=\"\"/>\n";
    m_t << " </div>";
  }
  endSectionContent(m_t);
  m_sectionCount++;
}


void HtmlGenerator::startMemberList()
{
  DBG_HTML(m_t << "<!-- startMemberList -->\n")
}

void HtmlGenerator::endMemberList()
{
  DBG_HTML(m_t << "<!-- endMemberList -->\n")
}

// anonymous type:
//  0 = single column right aligned
//  1 = double column left aligned
//  2 = single column left aligned
void HtmlGenerator::startMemberItem(const QCString &anchor,MemberItemType type,const QCString &inheritId)
{
  DBG_HTML(m_t << "<!-- startMemberItem() -->\n")
  if (m_emptySection)
  {
    m_t << "<table class=\"memberdecls\">\n";
    m_emptySection=FALSE;
  }
  m_t << "<tr class=\"memitem:" << anchor;
  if (!inheritId.isEmpty())
  {
    m_t << " inherit " << inheritId;
  }
  m_t << "\">";
  insertMemberAlignLeft(type, true);
}

void HtmlGenerator::endMemberItem(MemberItemType type)
{
  if (type==MemberItemType::AnonymousStart || type==MemberItemType::AnonymousEnd)
  {
    insertMemberAlign(false);
  }
  m_t << "</td></tr>\n";
}

void HtmlGenerator::startMemberTemplateParams()
{
}

void HtmlGenerator::endMemberTemplateParams(const QCString &anchor,const QCString &inheritId)
{
  m_t << "</td></tr>\n";
  m_t << "<tr class=\"memitem:" << anchor;
  if (!inheritId.isEmpty())
  {
    m_t << " inherit " << inheritId;
  }
  m_t << "\"><td class=\"memTemplItemLeft\" align=\"right\" valign=\"top\">";
}

void HtmlGenerator::startCompoundTemplateParams()
{
  m_t << "<div class=\"compoundTemplParams\">";
}

void HtmlGenerator::endCompoundTemplateParams()
{
  m_t << "</div>";
}

void HtmlGenerator::insertMemberAlign(bool templ)
{
  DBG_HTML(m_t << "<!-- insertMemberAlign -->\n")
  QCString className = templ ? "memTemplItemRight" : "memItemRight";
  m_t << "&#160;</td><td class=\"" << className << "\" valign=\"bottom\">";
}

void HtmlGenerator::insertMemberAlignLeft(MemberItemType type, bool initTag)
{
  if (!initTag) m_t << "&#160;</td>";
  switch (type)
  {
    case MemberItemType::Normal:         m_t << "<td class=\"memItemLeft\" align=\"right\" valign=\"top\">"; break;
    case MemberItemType::AnonymousStart: m_t << "<td class=\"memItemLeft\" >"; break;
    case MemberItemType::AnonymousEnd:   m_t << "<td class=\"memItemLeft\" valign=\"top\">"; break;
    case MemberItemType::Templated:      m_t << "<td class=\"memTemplParams\" colspan=\"2\">"; break;
  }
}

void HtmlGenerator::startMemberDescription(const QCString &anchor,const QCString &inheritId, bool typ)
{
  DBG_HTML(m_t << "<!-- startMemberDescription -->\n")
  if (m_emptySection)
  {
    m_t << "<table class=\"memberdecls\">\n";
    m_emptySection=FALSE;
  }
  m_t << "<tr class=\"memdesc:" << anchor;
  if (!inheritId.isEmpty())
  {
    m_t << " inherit " << inheritId;
  }
  m_t << "\">";
  m_t << "<td class=\"mdescLeft\">&#160;</td>";
  if (typ) m_t << "<td class=\"mdescLeft\">&#160;</td>";
  m_t << "<td class=\"mdescRight\">";;
}

void HtmlGenerator::endMemberDescription()
{
  DBG_HTML(m_t << "<!-- endMemberDescription -->\n")
  m_t << "<br /></td></tr>\n";
}

void HtmlGenerator::startMemberSections()
{
  DBG_HTML(m_t << "<!-- startMemberSections -->\n")
  m_emptySection=TRUE; // we postpone writing <table> until we actually
                       // write a row to prevent empty tables, which
                       // are not valid XHTML!
}

void HtmlGenerator::endMemberSections()
{
  DBG_HTML(m_t << "<!-- endMemberSections -->\n")
  if (!m_emptySection)
  {
    m_t << "</table>\n";
  }
}

void HtmlGenerator::startMemberHeader(const QCString &anchor, int typ)
{
  DBG_HTML(m_t << "<!-- startMemberHeader -->\n")
  if (!m_emptySection)
  {
    m_t << "</table>";
    m_emptySection=TRUE;
  }
  if (m_emptySection)
  {
    m_t << "<table class=\"memberdecls\">\n";
    m_emptySection=FALSE;
  }
  m_t << "<tr class=\"heading\"><td colspan=\"" << typ << "\"><h2 class=\"groupheader\">";
  if (!anchor.isEmpty())
  {
    m_t << "<a id=\"" << anchor << "\" name=\"" << anchor << "\"></a>\n";
  }
}

void HtmlGenerator::endMemberHeader()
{
  DBG_HTML(m_t << "<!-- endMemberHeader -->\n")
  m_t << "</h2></td></tr>\n";
}

void HtmlGenerator::startMemberSubtitle()
{
  DBG_HTML(m_t << "<!-- startMemberSubtitle -->\n")
  m_t << "<tr><td class=\"ititle\" colspan=\"2\">";
}

void HtmlGenerator::endMemberSubtitle()
{
  DBG_HTML(m_t << "<!-- endMemberSubtitle -->\n")
  m_t << "</td></tr>\n";
}

void HtmlGenerator::startIndexList()
{
  m_t << "<table>\n";
}

void HtmlGenerator::endIndexList()
{
  m_t << "</table>\n";
}

void HtmlGenerator::startIndexKey()
{
  //m_t << "  <tr><td class=\"indexkey\">";
}

void HtmlGenerator::endIndexKey()
{
  //m_t << "</td>";
}

void HtmlGenerator::startIndexValue(bool)
{
  //m_t << "<td class=\"indexvalue\">";
}

void HtmlGenerator::endIndexValue(const QCString &,bool)
{
  //m_t << "</td></tr>\n";
}

void HtmlGenerator::startMemberDocList()
{
  DBG_HTML(m_t << "<!-- startMemberDocList -->\n";)
}

void HtmlGenerator::endMemberDocList()
{
  DBG_HTML(m_t << "<!-- endMemberDocList -->\n";)
}

void HtmlGenerator::startMemberDoc( const QCString &clName, const QCString &memName,
                                    const QCString &anchor, const QCString &title,
                                    int memCount, int memTotal, bool showInline)
{
  DBG_HTML(m_t << "<!-- startMemberDoc -->\n";)
  m_t << "\n<h2 class=\"memtitle\">"
      << "<span class=\"permalink\"><a href=\"#" << anchor << "\">&#9670;&#160;</a></span>";
  docify(title);
  if (memTotal>1)
  {
    m_t << " <span class=\"overload\">[" << memCount << "/" << memTotal <<"]</span>";
  }
  m_t << "</h2>\n";
  m_t << "\n<div class=\"memitem\">\n";
  m_t << "<div class=\"memproto\">\n";
}

void HtmlGenerator::startMemberDocPrefixItem()
{
  DBG_HTML(m_t << "<!-- startMemberDocPrefixItem -->\n";)
  m_t << "<div class=\"memtemplate\">\n";
}

void HtmlGenerator::endMemberDocPrefixItem()
{
  DBG_HTML(m_t << "<!-- endMemberDocPrefixItem -->\n";)
  m_t << "</div>\n";
}

void HtmlGenerator::startMemberDocName(bool /*align*/)
{
  DBG_HTML(m_t << "<!-- startMemberDocName -->\n";)

  m_t << "      <table class=\"memname\">\n";

  m_t << "        <tr>\n";
  m_t << "          <td class=\"memname\">";
}

void HtmlGenerator::endMemberDocName()
{
  DBG_HTML(m_t << "<!-- endMemberDocName -->\n";)
  m_t << "</td>\n";
}

void HtmlGenerator::startParameterList(bool openBracket)
{
  DBG_HTML(m_t << "<!-- startParameterList -->\n";)
  m_t << "          <td>";
  if (openBracket) m_t << "(";
  m_t << "</td>\n";
}

void HtmlGenerator::startParameterType(bool first,const QCString &key)
{
  if (first)
  {
    DBG_HTML(m_t << "<!-- startFirstParameterType -->\n";)
    m_t << "          <td class=\"paramtype\">";
  }
  else
  {
    DBG_HTML(m_t << "<!-- startParameterType -->\n";)
    m_t << "        <tr>\n";
    m_t << "          <td class=\"paramkey\">" << key << "</td>\n";
    m_t << "          <td></td>\n";
    m_t << "          <td class=\"paramtype\">";
  }
}

void HtmlGenerator::endParameterType()
{
  DBG_HTML(m_t << "<!-- endParameterType -->\n";)
  m_t << "&#160;</td>\n";
}

void HtmlGenerator::startParameterName(bool /*oneArgOnly*/)
{
  DBG_HTML(m_t << "<!-- startParameterName -->\n";)
  m_t << "          <td class=\"paramname\">";
}

void HtmlGenerator::endParameterName(bool last,bool emptyList,bool closeBracket)
{
  DBG_HTML(m_t << "<!-- endParameterName -->\n";)
  if (last)
  {
    if (emptyList)
    {
      if (closeBracket) m_t << "</td><td>)";
      m_t << "</td>\n";
      m_t << "          <td>";
    }
    else
    {
      m_t << "&#160;</td>\n";
      m_t << "        </tr>\n";
      m_t << "        <tr>\n";
      m_t << "          <td></td>\n";
      m_t << "          <td>";
      if (closeBracket) m_t << ")";
      m_t << "</td>\n";
      m_t << "          <td></td><td>";
    }
  }
  else
  {
    m_t << "</td>\n";
    m_t << "        </tr>\n";
  }
}

void HtmlGenerator::endParameterList()
{
  DBG_HTML(m_t << "<!-- endParameterList -->\n";)
  m_t << "</td>\n";
  m_t << "        </tr>\n";
}

void HtmlGenerator::exceptionEntry(const QCString &prefix,bool closeBracket)
{
  DBG_HTML(m_t << "<!-- exceptionEntry -->\n";)
  m_t << "</td>\n";
  m_t << "        </tr>\n";
  m_t << "        <tr>\n";
  m_t << "          <td align=\"right\">";
  // colspan 2 so it gets both parameter type and parameter name columns
  if (!prefix.isEmpty())
    m_t << prefix << "</td><td>(</td><td colspan=\"2\">";
  else if (closeBracket)
    m_t << "</td><td>)</td><td></td><td>";
  else
    m_t << "</td><td></td><td colspan=\"2\">";
}

void HtmlGenerator::endMemberDoc(bool hasArgs)
{
  DBG_HTML(m_t << "<!-- endMemberDoc -->\n";)
  if (!hasArgs)
  {
    m_t << "        </tr>\n";
  }
  m_t << "      </table>\n";
 // m_t << "</div>\n";
}

void HtmlGenerator::startDotGraph()
{
  startSectionHeader(m_t,m_relPath,m_sectionCount);
}

void HtmlGenerator::endDotGraph(DotClassGraph &g)
{
  bool generateLegend = Config_getBool(GENERATE_LEGEND);
  bool umlLook = Config_getBool(UML_LOOK);
  endSectionHeader(m_t);
  startSectionSummary(m_t,m_sectionCount);
  endSectionSummary(m_t);
  startSectionContent(m_t,m_sectionCount);

  g.writeGraph(m_t,GOF_BITMAP,EOF_Html,dir(),fileName(),m_relPath,TRUE,TRUE,m_sectionCount);
  if (generateLegend && !umlLook)
  {
    m_t << "<center><span class=\"legend\">[";
    startHtmlLink((m_relPath+"graph_legend"+Doxygen::htmlFileExtension));
    m_t << theTranslator->trLegend();
    endHtmlLink();
    m_t << "]</span></center>";
  }

  endSectionContent(m_t);
  m_sectionCount++;
}

void HtmlGenerator::startInclDepGraph()
{
  startSectionHeader(m_t,m_relPath,m_sectionCount);
}

void HtmlGenerator::endInclDepGraph(DotInclDepGraph &g)
{
  endSectionHeader(m_t);
  startSectionSummary(m_t,m_sectionCount);
  endSectionSummary(m_t);
  startSectionContent(m_t,m_sectionCount);

  g.writeGraph(m_t,GOF_BITMAP,EOF_Html,dir(),fileName(),m_relPath,TRUE,m_sectionCount);

  endSectionContent(m_t);
  m_sectionCount++;
}

void HtmlGenerator::startGroupCollaboration()
{
  startSectionHeader(m_t,m_relPath,m_sectionCount);
}

void HtmlGenerator::endGroupCollaboration(DotGroupCollaboration &g)
{
  endSectionHeader(m_t);
  startSectionSummary(m_t,m_sectionCount);
  endSectionSummary(m_t);
  startSectionContent(m_t,m_sectionCount);

  g.writeGraph(m_t,GOF_BITMAP,EOF_Html,dir(),fileName(),m_relPath,TRUE,m_sectionCount);

  endSectionContent(m_t);
  m_sectionCount++;
}

void HtmlGenerator::startCallGraph()
{
  startSectionHeader(m_t,m_relPath,m_sectionCount);
}

void HtmlGenerator::endCallGraph(DotCallGraph &g)
{
  endSectionHeader(m_t);
  startSectionSummary(m_t,m_sectionCount);
  endSectionSummary(m_t);
  startSectionContent(m_t,m_sectionCount);

  g.writeGraph(m_t,GOF_BITMAP,EOF_Html,dir(),fileName(),m_relPath,TRUE,m_sectionCount);

  endSectionContent(m_t);
  m_sectionCount++;
}

void HtmlGenerator::startDirDepGraph()
{
  startSectionHeader(m_t,m_relPath,m_sectionCount);
}

void HtmlGenerator::endDirDepGraph(DotDirDeps &g)
{
  endSectionHeader(m_t);
  startSectionSummary(m_t,m_sectionCount);
  endSectionSummary(m_t);
  startSectionContent(m_t,m_sectionCount);

  g.writeGraph(m_t,GOF_BITMAP,EOF_Html,dir(),fileName(),m_relPath,TRUE,m_sectionCount);

  endSectionContent(m_t);
  m_sectionCount++;
}

void HtmlGenerator::writeGraphicalHierarchy(DotGfxHierarchyTable &g)
{
  g.writeGraph(m_t,dir(),fileName());
}

void HtmlGenerator::startMemberGroupHeader(bool)
{
  m_t << "<tr><td colspan=\"2\"><div class=\"groupHeader\">";
}

void HtmlGenerator::endMemberGroupHeader()
{
  m_t << "</div></td></tr>\n";
}

void HtmlGenerator::startMemberGroupDocs()
{
  m_t << "<tr><td colspan=\"2\"><div class=\"groupText\">";
}

void HtmlGenerator::endMemberGroupDocs()
{
  m_t << "</div></td></tr>\n";
}

void HtmlGenerator::startMemberGroup()
{
}

void HtmlGenerator::endMemberGroup(bool)
{
}

void HtmlGenerator::startIndent()
{
  DBG_HTML(m_t << "<!-- startIndent -->\n";)

  m_t << "<div class=\"memdoc\">\n";
}

void HtmlGenerator::endIndent()
{
  DBG_HTML(m_t << "<!-- endIndent -->\n";)
  m_t << "\n</div>\n" << "</div>\n";
}

void HtmlGenerator::addIndexItem(const QCString &,const QCString &)
{
}

void HtmlGenerator::writeNonBreakableSpace(int n)
{
  int i;
  for (i=0;i<n;i++)
  {
    m_t << "&#160;";
  }
}

void HtmlGenerator::startDescTable(const QCString &title)
{
  m_t << "<table class=\"fieldtable\">\n"
      << "<tr><th colspan=\"2\">" << title << "</th></tr>";
}
void HtmlGenerator::endDescTable()
{
  m_t << "</table>\n";
}

void HtmlGenerator::startDescTableRow()
{
  m_t << "<tr>";
}

void HtmlGenerator::endDescTableRow()
{
  m_t << "</tr>\n";
}

void HtmlGenerator::startDescTableTitle()
{
  m_t << "<td class=\"fieldname\">";
}

void HtmlGenerator::endDescTableTitle()
{
  m_t << "&#160;</td>";
}

void HtmlGenerator::startDescTableData()
{
  m_t << "<td class=\"fielddoc\">";
}

void HtmlGenerator::endDescTableData()
{
  m_t << "</td>";
}

void HtmlGenerator::startExamples()
{
  m_t << "<dl class=\"section examples\"><dt>";
  docify(theTranslator->trExamples());
  m_t << "</dt>";
}

void HtmlGenerator::endExamples()
{
  m_t << "</dl>\n";
}

void HtmlGenerator::startParamList(ParamListTypes,
                                const QCString &title)
{
  m_t << "<dl><dt><b>";
  docify(title);
  m_t << "</b></dt>";
}

void HtmlGenerator::endParamList()
{
  m_t << "</dl>";
}

void HtmlGenerator::writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *,int id)
{
  const DocNodeAST *astImpl = dynamic_cast<const DocNodeAST*>(ast);
  if (astImpl)
  {
    m_codeGen.setId(id);
    HtmlDocVisitor visitor(m_t,m_codeGen,ctx);
    std::visit(visitor,astImpl->root);
  }
}

//---------------- helpers for index generation -----------------------------

static void startQuickIndexList(TextStream &t,bool compact,bool topLevel=TRUE)
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

static void endQuickIndexList(TextStream &t,bool compact)
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

static void startQuickIndexItem(TextStream &t,const QCString &l,
                                bool hl,bool /*compact*/,
                                const QCString &relPath)
{
  t << "      <li";
  if (hl)
  {
    t << " class=\"current\"";
  }
  t << ">";
  if (!l.isEmpty()) t << "<a href=\"" << correctURL(l,relPath) << "\">";
  t << "<span>";
}

static void endQuickIndexItem(TextStream &t,const QCString &l)
{
  t << "</span>";
  if (!l.isEmpty()) t << "</a>";
  t << "</li>\n";
}

static bool quickLinkVisible(LayoutNavEntry::Kind kind)
{
  const auto &index = Index::instance();
  bool showNamespaces = Config_getBool(SHOW_NAMESPACES);
  switch (kind)
  {
    case LayoutNavEntry::MainPage:           return TRUE;
    case LayoutNavEntry::User:               return TRUE;
    case LayoutNavEntry::UserGroup:          return TRUE;
    case LayoutNavEntry::Pages:              return index.numIndexedPages()>0;
    case LayoutNavEntry::Modules:            return index.numDocumentedGroups()>0;
    case LayoutNavEntry::Namespaces:         return index.numDocumentedNamespaces()>0 && showNamespaces;
    case LayoutNavEntry::NamespaceList:      return index.numDocumentedNamespaces()>0 && showNamespaces;
    case LayoutNavEntry::NamespaceMembers:   return index.numDocumentedNamespaceMembers(NamespaceMemberHighlight::All)>0;
    case LayoutNavEntry::Concepts:           return index.numDocumentedConcepts()>0;
    case LayoutNavEntry::Classes:            return index.numAnnotatedClasses()>0;
    case LayoutNavEntry::ClassList:          return index.numAnnotatedClasses()>0;
    case LayoutNavEntry::ClassIndex:         return index.numAnnotatedClasses()>0;
    case LayoutNavEntry::ClassHierarchy:     return index.numHierarchyClasses()>0;
    case LayoutNavEntry::ClassMembers:       return index.numDocumentedClassMembers(ClassMemberHighlight::All)>0;
    case LayoutNavEntry::Files:              return Config_getBool(SHOW_FILES) && index.numDocumentedFiles()>0;
    case LayoutNavEntry::FileList:           return Config_getBool(SHOW_FILES) && index.numDocumentedFiles()>0;
    case LayoutNavEntry::FileGlobals:        return Config_getBool(SHOW_FILES) && index.numDocumentedFileMembers(FileMemberHighlight::All)>0;
    case LayoutNavEntry::Examples:           return !Doxygen::exampleLinkedMap->empty();
    case LayoutNavEntry::Interfaces:         return index.numAnnotatedInterfaces()>0;
    case LayoutNavEntry::InterfaceList:      return index.numAnnotatedInterfaces()>0;
    case LayoutNavEntry::InterfaceIndex:     return index.numAnnotatedInterfaces()>0;
    case LayoutNavEntry::InterfaceHierarchy: return index.numHierarchyInterfaces()>0;
    case LayoutNavEntry::Structs:            return index.numAnnotatedStructs()>0;
    case LayoutNavEntry::StructList:         return index.numAnnotatedStructs()>0;
    case LayoutNavEntry::StructIndex:        return index.numAnnotatedStructs()>0;
    case LayoutNavEntry::Exceptions:         return index.numAnnotatedExceptions()>0;
    case LayoutNavEntry::ExceptionList:      return index.numAnnotatedExceptions()>0;
    case LayoutNavEntry::ExceptionIndex:     return index.numAnnotatedExceptions()>0;
    case LayoutNavEntry::ExceptionHierarchy: return index.numHierarchyExceptions()>0;
    case LayoutNavEntry::None:             // should never happen, means not properly initialized
      assert(kind != LayoutNavEntry::None);
      return FALSE;
  }
  return FALSE;
}

static void renderQuickLinksAsTree(TextStream &t,const QCString &relPath,LayoutNavEntry *root)

{
  int count=0;
  for (const auto &entry : root->children())
  {
    if (entry->visible() && quickLinkVisible(entry->kind())) count++;
  }
  if (count>0) // at least one item is visible
  {
    startQuickIndexList(t,FALSE);
    for (const auto &entry : root->children())
    {
      if (entry->visible() && quickLinkVisible(entry->kind()))
      {
        QCString url = entry->url();
        t << "<li><a href=\"" << relPath << url << "\"><span>";
        t << fixSpaces(entry->title());
        t << "</span></a>\n";
        // recursive into child list
        renderQuickLinksAsTree(t,relPath,entry.get());
        t << "</li>";
      }
    }
    endQuickIndexList(t,FALSE);
  }
}


static void renderQuickLinksAsTabs(TextStream &t,const QCString &relPath,
                             LayoutNavEntry *hlEntry,LayoutNavEntry::Kind kind,
                             bool highlightParent,bool highlightSearch)
{
  if (hlEntry->parent()) // first draw the tabs for the parent of hlEntry
  {
    renderQuickLinksAsTabs(t,relPath,hlEntry->parent(),kind,highlightParent,highlightSearch);
  }
  if (hlEntry->parent() && !hlEntry->parent()->children().empty()) // draw tabs for row containing hlEntry
  {
    bool topLevel = hlEntry->parent()->parent()==0;
    int count=0;
    for (const auto &entry : hlEntry->parent()->children())
    {
      if (entry->visible() && quickLinkVisible(entry->kind())) count++;
    }
    if (count>0) // at least one item is visible
    {
      startQuickIndexList(t,TRUE,topLevel);
      for (const auto &entry : hlEntry->parent()->children())
      {
        if (entry->visible() && quickLinkVisible(entry->kind()))
        {
          QCString url = entry->url();
          startQuickIndexItem(t,url,
              entry.get()==hlEntry  &&
              (!entry->children().empty() ||
               (entry->kind()==kind && !highlightParent)
              ),
              TRUE,relPath);
          t << fixSpaces(entry->title());
          endQuickIndexItem(t,url);
        }
      }
      if (hlEntry->parent()==LayoutDocManager::instance().rootNavEntry()) // first row is special as it contains the search box
      {
        bool searchEngine      = Config_getBool(SEARCHENGINE);
        bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
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

static void writeDefaultQuickLinks(TextStream &t,bool compact,
                                   HighlightedItem hli,
                                   const QCString &file,
                                   const QCString &relPath)
{
  bool serverBasedSearch = Config_getBool(SERVER_BASED_SEARCH);
  bool searchEngine = Config_getBool(SEARCHENGINE);
  bool externalSearch = Config_getBool(EXTERNAL_SEARCH);
  LayoutNavEntry *root = LayoutDocManager::instance().rootNavEntry();
  LayoutNavEntry::Kind kind = LayoutNavEntry::None;
  LayoutNavEntry::Kind altKind = LayoutNavEntry::None; // fall back for the old layout file
  bool highlightParent=false;
  switch (hli) // map HLI enums to LayoutNavEntry::Kind enums
  {
    case HighlightedItem::Main:                kind = LayoutNavEntry::MainPage;         break;
    case HighlightedItem::Modules:             kind = LayoutNavEntry::Modules;          break;
    case HighlightedItem::Namespaces:          kind = LayoutNavEntry::NamespaceList;    altKind = LayoutNavEntry::Namespaces;  break;
    case HighlightedItem::ClassHierarchy:      kind = LayoutNavEntry::ClassHierarchy;   break;
    case HighlightedItem::InterfaceHierarchy:  kind = LayoutNavEntry::InterfaceHierarchy;   break;
    case HighlightedItem::ExceptionHierarchy:  kind = LayoutNavEntry::ExceptionHierarchy;   break;
    case HighlightedItem::Classes:             kind = LayoutNavEntry::ClassIndex;       altKind = LayoutNavEntry::Classes;     break;
    case HighlightedItem::Concepts:            kind = LayoutNavEntry::Concepts;         break;
    case HighlightedItem::Interfaces:          kind = LayoutNavEntry::InterfaceIndex;   altKind = LayoutNavEntry::Interfaces;  break;
    case HighlightedItem::Structs:             kind = LayoutNavEntry::StructIndex;      altKind = LayoutNavEntry::Structs;     break;
    case HighlightedItem::Exceptions:          kind = LayoutNavEntry::ExceptionIndex;   altKind = LayoutNavEntry::Exceptions;  break;
    case HighlightedItem::AnnotatedClasses:    kind = LayoutNavEntry::ClassList;        altKind = LayoutNavEntry::Classes;     break;
    case HighlightedItem::AnnotatedInterfaces: kind = LayoutNavEntry::InterfaceList; altKind = LayoutNavEntry::Interfaces;  break;
    case HighlightedItem::AnnotatedStructs:    kind = LayoutNavEntry::StructList;       altKind = LayoutNavEntry::Structs;     break;
    case HighlightedItem::AnnotatedExceptions: kind = LayoutNavEntry::ExceptionList; altKind = LayoutNavEntry::Exceptions;  break;
    case HighlightedItem::Files:               kind = LayoutNavEntry::FileList;         altKind = LayoutNavEntry::Files;       break;
    case HighlightedItem::NamespaceMembers:    kind = LayoutNavEntry::NamespaceMembers; break;
    case HighlightedItem::Functions:           kind = LayoutNavEntry::ClassMembers;     break;
    case HighlightedItem::Globals:             kind = LayoutNavEntry::FileGlobals;      break;
    case HighlightedItem::Pages:               kind = LayoutNavEntry::Pages;            break;
    case HighlightedItem::Examples:            kind = LayoutNavEntry::Examples;         break;
    case HighlightedItem::UserGroup:           kind = LayoutNavEntry::UserGroup;        break;
    case HighlightedItem::ClassVisible:        kind = LayoutNavEntry::ClassList;        altKind = LayoutNavEntry::Classes;
                                               highlightParent = true; break;
    case HighlightedItem::ConceptVisible:      kind = LayoutNavEntry::Concepts;
                                               highlightParent = true; break;
    case HighlightedItem::InterfaceVisible:    kind = LayoutNavEntry::InterfaceList;    altKind = LayoutNavEntry::Interfaces;
                                               highlightParent = true; break;
    case HighlightedItem::StructVisible:       kind = LayoutNavEntry::StructList;       altKind = LayoutNavEntry::Structs;
                                               highlightParent = true; break;
    case HighlightedItem::ExceptionVisible:    kind = LayoutNavEntry::ExceptionList;    altKind = LayoutNavEntry::Exceptions;
                                               highlightParent = true; break;
    case HighlightedItem::NamespaceVisible:    kind = LayoutNavEntry::NamespaceList;    altKind = LayoutNavEntry::Namespaces;
                                               highlightParent = true; break;
    case HighlightedItem::FileVisible:         kind = LayoutNavEntry::FileList;         altKind = LayoutNavEntry::Files;
                                               highlightParent = true; break;
    case HighlightedItem::None:   break;
    case HighlightedItem::Search: break;
  }

  if (compact && Config_getBool(HTML_DYNAMIC_MENUS))
  {
    QCString searchPage;
    if (externalSearch)
    {
      searchPage = "search" + Doxygen::htmlFileExtension;
    }
    else
    {
      searchPage = "search.php";
    }
    t << "<script type=\"text/javascript\" src=\"" << relPath << "menudata.js\"></script>\n";
    t << "<script type=\"text/javascript\" src=\"" << relPath << "menu.js\"></script>\n";
    t << "<script type=\"text/javascript\">\n";
    t << "/* @license magnet:?xt=urn:btih:d3d9a9a6595521f9666a5e94cc830dab83b65699&amp;dn=expat.txt MIT */\n";
    t << "$(function() {\n";
    t << "  initMenu('" << relPath << "',"
      << (searchEngine?"true":"false") << ","
      << (serverBasedSearch?"true":"false") << ",'"
      << searchPage << "','"
      << theTranslator->trSearch() << "');\n";
    if (Config_getBool(SEARCHENGINE))
    {
      if (!serverBasedSearch)
      {
        t << "  $(document).ready(function() { init_search(); });\n";
      }
      else
      {
        t << "  $(document).ready(function() {\n"
          << "    if ($('.searchresults').length > 0) { searchBox.DOMSearchField().focus(); }\n"
          << "  });\n";
      }
    }
    t << "});\n";
    t << "/* @license-end */\n";
    t << "</script>\n";
    t << "<div id=\"main-nav\"></div>\n";
  }
  else if (compact) // && !Config_getBool(HTML_DYNAMIC_MENUS)
  {
    // find highlighted index item
    LayoutNavEntry *hlEntry = root->find(kind,kind==LayoutNavEntry::UserGroup ? file : QCString());
    if (!hlEntry && altKind!=LayoutNavEntry::None) { hlEntry=root->find(altKind); kind=altKind; }
    if (!hlEntry) // highlighted item not found in the index! -> just show the level 1 index...
    {
      highlightParent=TRUE;
      hlEntry = root->children().front().get();
      if (hlEntry==0)
      {
        return; // argl, empty index!
      }
    }
    if (kind==LayoutNavEntry::UserGroup)
    {
      LayoutNavEntry *e = hlEntry->children().front().get();
      if (e)
      {
        hlEntry = e;
      }
    }
    renderQuickLinksAsTabs(t,relPath,hlEntry,kind,highlightParent,hli==HighlightedItem::Search);
  }
  else
  {
    renderQuickLinksAsTree(t,relPath,root);
  }
}

void HtmlGenerator::endQuickIndices()
{
  m_t << "</div><!-- top -->\n";
}

QCString HtmlGenerator::writeSplitBarAsString(const QCString &name,const QCString &relpath)
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  QCString result;
  // write split bar
  if (generateTreeView)
  {
    if (!Config_getBool(DISABLE_INDEX) || !Config_getBool(FULL_SIDEBAR))
    {
      result += QCString(
        "<div id=\"side-nav\" class=\"ui-resizable side-nav-resizable\">\n");
    }
    result+= QCString(
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
     "/* @license magnet:?xt=urn:btih:d3d9a9a6595521f9666a5e94cc830dab83b65699&amp;dn=expat.txt MIT */\n"
     "$(document).ready(function(){initNavTree('") +
     QCString(addHtmlExtensionIfMissing(name)) +
     QCString("','") + relpath +
     QCString("'); initResizable(); });\n"
     "/* @license-end */\n"
     "</script>\n"
     "<div id=\"doc-content\">\n");
  }
  return result;
}

void HtmlGenerator::writeSplitBar(const QCString &name)
{
  m_t << writeSplitBarAsString(name,m_relPath);
}

void HtmlGenerator::writeNavigationPath(const QCString &s)
{
  m_t << substitute(s,"$relpath^",m_relPath);
}

void HtmlGenerator::startContents()
{
  m_t << "<div class=\"contents\">\n";
}

void HtmlGenerator::endContents()
{
  m_t << "</div><!-- contents -->\n";
}

void HtmlGenerator::startPageDoc(const QCString &pageTitle)
{
  m_t << "<div>";
}

void HtmlGenerator::endPageDoc()
{
  m_t << "</div><!-- PageDoc -->\n";
}

void HtmlGenerator::writeQuickLinks(bool compact,HighlightedItem hli,const QCString &file)
{
  writeDefaultQuickLinks(m_t,compact,hli,file,m_relPath);
}

// PHP based search script
void HtmlGenerator::writeSearchPage()
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  bool disableIndex = Config_getBool(DISABLE_INDEX);
  QCString projectName = Config_getString(PROJECT_NAME);
  QCString htmlOutput = Config_getString(HTML_OUTPUT);

  // OPENSEARCH_PROVIDER {
  QCString configFileName = htmlOutput+"/search_config.php";
  std::ofstream f = Portable::openOutputStream(configFileName);
  if (f.is_open())
  {
    TextStream t(&f);
    t << "<?php\n\n";
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
    t << "?>\n";
  }
  f.close();

  ResourceMgr::instance().copyResource("search_functions.php",htmlOutput);
  ResourceMgr::instance().copyResource("search_opensearch.php",htmlOutput);
  // OPENSEARCH_PROVIDER }

  QCString fileName = htmlOutput+"/search.php";
  f = Portable::openOutputStream(fileName);
  if (f.is_open())
  {
    TextStream t(&f);
    t << substituteHtmlKeywords(g_header,"Search","");

    t << "<!-- " << theTranslator->trGeneratedBy() << " Doxygen "
      << getDoxygenVersion() << " -->\n";
    t << "<script type=\"text/javascript\">\n";
		t << "/* @license magnet:?xt=urn:btih:d3d9a9a6595521f9666a5e94cc830dab83b65699&amp;dn=expat.txt MIT */\n";
		t << "var searchBox = new SearchBox(\"searchBox\", \""
      << "search/\",'" << Doxygen::htmlFileExtension << "');\n";
		t << "/* @license-end */\n";
    t << "</script>\n";
    if (!Config_getBool(DISABLE_INDEX))
    {
      writeDefaultQuickLinks(t,TRUE,HighlightedItem::Search,QCString(),QCString());
    }
    else
    {
      t << "</div>\n";
    }

    t << "<?php\n";
    t << "require_once \"search_functions.php\";\n";
    t << "main();\n";
    t << "?>\n";

    // Write empty navigation path, to make footer connect properly
    if (generateTreeView)
    {
      t << "</div><!-- doc-content -->\n";
    }

    writePageFooter(t,"Search","","");
  }
  f.close();

  QCString scriptName = htmlOutput+"/search/search.js";
  f = Portable::openOutputStream(scriptName);
  if (f.is_open())
  {
    TextStream t(&f);
    t << ResourceMgr::instance().getAsString("extsearch.js");
  }
  else
  {
     err("Failed to open file '%s' for writing...\n",qPrint(scriptName));
  }
}

void HtmlGenerator::writeExternalSearchPage()
{
  bool generateTreeView = Config_getBool(GENERATE_TREEVIEW);
  QCString dname = Config_getString(HTML_OUTPUT);
  QCString fileName = dname+"/search"+Doxygen::htmlFileExtension;
  std::ofstream f = Portable::openOutputStream(fileName);
  if (f.is_open())
  {
    TextStream t(&f);
    t << substituteHtmlKeywords(g_header,"Search","");

    t << "<!-- " << theTranslator->trGeneratedBy() << " Doxygen "
      << getDoxygenVersion() << " -->\n";
    t << "<script type=\"text/javascript\">\n";
		t << "/* @license magnet:?xt=urn:btih:d3d9a9a6595521f9666a5e94cc830dab83b65699&amp;dn=expat.txt MIT */\n";
		t << "var searchBox = new SearchBox(\"searchBox\", \""
      << "search/\",'" << Doxygen::htmlFileExtension << "');\n";
		t << "/* @license-end */\n";
    t << "</script>\n";
    if (!Config_getBool(DISABLE_INDEX))
    {
      writeDefaultQuickLinks(t,TRUE,HighlightedItem::Search,QCString(),QCString());
      if (!Config_getBool(HTML_DYNAMIC_MENUS)) // for dynamic menus, menu.js creates this part
      {
        t << "            <input type=\"text\" id=\"MSearchField\" name=\"query\" value=\"\" placeholder=\"" << theTranslator->trSearch() <<
             "\" size=\"20\" accesskey=\"S\" onfocus=\"searchBox.OnSearchFieldFocus(true)\" onblur=\"searchBox.OnSearchFieldFocus(false)\"/>\n";
      }
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
      t << "</div>\n";
    }
    t << writeSplitBarAsString("search","");
    t << "<div class=\"header\">\n";
    t << "  <div class=\"headertitle\">\n";
    t << "    <div class=\"title\">" << theTranslator->trSearchResultsTitle() << "</div>\n";
    t << "  </div>\n";
    t << "</div>\n";
    t << "<div class=\"contents\">\n";

    t << "<div id=\"searchresults\"></div>\n";
    t << "</div>\n";

    if (generateTreeView)
    {
      t << "</div><!-- doc-content -->\n";
    }

    writePageFooter(t,"Search","","");

  }
  f.close();

  QCString scriptName = dname+"/search/search.js";
  f = Portable::openOutputStream(scriptName);
  if (f.is_open())
  {
    TextStream t(&f);
    t << "var searchResultsText=["
      << "\"" << theTranslator->trSearchResults(0) << "\","
      << "\"" << theTranslator->trSearchResults(1) << "\","
      << "\"" << theTranslator->trSearchResults(2) << "\"];\n";
    t << "var serverUrl=\"" << Config_getString(SEARCHENGINE_URL) << "\";\n";
    t << "var tagMap = {\n";
    bool first=TRUE;
    // add search mappings
    const StringVector &extraSearchMappings = Config_getList(EXTRA_SEARCH_MAPPINGS);
    for (const auto &ml : extraSearchMappings)
    {
      QCString mapLine = ml.c_str();
      int eqPos = mapLine.find('=');
      if (eqPos!=-1) // tag command contains a destination
      {
        QCString tagName = mapLine.left(eqPos).stripWhiteSpace();
        QCString destName = mapLine.right(mapLine.length()-eqPos-1).stripWhiteSpace();
        if (!tagName.isEmpty())
        {
          if (!first) t << ",\n";
          t << "  \"" << tagName << "\": \"" << destName << "\"";
          first=FALSE;
        }
      }
    }
    if (!first) t << "\n";
    t << "};\n\n";
    t << ResourceMgr::instance().getAsString("extsearch.js");
    t << "\n";
    t << "$(document).ready(function() {\n";
    t << "  var query = trim(getURLParameter('query'));\n";
    t << "  if (query) {\n";
    t << "    searchFor(query,0,20);\n";
    t << "  } else {\n";
    t << "    var results = $('#results');\n";
    t << "    results.html('<p>" << theTranslator->trSearchResults(0) << "</p>');\n";
    t << "  }\n";
    t << "});\n";
  }
  else
  {
     err("Failed to open file '%s' for writing...\n",qPrint(scriptName));
  }
}

void HtmlGenerator::startConstraintList(const QCString &header)
{
  m_t << "<div class=\"typeconstraint\">\n";
  m_t << "<dl><dt><b>" << header << "</b></dt><dd>\n";
  m_t << "<table border=\"0\" cellspacing=\"2\" cellpadding=\"0\">\n";
}

void HtmlGenerator::startConstraintParam()
{
  m_t << "<tr><td valign=\"top\"><em>";
}

void HtmlGenerator::endConstraintParam()
{
  m_t << "</em></td>";
}

void HtmlGenerator::startConstraintType()
{
  m_t << "<td>&#160;:</td><td valign=\"top\"><em>";
}

void HtmlGenerator::endConstraintType()
{
  m_t << "</em></td>";
}

void HtmlGenerator::startConstraintDocs()
{
  m_t << "<td>&#160;";
}

void HtmlGenerator::endConstraintDocs()
{
  m_t << "</td></tr>\n";
}

void HtmlGenerator::endConstraintList()
{
  m_t << "</table>\n";
  m_t << "</dd>\n";
  m_t << "</dl>\n";
  m_t << "</div>\n";
}

void HtmlGenerator::lineBreak(const QCString &style)
{
  if (!style.isEmpty())
  {
    m_t << "<br class=\"" << style << "\" />\n";
  }
  else
  {
    m_t << "<br />\n";
  }
}

void HtmlGenerator::startHeaderSection()
{
  m_t << "<div class=\"header\">\n";
}

void HtmlGenerator::startTitleHead(const QCString &)
{
  m_t << "  <div class=\"headertitle\">";
  startTitle();
}

void HtmlGenerator::endTitleHead(const QCString &,const QCString &)
{
  endTitle();
  m_t << "</div>\n";
}

void HtmlGenerator::endHeaderSection()
{
  m_t << "</div><!--header-->\n";
}

void HtmlGenerator::startInlineHeader()
{
  if (m_emptySection)
  {
    m_t << "<table class=\"memberdecls\">\n";
    m_emptySection=FALSE;
  }
  m_t << "<tr><td colspan=\"2\"><h3>";
}

void HtmlGenerator::endInlineHeader()
{
  m_t << "</h3></td></tr>\n";
}

void HtmlGenerator::startMemberDocSimple(bool isEnum)
{
  DBG_HTML(m_t << "<!-- startMemberDocSimple -->\n";)
  m_t << "<table class=\"fieldtable\">\n";
  m_t << "<tr><th colspan=\"" << (isEnum?"2":"3") << "\">";
  m_t << (isEnum? theTranslator->trEnumerationValues() :
       theTranslator->trCompoundMembers()) << "</th></tr>\n";
}

void HtmlGenerator::endMemberDocSimple(bool)
{
  DBG_HTML(m_t << "<!-- endMemberDocSimple -->\n";)
  m_t << "</table>\n";
}

void HtmlGenerator::startInlineMemberType()
{
  DBG_HTML(m_t << "<!-- startInlineMemberType -->\n";)
  m_t << "<tr><td class=\"fieldtype\">\n";
}

void HtmlGenerator::endInlineMemberType()
{
  DBG_HTML(m_t << "<!-- endInlineMemberType -->\n";)
  m_t << "</td>\n";
}

void HtmlGenerator::startInlineMemberName()
{
  DBG_HTML(m_t << "<!-- startInlineMemberName -->\n";)
  m_t << "<td class=\"fieldname\">\n";
}

void HtmlGenerator::endInlineMemberName()
{
  DBG_HTML(m_t << "<!-- endInlineMemberName -->\n";)
  m_t << "</td>\n";
}

void HtmlGenerator::startInlineMemberDoc()
{
  DBG_HTML(m_t << "<!-- startInlineMemberDoc -->\n";)
  m_t << "<td class=\"fielddoc\">\n";
}

void HtmlGenerator::endInlineMemberDoc()
{
  DBG_HTML(m_t << "<!-- endInlineMemberDoc -->\n";)
  m_t << "</td></tr>\n";
}

void HtmlGenerator::startLabels()
{
  DBG_HTML(m_t << "<!-- startLabels -->\n";)
  m_t << "<span class=\"mlabels\">";
}

void HtmlGenerator::writeLabel(const QCString &l,bool /*isLast*/)
{
  DBG_HTML(m_t << "<!-- writeLabel(" << l << ") -->\n";)
  //m_t << "<tt>[" << l << "]</tt>";
  //if (!isLast) m_t << ", ";
  m_t << "<span class=\"mlabel\">" << l << "</span>";
}

void HtmlGenerator::endLabels()
{
  DBG_HTML(m_t << "<!-- endLabels -->\n";)
  m_t << "</span>";
}

void HtmlGenerator::writeInheritedSectionTitle(
                  const QCString &id,    const QCString &ref,
                  const QCString &file,  const QCString &anchor,
                  const QCString &title, const QCString &name)
{
  DBG_HTML(m_t << "<!-- writeInheritedSectionTitle -->\n";)
  QCString a = anchor;
  if (!a.isEmpty()) a.prepend("#");
  QCString classLink = QCString("<a class=\"el\" ");
  if (!ref.isEmpty())
  {
    classLink+= externalLinkTarget();
    classLink += " href=\"";
    classLink+= externalRef(m_relPath,ref,TRUE);
  }
  else
  {
    classLink += "href=\"";
    classLink+=m_relPath;
  }
  classLink=classLink+addHtmlExtensionIfMissing(file)+a;
  classLink+=QCString("\">")+convertToHtml(name,FALSE)+"</a>";
  m_t << "<tr class=\"inherit_header " << id << "\">"
    << "<td colspan=\"2\" onclick=\"javascript:toggleInherit('" << id << "')\">"
    << "<img src=\"" << m_relPath << "closed.png\" alt=\"-\"/>&#160;"
    << theTranslator->trInheritedFrom(convertToHtml(title,FALSE),classLink)
    << "</td></tr>\n";
}

void HtmlGenerator::writeSummaryLink(const QCString &file,const QCString &anchor,const QCString &title,bool first)
{
  if (first)
  {
    m_t << "  <div class=\"summary\">\n";
  }
  else
  {
    m_t << " &#124;\n";
  }
  m_t << "<a href=\"";
  if (!file.isEmpty())
  {
    m_t << m_relPath << addHtmlExtensionIfMissing(file);
  }
  else if (!anchor.isEmpty())
  {
    m_t << "#";
    m_t << anchor;
  }
  m_t << "\">";
  m_t << title;
  m_t << "</a>";
}

void HtmlGenerator::endMemberDeclaration(const QCString &anchor,const QCString &inheritId)
{
  m_t << "<tr class=\"separator:" << anchor;
  if (!inheritId.isEmpty())
  {
    m_t << " inherit " << inheritId;
  }
  m_t << "\"><td class=\"memSeparator\" colspan=\"2\">&#160;</td></tr>\n";
}

QCString HtmlGenerator::getMathJaxMacros()
{
  return getConvertLatexMacro();
}

QCString HtmlGenerator::getNavTreeCss()
{
  ResourceMgr &mgr = ResourceMgr::instance();
  return replaceVariables(mgr.getAsString("navtree.css"));
}

void HtmlGenerator::writeLocalToc(const SectionRefs &sectionRefs,const LocalToc &localToc)
{
  if (localToc.isHtmlEnabled())
  {
    int maxLevel = localToc.htmlLevel();
    m_t << "<div class=\"toc\">";
    m_t << "<h3>" << theTranslator->trRTFTableOfContents() << "</h3>\n";
    m_t << "<ul>";
    int level=1,l;
    char cs[2];
    cs[1]='\0';
    BoolVector inLi(maxLevel+1,false);
    for (const SectionInfo *si : sectionRefs)
    {
      SectionType type = si->type();
      if (isSection(type))
      {
        //printf("  level=%d title=%s\n",level,qPrint(si->title));
        int nextLevel = static_cast<int>(type);
        if (nextLevel>level)
        {
          for (l=level;l<nextLevel;l++)
          {
            if (l < maxLevel) m_t << "<ul>";
          }
        }
        else if (nextLevel<level)
        {
          for (l=level;l>nextLevel;l--)
          {
            if (l <= maxLevel && inLi[l]) m_t << "</li>\n";
            inLi[l]=false;
            if (l <= maxLevel) m_t << "</ul>\n";
          }
        }
        cs[0]=static_cast<char>('0'+nextLevel);
        if (nextLevel <= maxLevel && inLi[nextLevel])
        {
          m_t << "</li>\n";
        }
        QCString titleDoc = convertToHtml(si->title());
        if (nextLevel <= maxLevel)
        {
          m_t << "<li class=\"level"+QCString(cs)+"\">"
              << "<a href=\"#"+si->label()+"\">"
              << convertToHtml(filterTitle(si->title().isEmpty()?si->label():titleDoc))
              << "</a>";
        }
        inLi[nextLevel]=true;
        level = nextLevel;
      }
    }
    if (level > maxLevel) level = maxLevel;
    while (level>1 && level <= maxLevel)
    {
      if (inLi[level])
      {
        m_t << "</li>\n";
      }
      inLi[level]=FALSE;
      m_t << "</ul>\n";
      level--;
    }
    if (level <= maxLevel && inLi[level]) m_t << "</li>\n";
    m_t << "</ul>\n";
    m_t << "</div>\n";
  }
}

