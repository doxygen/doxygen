{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.examplesDescription }}
</div>
{% indexentry nav name=tr.examples file=page.fileName anchor='' isReference=False %}
{% opensubindex nav %}
{% with tree=exampleTree %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

