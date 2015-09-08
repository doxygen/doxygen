{# main navigation row #}
<div id="navrow1" class="tabs">
  <ul class="tablist">
  {# main tab #}
   <li{% if page.highlight=='main' %} class="current"{% endif %}><a href="{{ page.relPath }}index{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.mainPage|nowrap }}</span></a></li>
  {# pages tab #}
    {% if pageTree.tree %}
   <li{% if page.highlight=='pages' %} class="current"{% endif %}><a href="{{ page.relPath }}pages{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.pages|nowrap }}</span></a></li>
    {% endif %}
  {# modules tab #}
    {% if moduleTree.tree %}
   <li{% if page.highlight=='modules' %} class="current"{% endif %}><a href="{{ page.relPath }}modules{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.modules|nowrap }}</span></a></li>
    {% endif %}
  {# namespaces tab #}
    {% if namespaceList %}
   <li{% if page.highlight=='namespaces' %} class="current"{% endif %}><a href="{{ page.relPath }}namespaces{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.namespaces|nowrap }}</span></a></li>
    {% endif %}
  {# classes tab #}
    {% if classList %}
   <li{% if page.highlight=='classes' %} class="current"{% endif %}><a href="{{ page.relPath }}annotated{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.classes|nowrap }}</span></a></li>
    {% endif %}
  {# files tab #}
    {% if fileList %}
   <li{% if page.highlight=='files' %} class="current"{% endif %}><a href="{{ page.relPath }}files{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.files|nowrap }}</span></a></li>
    {% endif %}
  {# examples tab #}
    {% if exampleTree.tree %}
   <li{% if page.highlight=='examples' %} class="current"{% endif %}><a href="{{ page.relPath }}examples{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.examples|nowrap }}</span></a></li>
    {% endif %}
  {# search box #}
  {% if config.SEARCHENGINE %}
    {% if config.SERVER_BASED_SEARCH %}
      {# server based search box #}
   <li>
     <div id="MSearchBox" class="MSearchBoxInactive">
       <div class="left">
         <form id="FSearchBox" action="{{ page.relPath }}search{% if config.EXTERNAL_SEARCH %}{{ config.HTML_FILE_EXTENSION }}{% else %}.php{% endif %}" method="get">
           <img id="MSearchSelect" src="{{ page.relPath }}search/mag.png" alt=""/>
      {% if page.highlight!='search' %}
           <input type="text" id="MSearchField" name="query" value="{{ tr.search }}" size="20" accesskey="S" 
                  onfocus="searchBox.OnSearchFieldFocus(true)" 
                  onblur="searchBox.OnSearchFieldFocus(false)"/>
         </form>
       </div><div class="right"></div>
     </div>
   </li>
      {% endif %}
    {% else %}
      {# client based search box #}
   <li>
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
            id="MSearchCloseImg" border="0" src="{{ page.relPath }}search/close.png" alt=""/></a>
       </span>
     </div>
   </li>
    {% endif %}
  {% endif %}
  </ul>
</div>
{# second navigation row #}
{% if page.highlight=='namespace' or page.highlight=='classes' or page.highlight=='files' %}
<div id="navrow2" class="tabs2">
  <ul class="tablist">
  {# namespace subtabs #}
  {% if page.highlight=='namespaces' %}
    <li{% if page.subhighlight=='namespacelist' %} class="current"{% endif %}><a href="{{ page.relPath }}namespaces{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.namespaceList|nowrap }}</span></a></li>
    <li{% if page.subhighlight=='namespacemembers' %} class="current"{% endif %}><a href="{{ page.relPath }}namespacemembers{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.namespaceMembers|nowrap }}</span></a></li>
  {% endif %}
  {# class subtabs #}
  {% if page.highlight=='classes' %}
    <li{% if page.subhighlight=='classlist' %} class="current"{% endif %}><a href="{{ page.relPath }}annotated{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.classList|nowrap }}</span></a></li>
    <li{% if page.subhighlight=='classindex' %} class="current"{% endif %}><a href="{{ page.relPath }}classes{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.classIndex|nowrap }}</span></a></li>
    {% if classHierarchy.tree %}
    <li{% if page.subhighlight=='classhierarchy' %} class="current"{% endif %}><a href="{{ page.relPath }}hierarchy{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.classHierarchy|nowrap }}</span></a></li>
    {% endif %}
    {% if classMembersIndex.all %}
    <li{% if page.subhighlight=='classmembers' %} class="current"{% endif %}><a href="{{ page.relPath }}functions{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.classMembers|nowrap }}</span></a></li>
    {% endif %}
  {% endif %}
  {# file subtabs #}
  {% if page.highlight=='files' %}
    <li{% if page.subhighlight=='filelist' %} class="current"{% endif %}><a href="{{ page.relPath }}files{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.fileList|nowrap }}</span></a></li>
    <li{% if page.subhighlight=='filemembers' %} class="current"{% endif %}><a href="{{ page.relPath }}globals{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.fileMembers|nowrap }}</span></a></li>
  {% endif %}
  </ul>
</div>
{% endif %}
