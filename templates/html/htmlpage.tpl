{% extend 'htmlbase.tpl' %}
{% msg %}Generating HTML output for page {{ compound.name }}{% endmsg %}

{% block navpath %}
  {% if not config.GENERATE_TREEVIEW %}
    {% with navpath=compound.navigationPath %}
      {% include 'htmlnavpath.tpl' %}
    {% endwith %}
  {% endif %}
{% endblock %}

{% block content %}
<div class="contents">
{% if isMainPage and config.DISABLE_INDEX %}
{# no other navigation means, so we produce the links on the main page #}
<ul>
{% if pageTree.tree %}
  <li><a href="{{ page.relPath }}pages{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.pages }}</span></a></li>
{% endif %}
{% if moduleTree.tree %}
  <li><a href="{{ page.relPath }}modules{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.modules }}</span></a></li>
{% endif %}
{% if namespaceList %}
  <li><a href="{{ page.relPath }}namespaces{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.namespaces }}</span></a>
  <ul>
    <li><a href="{{ page.relPath }}namespaces{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.namespaceList }} </span></a></li>
    <li><a href="{{ page.relPath }}namespacemembers{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.namespaceMembers }}</span></a></li>
  </ul>
  </li>
{% endif %}
{% if classList %}
  <li><a href="{{ page.relPath }}annotated{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.classes }}</span></a>
  <ul>
    <li><a href="{{ page.relPath }}annotated{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.classList }}</span></a></li>
    <li><a href="{{ page.relPath }}classes{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.classIndex }}</span></a></li>
    {% if classHierarchy.tree %}
    <li><a href="{{ page.relPath }}hierarchy{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.classHierarchy }} </span></a></li>
    {% endif %}
    {% if classMembersIndex.all %}
    <li><a href="{{ page.relPath }}functions{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.classMembers }} </span></a></li>
    {% endif %}
  </ul>
  </li>
{% endif %}
{% if fileList %}
  <li><a href="{{ page.relPath }}files{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.files }}</span></a>
  <ul>
    <li><a href="{{ page.relPath }}files{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.fileList }} </span></a></li>
    <li><a href="{{ page.relPath }}globals{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.fileMembers }} </span></a></li>
  </ul>
  </li>
{% endif %}
{% if exampleList.items %}
  <li><a href="{{ page.relPath }}examples{{ config.HTML_FILE_EXTENSION }}"><span>{{ tr.examples }}</span></a>
{% endif %}
</ul>
{% endif %}
{{ compound.details }}
</div>
{% endblock %}
