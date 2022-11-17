{% msg %}----- Start generating HTML output for {{ config.PROJECT_NAME }} from template ----{% endmsg %}

{# ---- copy fixed resources to the output ----- #}

{% resource 'doxygen.css' %}
{% if config.HTML_DYNAMIC_MENUS %}
  {% resource 'tabs.css' %}
{% else %}
  {% resource 'fixed_tabs.css' as 'tabs.css' %}
{% endif %}
{% resource 'jquery.js' %}
{% resource 'dynsections.js' %}
{% if config.SOURCE_BROWSER and config.SOURCE_TOOLTIPS %}
{% resource 'dynsections_tooltips.js' append 'dynsections.js' %}
{% endif %}
{% resource 'tab_a.lum' %}
{% resource 'tab_b.lum' %}
{% resource 'tab_h.lum' %}
{% resource 'tab_s.lum' %}
{% resource 'tab_h.lum' %}
{% resource 'bc_s.luma' %}
{% resource 'doxygen.svg' %}
{% resource 'closed.luma' %}
{% resource 'open.luma' %}
{% resource 'bdwn.luma' %}
{% resource 'sync_on.luma' %}
{% resource 'sync_off.luma' %}

{# navigation #}
{% resource 'nav_f.lum' %}
{% resource 'nav_g.png' %}
{% resource 'nav_h.lum' %}
{% if config.GENERATE_TREEVIEW %}
{% resource 'navtree.css' %}
{% resource 'navtree.js' %}
{% resource 'resize.js' %}
{% endif %}
{% if not config.DISABLE_INDEX and config.HTML_DYNAMIC_MENUS %}
{% resource 'menu.js' %}
{% endif %}
{% resource 'doc.luma' %}
{% resource 'folderopen.luma' %}
{% resource 'folderclosed.luma' %}
{% resource 'splitbar.lum' %}

{# general search resources #}
{% if config.DISABLE_INDEX %}
  {% if config.GENERATE_TREEVIEW and config.FULL_SIDEBAR %}
  {% resource 'search_sidebar.css'   as 'search/search.css'   %}
  {% else %}
  {% resource 'search_nomenu.css'    as 'search/search.css'   %}
  {% endif %}
{% else %}
  {% if not config.HTML_DYNAMIC_MENUS %}
  {% resource 'search_fixedtabs.css' as 'search/search.css'   %}
  {% else %}
  {% resource 'search.css'           as 'search/search.css'   %}
  {% endif %}
{% endif %}
{% resource 'search_common.css'      append 'search/search.css' %}
{% create 'search/nomatches.html' from 'nomatches.tpl' %}

{% if config.SERVER_BASED_SEARCH %}
  {# server side search resources #}
  {% resource 'mag.svg'     as 'search/mag.svg'     %}
  {% resource 'extsearch.js as 'search/search.js'   %}
  {% resource 'search_functions.php'  as 'search/search_functions.php'  %}
  {% resource 'search_opensearch.php' as 'search/search_opensearch.php' %}
{% else %}
  {# client side search resources #}
  {% resource 'mag_sel.svg' as 'search/mag_sel.svg' %}
  {% resource 'close.svg'   as 'search/close.svg'   %}
  {% resource 'search.js'   as 'search/search.js'   %}
{% endif %}

{# interactive SVGs #}
{% if config.INTERACTIVE_SVG %}
  {% resource 'svgpan.js' %}
{% endif %}

{# -------------------------------------------------- #}

{# global constants #}
{% set maxItemsForFlatList=30 %}
{% set maxItemsForMultiPageList=200 %}

{# global variable #}
{% set page_postfix='' %}

{# open the global navigation index #}
{% if config.PROJECT_NAME %}
  {% indexentry nav name=config.PROJECT_NAME file='index' anchor='' isReference=False separateIndex=False addToIndex=True %}
{% else %}
  {% indexentry nav name=tr.mainPage file='index' anchor='' isReference=False separateIndex=False addToIndex=True %}
{% endif %}
{% opensubindex nav %}

{# ----------- HTML DOCUMENTATION PAGES ------------ #}

{# write main page documentation #}
{% with page=mainPage compound=mainPage isMainPage=True %}
  {% create mainPage.fileName|append:config.HTML_FILE_EXTENSION from 'htmlpage.tpl' %}
{% endwith %}

{# write namespace documentation pages #}
{% for compound in namespaceList %}
  {% with page=compound %}
    {% create compound.fileName|append:config.HTML_FILE_EXTENSION from 'htmlnamespace.tpl' %}
  {% endwith %}
{% endfor %}

{# write concept documentation pages #}
{% for compound in conceptList %}
  {% with page=compound %}
    {% create compound.fileName|append:config.HTML_FILE_EXTENSION from 'htmlconcept.tpl' %}
  {% endwith %}
{% endfor %}

{# write class documentation pages #}
{% for compound in classList %}
  {% with page=compound %}
    {% create compound.fileName|append:config.HTML_FILE_EXTENSION from 'htmlclass.tpl' %}
    {% if compound.allMembersList and not config.OPTIMIZE_OUTPUT_FOR_C %}
      {% create compound.allMembersFileName|append:config.HTML_FILE_EXTENSION from 'htmlallmembers.tpl' %}
    {% endif %}
  {% endwith %}
{% endfor %}

{# write the file sources #}
{% for compound in fileList %}
  {% with page=compound %}
    {# TODO: to deal with clang optimization, we need to write the sources in a different order! #}
    {% if compound.hasSourceFile %}
      {% create compound.sourceFileName|append:config.HTML_FILE_EXTENSION from 'htmlsource.tpl' %}
    {% endif %}
  {% endwith %}
{% endfor %}

{# write file documentation pages #}
{% for compound in fileList %}
  {% with page=compound %}
    {% create compound.fileName|append:config.HTML_FILE_EXTENSION from 'htmlfile.tpl' %}
  {% endwith %}
{% endfor %}

{# write related page documentation #}
{% for compound in pageList %}
  {% with page=compound isMainPage=False %}
    {% create compound.fileName|append:config.HTML_FILE_EXTENSION from 'htmlpage.tpl' %}
  {% endwith %}
{% endfor %}

{# write module documentation #}
{% for compound in moduleList %}
  {% with page=compound %}
    {% create compound.fileName|append:config.HTML_FILE_EXTENSION from 'htmlmodule.tpl' %}
  {% endwith %}
{% endfor %}

{# write example documentation #}
{% for compound in exampleList %}
  {% with page=compound %}
    {% create compound.fileName|append:config.HTML_FILE_EXTENSION from 'htmlexample.tpl' %}
  {% endwith %}
{% endfor %}

{# ----------- INDEXES ------------ #}

{# --- related pages --- #}
{% if pageTree.tree %}
  {% with page=pageTree %}
    {% create pageTree.fileName|append:config.HTML_FILE_EXTENSION from 'htmlpages.tpl' %}
  {% endwith %}
{% endif %}

{# ---- examples --- #}
{% if exampleTree.tree %}
  {% with page=exampleTree %}
    {% create exampleTree.fileName|append:config.HTML_FILE_EXTENSION from 'htmlexamples.tpl' %}
  {% endwith %}
{% endif %}

{# --- modules --- #}
{% if moduleTree.tree %}
  {% with page=moduleTree %}
    {% create moduleTree.fileName|append:config.HTML_FILE_EXTENSION from 'htmlmodules.tpl' %}
  {% endwith %}
{% endif %}

{# --- namespaces --- #}
{% if namespaceList %}
  {% indexentry nav name=tr.namespaces file='namespaces' anchor='' isReference=False separateIndex=False addToIndex=True %}
  {% opensubindex nav %}

  {% if namespaceTree.tree %}
    {% with page=namespaceTree %}
      {% create namespaceTree.fileName|append:config.HTML_FILE_EXTENSION from 'htmlnamespaces.tpl' %}
    {% endwith %}
  {% endif %}

  {# write symbol indices for namespace members #}
  {% if namespaceMembersIndex.all %}
    {% with page=namespaceMembersIndex scope='namespace' template='htmlnsmembers.tpl' %}
      {% indexentry nav name=tr.namespaceMembers file=page.fileName anchor='' isReference=False separateIndex=False addToIndex=True %}
      {% include 'htmlmembersindex.tpl' %}
    {% endwith %}
  {% endif %}

  {% closesubindex nav %}
{% endif %}

{# --- concepts --- #}
{% if conceptTree.tree %}
  {% with page=conceptTree %}
    {% create conceptTree.fileName|append:config.HTML_FILE_EXTENSION from 'htmlconcepts.tpl' %}
  {% endwith %}
{% endif %}

{# --- classes --- #}
{% if classList %}
  {% indexentry nav name=tr.classes file='annotated'|append:config.HTML_FILE_EXTENSION anchor='' isReference=False separateIndex=False addToIndex=False %}
  {% opensubindex nav %}

  {# write the annotated class list #}
  {% if classTree.tree %}
    {% with page=classTree %}
      {% create classTree.fileName|append:config.HTML_FILE_EXTENSION from 'htmlannotated.tpl' %}
    {% endwith %}
  {% endif %}

  {# write class index #}
  {% if classIndex.list %}
    {% with page=classIndex %}
      {% create classIndex.fileName|append:config.HTML_FILE_EXTENSION from 'htmlclasses.tpl' %}
    {% endwith %}
  {% endif %}

  {# write the class inheritance hierarchy #}
  {% if classHierarchy.tree and classHierarchy.maxDepth>1 %}
    {% with page=classHierarchy %}
      {% create classHierarchy.fileName|append:config.HTML_FILE_EXTENSION from 'htmlhierarchy.tpl' %}
      {% if config.HAVE_DOT and config.GRAPHICAL_HIERARCHY %}
        {% with fileName='inherits' %}
          {% create fileName|append:config.HTML_FILE_EXTENSION from 'htmlgraphhierarchy.tpl' %}
        {% endwith %}
      {% endif %}
    {% endwith %}
  {% endif %}

  {# write symbol indices for class members #}
  {% if classMembersIndex.all %}
    {% with page=classMembersIndex scope='class' template='htmlclmembers.tpl' %}
      {% indexentry nav name=tr.classMembers file=page.fileName anchor='' isReference=False separateIndex=False addToIndex=True %}
      {% include 'htmlmembersindex.tpl' %}
    {% endwith %}
  {% endif %}

  {% closesubindex nav %}
{% endif %}

{# --- files --- #}
{% if fileList %}
  {% indexentry nav name=tr.files file='files' anchor='' isReference=False separateIndex=False addToIndex=False addToIndex=False %}
  {% opensubindex nav %}

  {# write the directory/file hierarchy #}
  {% if fileTree.tree %}
    {% with page=fileTree %}
      {% create fileTree.fileName|append:config.HTML_FILE_EXTENSION from 'htmlfiles.tpl' %}
    {% endwith %}
  {% endif %}

  {# write symbol indices for global namespace #}
  {% if globalsIndex.all %}
    {% with page=globalsIndex scope='file' template='htmlflmembers.tpl' %}
      {% indexentry nav name=tr.fileMembers file=page.fileName anchor='' isReference=False separateIndex=False addToIndex=True %}
      {% include 'htmlmembersindex.tpl' %}
    {% endwith %}
  {% endif %}

  {% closesubindex nav %}
{% endif %}

{# write directory documentation pages #}
{% for compound in dirList %}
  {% with page=compound %}
    {% create compound.fileName|append:config.HTML_FILE_EXTENSION from 'htmldir.tpl' %}
  {% endwith %}
{% endfor %}

{# close the global navigation index #}
{% closesubindex nav %}

{# write search data #}
{% if config.SEARCHENGINE and not config.SERVER_BASED_SEARCH %}
  {% create 'search/searchdata.js' from 'htmljssearchdata.tpl' %}
  {% for idx in searchIndices %}
    {% for si in idx.symbolIndices %}
      {% with hexCount=forloop.counter0|hex baseName=si.name|append:'_'|append:hexCount %}
        {% create baseName|prepend:'search/'|append:config.HTML_FILE_EXTENSION from 'htmlsearchresult.tpl' %}
        {% create baseName|prepend:'search/'|append:'.js' from 'htmljssearchindex.tpl' %}
      {% endwith %}
    {% endfor %}
  {% endfor %}
{% endif %}

{# write html help index #}
{% if config.GENERATE_HTMLHELP %}
  {% encoding config.CHM_INDEX_ENCODING|default:'CP1250' %}
    {% create 'index.hhp' from 'htmlhelpindexhhp.tpl' %}
    {% create 'index.hhc' from 'htmlhelpindexhhc.tpl' %}
  {% endencoding %}
{% endif %}

{# write the navigation tree data #}
{% if config.GENERATE_TREEVIEW %}
  {% create 'navtreedata.js' from 'htmljsnavtree.tpl' %}
{% endif %}

{# write menu data #}
{% if not config.DISABLE_INDEX %}
  {% create 'menudata.js' from 'htmljsmenudata.tpl' %}
{% endif %}

{% msg %}----- End generating HTML output for {{ config.PROJECT_NAME }} from template ----{% endmsg %}
