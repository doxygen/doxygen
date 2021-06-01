{% block header %}
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "https://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/xhtml;charset=UTF-8"/>
<meta http-equiv="X-UA-Compatible" content="IE=11"/>
<meta name="generator" content="Doxygen {{ doxygen.version }}"/>
<meta name="viewport" content="width=device-width, initial-scale=1"/>
<title>{% if config.PROJECT_NAME %}{{ config.PROJECT_NAME }}: {% endif %}{{ page.title }}</title>
<link href="{{ page.relPath }}tabs.css" rel="stylesheet" type="text/css"/>
{% if config.DISABLE_INDEX and config.FULL_SIDEBAR %}
<script type="text/javascript">var page_layout=1;</script>
{% endif %}
<script type="text/javascript" src="{{ page.relPath }}jquery.js"></script>
<script type="text/javascript" src="{{ page.relPath }}dynsections.js"></script>
{% if config.GENERATE_TREEVIEW %}
<link href="{{ page.relPath }}navtree.css" rel="stylesheet" type="text/css"/>
<script type="text/javascript" src="{{ page.relPath }}resize.js"></script>
<script type="text/javascript" src="{{ page.relPath }}navtreedata.js"></script>
<script type="text/javascript" src="{{ page.relPath }}navtree.js"></script>
{% endif %}
{% if config.SEARCHENGINE %}
<link href="{{ page.relPath }}search/search.css" rel="stylesheet" type="text/css"/>
  {% if not config.SERVER_BASED_SEARCH %}
<script type="text/javascript" src="{{ page.relPath }}search/searchdata.js"></script>
  {% endif %}
<script type="text/javascript" src="{{ page.relPath }}search/search.js"></script>
  {% if config.SERVER_BASED_SEARCH %}
<script type="text/javascript">
  /* @license magnet:?xt=urn:btih:cf05388f2679ee054f2beb29a391d25f4e673ac3&amp;dn=gpl-2.0.txt GPL-v2 */
  $(document).ready(function() {
    if ($('.searchresults').length > 0) { searchBox.DOMSearchField().focus(); }
  });
  /* @license-end */
</script>
<link rel="search" href="{{ page.relPath }}search-opensearch.php?v=opensearch.xml" type="application/opensearchdescription+xml" title="{{ config.PROJECT_NAME }}"/>
  {% elif config.DISABLE_INDEX or not config.HTML_DYNAMIC_MENUS %}
<script type="text/javascript">
  /* @license magnet:?xt=urn:btih:cf05388f2679ee054f2beb29a391d25f4e673ac3&amp;dn=gpl-2.0.txt GPL-v2 */
  $(document).ready(function() { init_search(); });
  /* @license-end */
