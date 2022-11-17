{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.examplesDescription }}
</div>
{% indexentry nav name=tr.examples file=page.fileName anchor='' isReference=False separateIndex=False addToIndex=True %}
{% opensubindex nav %}
{% with tree=exampleTree treeType='Example' %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

