{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.relatedPagesDesc }}
</div>
{% indexentry nav name=tr.pages file=page.fileName anchor='' isReference=False separateIndex=False addToIndex=True %}
{% opensubindex nav %}
{% with tree=pageTree treeType='Page' %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

