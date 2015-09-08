{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.namespaceListDescription }}
</div>
{% indexentry nav name=tr.namespaceList file=page.fileName anchor='' isReference=False %}
{% opensubindex nav %}
{% with tree=namespaceTree %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

