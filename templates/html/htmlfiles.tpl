{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.fileListDescription }}
</div>
{% indexentry nav name=tr.fileList file=page.fileName anchor='' isReference=False %}
{% opensubindex nav %}
{% with tree=fileTree %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

