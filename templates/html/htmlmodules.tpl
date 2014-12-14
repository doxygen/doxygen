{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.modulesDescription }}
</div>
{% indexentry nav name=tr.modules file=page.fileName anchor='' %}
{% opensubindex nav %}
{% with tree=moduleTree %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

