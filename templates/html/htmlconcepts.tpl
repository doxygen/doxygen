{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.conceptListDescription }}
</div>
{% indexentry nav name=tr.concepts file=page.fileName anchor='' isReference=False separateIndex=True addToIndex=True %}
{% opensubindex nav %}
{% with tree=conceptTree %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

