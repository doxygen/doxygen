{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.namespaceListDescription }}
</div>
{% indexentry nav name=tr.namespaceList file=page.fileName anchor='' isReference=False separateIndex=True addToIndex=True %}
{% opensubindex nav %}
{% with tree=namespaceTree treeType='Namespace' %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

