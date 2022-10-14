{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.modulesDescription }}
</div>
{% indexentry nav name=tr.modules file=page.fileName anchor='' isReference=False separateIndex=True addToIndex=True %}
{% opensubindex nav %}
{% with tree=moduleTree treeType='Module' %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