</script>
  {% endif %}
{% endif %}
{% if config.USE_MATHJAX %}
{% if config.MATHJAX_VERSION=="MathJax_3" %}
<script src="https://polyfill.io/v3/polyfill.min.js?features=es6"></script>
<script type="text/javascript">
window.MathJax = {
  options: {
    ignoreHtmlClass: 'tex2jax_ignore',
    processHtmlClass: 'tex2jax_process'
  },
  tex: {
{% if doxygen.mathJaxMacros %}
    macros: { {{ doxygen.mathJaxMacros|raw }} },
{% endif %}
    packages: ['base','configmacros'{% if doxygen.mathJaxMacros %},'newcommand'{% endif %}{% for ext in config.MATHJAX_EXTENSIONS %},'{{ ext }}'{% endfor %}]
  }
};
{{ doxygen.mathJaxCodeFile }}
</script>
<script type="text/javascript" id="MathJax-script" async="async" src="{{ config.MATHJAX_RELPATH }}{% if config.MATHJAX_RELPATH|relative %}{{ page.relPath }}{% endif %}es5/tex-{{ config.MATHJAX_FORMAT|lower }}.js"></script>
{% else %}{# MathJax_2 #}
<script type="text/x-mathjax-config">
MathJax.Hub.Config({
  extensions: ["tex2jax.js"{% for ext in config.MATHJAX_EXTENSIONS %}, "{{ ext }}.js"{% endfor %}],
  jax: ["input/TeX","output/{{ config.MATHJAX_FORMAT|default:'HTML-CSS' }}"],
{% if doxygen.mathJaxMacros %}
  TeX: { Macros: { {{ doxygen.mathJaxMacros|raw }} } }
{% endif %}
});
{{ doxygen.mathJaxCodeFile }}
</script>
<script type="text/javascript" async="async" src="{{ config.MATHJAX_RELPATH }}{% if config.MATHJAX_RELPATH|relative %}{{ page.relPath }}{% endif %}MathJax.js"></script>
{% endif %}{# MathJax_3 #}
{% endif %}{# MathJax #}
<link href="{{ page.relPath }}{{ config.HTML_STYLESHEET|default:'doxygen.css' }}" rel="stylesheet" type="text/css" />
{% if config.HTML_EXTRA_STYLESHEET %}
<link href="{{ page.relPath }}{{ config.HTML_EXTRA_STYLESHEET }}" rel="stylesheet" type="text/css" />
{% endif %}
</head>
<body>
{% endblock %}
{% if config.DISABLE_INDEX and config.FULL_SIDEBAR %}
<div id="side-nav" class="ui-resizable side-nav-resizable"><!-- do not remove this div, it is closed by doxygen! -->
{% endif %}
<div id="top"><!-- do not remove this div, it is closed by doxygen! -->
{% block titlearea %}
{% if config.PROJECT_NAME or config.PROJECT_BRIEF or config.PROJECT_LOGO or config.DISABLE_INDEX and config.SEARCHENGINE %}
<div id="titlearea">
<table cellspacing="0" cellpadding="0">
 <tbody>
 <tr style="height: 56px;">
 {% if config.PROJECT_LOGO %}
  <td id="projectlogo"><img alt="Logo" src="{{ page.relPath }}{{ config.PROJECT_LOGO|stripPath }}"/></td>
 {% endif %}
  <td id="projectalign" style="padding-left: 0.5em;">
 {% if config.PROJECT_NAME %}
   <div id="projectname">{{ config.PROJECT_NAME }}{% if config.PROJECT_NUMBER %}<span id="projectnumber">&#160;{{ config.PROJECT_NUMBER }}</span>{% endif %}
   </div>
 {% endif %}
 {% if config.PROJECT_BRIEF %}
   <div id="projectbrief">{{ config.PROJECT_BRIEF }}</div>
 {% endif %}
  </td>
 {% if config.DISABLE_INDEX and config.SEARCHENGINE %}{# search box is part of title area #}
  {% if config.GENERATE_TREEVIEW and config.FULL_SIDEBAR %}{# search box separate row #}
 </tr>
 <tr>
  <td colspan="2">
  {% else %}
  <td>
  {% endif %}
  {% if config.SERVER_BASED_SEARCH %}
   <div id="MSearchBox" class="MSearchBoxInactive">
    <div class="left">
     <form id="FSearchBox" action="{{ page.relPath }}{% if config.EXTERNAL_SEARCH %}search{{ doxygen.htmlFileExtension }}{% else %}search.php{% endif %}" method="get">
      <img id="MSearchSelect" src="{{ page.relPath }}search/mag.svg" alt=""/>
      <input type="text" id="MSearchField" name="query" value="{{ tr.search }}" size="20" accesskey="S"
                onfocus="searchBox.OnSearchFieldFocus(true)"
                onblur="searchBox.OnSearchFieldFocus(false)"/>
     </form>
    </div>
    <div class="right"></div>
   </div>
  {% else %}{# !SERVER_BASED_SEARCH #}
   <div id="MSearchBox" class="MSearchBoxInactive">
    <span class="left">
      <img id="MSearchSelect" src="{{ page.relPath }}search/mag_sel.svg"
           onmouseover="return searchBox.OnSearchSelectShow()"
           onmouseout="return searchBox.OnSearchSelectHide()"
           alt=""/>
      <input type="text" id="MSearchField" value="{{ tr.search }}" accesskey="S"
           onfocus="searchBox.OnSearchFieldFocus(true)"
           onblur="searchBox.OnSearchFieldFocus(false)"
           onkeyup="searchBox.OnSearchFieldChange(event)"/>
    </span><span class="right">
        <a id="MSearchClose" href="javascript:searchBox.CloseResultsWindow()"><img
           id="MSearchCloseImg" border="0" src="{{ page.relPath }}search/close.svg"
           alt=""/></a>
    </span>
   </div>
  </td>
  {% endif %}{# SERVER_BASED_SEARCH #}
 {% endif %}{# DISABLE_INDEX and SEARCHENGINE #}
 </tr>
 </tbody>
</table>
</div>
{% endif %}{# titlearea visible #}
{% endblock %}
<!-- end header part -->
<!-- Generated by Doxygen {{ doxygen.version }} -->
{% block search %}
{% if config.SEARCHENGINE %}
<script type="text/javascript">
/* @license magnet:?xt=urn:btih:cf05388f2679ee054f2beb29a391d25f4e673ac3&amp;dn=gpl-2.0.txt GPL-v2 */
	var searchBox = new SearchBox("searchBox", "{{ page.relPath }}search",false,'{{ tr.search }}','{{ config.HTML_FILE_EXTENSION }}');
/* @license-end */
</script>
{% endif %}
{% endblock %}

{% block tabs %}
{% if config.HTML_DYNAMIC_MENUS %}
<script type="text/javascript" src="menudata.js"></script>
<script type="text/javascript" src="menu.js"></script>
<script type="text/javascript">
/* @license magnet:?xt=urn:btih:cf05388f2679ee054f2beb29a391d25f4e673ac3&amp;dn=gpl-2.0.txt GPL-v2 */
$(function() {
 initMenu('{{ page.relPath }}',{% if config.SEARCHENGINE %}true{% else %}false{% endif %},{% if config.SERVER_BASED_SEARCH %}true{% else %}false{% endif %},{% if config.EXTERNAL_SEARCH %}'search{{ config.HTML_FILE_EXTENSION }}'{% else %}'search.php'{% endif %},'{{ tr.search }}');
 {% if config.SEARCHENGINE %}
 $(document).ready(function() { {% if not config.SERVER_BASED_SEARCH %}init_search();{% else %}if ($('.searchresults').length > 0) { searchBox.DOMSearchField().focus(); } {% endif %}});
 {% endif %}
});
/* @license-end */
</script>
<div id="main-nav"></div>
{% endif %}

{% if not config.DISABLE_INDEX and not config.HTML_DYNAMIC_MENUS %}
{% include 'htmltabs.tpl' %}
{% endif %}
</div><!-- top -->

{% endblock %}

{% block navpath %}
{% endblock %}

{% block splitbar %}
{% if config.GENERATE_TREEVIEW %}
{% if not config.DISABLE_INDEX or not config.FULL_SIDEBAR %}
<div id="side-nav" class="ui-resizable side-nav-resizable">
{% endif %}
  <div id="nav-tree">
    <div id="nav-tree-contents">
      <div id="nav-sync" class="sync"></div>
    </div>
  </div>
  <div id="splitbar" style="-moz-user-select:none;"
       class="ui-resizable-handle">
  </div>
</div>
<script type="text/javascript">
/* @license magnet:?xt=urn:btih:cf05388f2679ee054f2beb29a391d25f4e673ac3&amp;dn=gpl-2.0.txt GPL-v2 */
$(document).ready(function(){initNavTree('{{ page.fileName }}{% if page_postfix %}{{ page_postfix }}{% endif %}{{ config.HTML_FILE_EXTENSION }}','{{ page.relPath }}'); initResizable(); });
/* @license-end */
</script>
<div id="doc-content">
{% endif %}
{% endblock %}

{% block searchInfo %}
{% if config.SEARCHENGINE and not config.SERVER_BASED_SEARCH %}
<!-- window showing the filter options -->
<div id="MSearchSelectWindow"
     onmouseover="return searchBox.OnSearchSelectShow()"
     onmouseout="return searchBox.OnSearchSelectHide()"
     onkeydown="return searchBox.OnSearchSelectKey(event)">
</div>
<!-- iframe showing the search results (closed by default) -->
<div id="MSearchResultsWindow">
<iframe src="javascript:void(0)" frameborder="0"
	name="MSearchResults" id="MSearchResults">
</iframe>
</div>
{% endif %}
{% endblock %}

<div class="header">
{% block title %}
<div class="headertitle">
<div class="title">{{ page.title }}</div></div>
{% endblock %}
</div><!-- header -->

{% block content %}
{% endblock %}

{% block endsplitbar %}
{% if config.GENERATE_TREEVIEW %}
</div><!-- doc-content -->
{% endif %}
{% endblock %}

{% block footer %}
<!-- start footer part -->
{% if config.GENERATE_TREEVIEW %}
<div id="nav-path" class="navpath">{# id is needed for treeview function! #}
  <ul>
    {# navpath #}
    {% if page|get:'navigationPath' %}
      {% for obj in page.navigationPath %}
        <li class="navelem">
          {% with text=obj.text %}
            {% include 'htmlobjlink.tpl' %}
          {% endwith %}
        </li>
      {% endfor %}
    {% endif %}
    <li class="footer">
{% if config.HTML_TIMESTAMP %}
{{ tr.generatedAt:doxygen.date,config.PROJECT_NAME }}
{% else %}
{{ tr.generatedBy }}
{% endif %}
    <a href="https://www.doxygen.org/index.html"><img class="footer" src="{{ page.relPath }}doxygen.svg" width="104" height="31" alt="doxygen"/></a> {{ doxygen.version }} </li>
  </ul>
</div>
{% else %}
  <hr class="footer"/><address class="footer"><small>
{% if config.HTML_TIMESTAMP %}{{ tr.generatedAt:doxygen.date,config.PROJECT_NAME }}{% else %}{{ tr.generatedBy }}{% endif %}&#160;<a href="https://www.doxygen.org/index.html"><img class="footer" src="{{ page.relPath }}doxygen.svg" width="104" height="31" alt="doxygen"/></a> {{ doxygen.version }}
</small></address>
{% endif %}
</body>
</html>
{% endblock %}
