{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.classListDescription }}
</div>
{% indexentry nav name=tr.classes file=page.fileName anchor='' isReference=False %}
{% opensubindex nav %}
{% with tree=classTree %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

