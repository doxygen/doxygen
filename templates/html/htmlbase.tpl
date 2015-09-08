{% block header %}
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/xhtml;charset=UTF-8"/>
<meta http-equiv="X-UA-Compatible" content="IE=9"/>
<meta name="generator" content="Doxygen {{ doxygen.version }}"/>
<title>{{ config.PROJECT_NAME }}: {{ page.title }}</title>
<link href="{{ page.relPath }}tabs.css" rel="stylesheet" type="text/css"/>
<script type="text/javascript" src="{{ page.relPath }}jquery.js"></script>
<script type="text/javascript" src="{{ page.relPath }}dynsections.js"></script>
{% if config.GENERATE_TREEVIEW %}
<link href="{{ page.relPath }}navtree.css" rel="stylesheet" type="text/css"/>
<script type="text/javascript" src="{{ page.relPath }}resize.js"></script>
<script type="text/javascript" src="{{ page.relPath }}navtreedata.js"></script>
<script type="text/javascript" src="{{ page.relPath }}navtree.js"></script>
<script type="text/javascript">
  $(document).ready(initResizable);
  $(window).load(resizeHeight);
</script>
{% endif %}
{% if config.SEARCHENGINE %}
<link href="{{ page.relPath }}search/search.css" rel="stylesheet" type="text/css"/>
  {% if not config.SERVER_BASED_SEARCH %}
<script type="text/javascript" src="{{ page.relPath }}search/searchdata.js"></script>
  {% endif %}
<script type="text/javascript" src="{{ page.relPath }}search/search.js"></script>
  {% if config.SERVER_BASED_SEARCH %}
<script type="text/javascript">
  $(document).ready(function() {
    if ($('.searchresults').length > 0) { searchBox.DOMSearchField().focus(); }
  });
</script>
<link rel="search" href="{{ page.relPath }}search-opensearch.php?v=opensearch.xml" type="application/opensearchdescription+xml" title="{{ config.PROJECT_NAME }}"/>
  {% else %}
<script type="text/javascript">
  $(document).ready(function() { searchBox.OnSelectItem(0); });
</script>
  {% endif %}
{% endif %}
{% if config.USE_MATHJAX %}
<script type="text/x-mathjax-config">
  MathJax.Hub.Config({
    extensions: ["tex2jax.js"{% for ext in config.MATHJAX_EXTENSIONS %}, "{{ ext }}"{% endfor %}],
    jax: ["input/TeX","output/{{ config.MATHJAX_FORMAT|default:'HTML-CSS' }}"],
});
{{ doxygen.mathJaxCodeFile }}
</script>
<script type="text/javascript" src="{{ config.MATHJAX_RELPATH }}{% if config.MATHJAX_RELPATH|relative %}{{ page.relPath }}{% endif %}MathJax.js"></script>
{% endif %}{# MathJax #}
<link href="{{ page.relPath }}{{ config.HTML_STYLESHEET|default:'doxygen.css' }}" rel="stylesheet" type="text/css" />
{% if config.HTML_EXTRA_STYLESHEET %}
<link href="{{ page.relPath }}{{ config.HTML_EXTRA_STYLESHEET }}" rel="stylesheet" type="text/css" />
{% endif %}
</head>
<body>
{% endblock %}
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
  <td style="padding-left: 0.5em;">
 {% if config.PROJECT_NAME %}
   <div id="projectname">{{ config.PROJECT_NAME }}
  {% if config.PROJECT_NUMBER %}
    <span id="projectnumber">{{ config.PROJECT_NUMBER }}</span>
  {% endif %}
   </div>
 {% endif %}
 {% if config.PROJECT_BRIEF %}
   <div id="projectbrief">{{ config.PROJECT_BRIEF }}</div>
 {% endif %}
  </td>
 {% if config.DISABLE_INDEX and config.SEARCHENGINE %}{# search box is part of title area #}
  <td>
  {% if config.SERVER_BASED_SEARCH %}
   <div id="MSearchBox" class="MSearchBoxInactive">
    <div class="left">
     <form id="FSearchBox" action="{{ page.relPath }}{% if config.EXTERNAL_SEARCH %}search{{ doxygen.htmlFileExtension }}{% else %}search.php{% endif %}" method="get">
      <img id="MSearchSelect" src="{{ page.relPath }}search/mag.png" alt=""/>
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
      <img id="MSearchSelect" src="{{ page.relPath }}search/mag_sel.png"
           onmouseover="return searchBox.OnSearchSelectShow()"
           onmouseout="return searchBox.OnSearchSelectHide()"
           alt=""/>
      <input type="text" id="MSearchField" value="{{ tr.search }}" accesskey="S"
           onfocus="searchBox.OnSearchFieldFocus(true)"
           onblur="searchBox.OnSearchFieldFocus(false)"
           onkeyup="searchBox.OnSearchFieldChange(event)"/>
    </span><span class="right">
        <a id="MSearchClose" href="javascript:searchBox.CloseResultsWindow()"><img
           id="MSearchCloseImg" border="0" src="{{ page.relPath }}search/close.png"
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
var searchBox = new SearchBox("searchBox", "{{ page.relPath }}search",false,'{{ tr.search }}');
</script>
{% endif %}
{% endblock %}

{% block tabs %}
{% if not config.DISABLE_INDEX %}
{% include 'htmltabs.tpl' %}
{% endif %}
{% endblock %}

{% block navpath %}
{% endblock %}


</div><!-- top -->
{% block splitbar %}
{% if config.GENERATE_TREEVIEW %}
<div id="side-nav" class="ui-resizable side-nav-resizable">
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
$(document).ready(function(){initNavTree('{{ page.fileName }}{% if page_postfix %}{{ page_postfix }}{% endif %}{{ config.HTML_FILE_EXTENSION }}','{{ page.relPath }}');});
</script>
<div id="doc-content">
{% endif %}
{% endblock %}

{% block searchInfo %}
{% if config.SEARCHENGINE and not config.SERVER_BASED_SEARCH %}
<!-- window showing the filter options -->
<div id="MSearchSelectWindow" onmouseover="return searchBox.OnSearchSelectShow()"
     onmouseout="return searchBox.OnSearchSelectHide()"
     onkeydown="return searchBox.OnSearchSelectKey(event)">
</div>
<!-- iframe showing the search results (closed by default) -->
<div id="MSearchResultsWindow">
<iframe src="javascript:void(0)" frameborder="0" name="MSearchResults" id="MSearchResults">
</iframe>
</div>
{% endif %}
{% endblock %}

<div class="header">
{% block title %}
  <div class="headertitle"><div class="title">{{ page.title }}</div></div>
{% endblock %}
</div>

{% block content %}
{% endblock %}

{% block endsplitbar %}
{% if config.GENERATE_TREEVIEW %}
</div><!-- content -->
{% endif %}
{% endblock %}

{% block footer %}
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
    <a href="http://www.doxygen.org/index.html">
    <img class="footer" src="{{ page.relPath }}doxygen.png" alt="doxygen"/></a> {{ doxygen.version }} </li>
  </ul>
</div>
{% else %}
  <hr class="footer"/><address class="footer"><small>
{% if config.HTML_TIMESTAMP %}
{{ tr.generatedAt:doxygen.date,config.PROJECT_NAME }}
{% else %}
{{ tr.generatedBy }}
{% endif %}
&#160;<a href="http://www.doxygen.org/index.html"><img class="footer" src="{{ page.relPath }}doxygen.png" alt="doxygen"/></a> 
  {{ doxygen.version }}
  </small></address>
{% endif %}
</body>
</html>
{% endblock %}
