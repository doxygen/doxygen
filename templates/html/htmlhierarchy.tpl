{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
<p>{{ tr.classHierarchyDescription }}</p>
{% if config.HAVE_DOT and config.GRAPHICAL_HIERARCHY %}
<p><a href="inherits{{ config.HTML_FILE_EXTENSION }}">{{ tr.gotoGraphicalHierarchy }}</a></p>
{% endif %}
</div>
{% indexentry nav name=tr.classHierarchy file=page.fileName anchor='' isReference=False %}
{% opensubindex nav %}
{% with tree=classHierarchy %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div>
{% endblock %}
