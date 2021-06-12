{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.classListDescription }}
</div>
{% indexentry nav name=tr.classList file=page.fileName anchor='' isReference=False separateIndex=True addToIndex=True %}
{% opensubindex nav %}
{% with tree=classTree %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

