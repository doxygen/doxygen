{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
{{ tr.relatedPagesDesc }}
</div>
{% indexentry nav name=tr.pages file=page.fileName anchor='' %}
{% opensubindex nav %}
{% with tree=pageTree %}
  {% include 'htmldirtree.tpl' %}
{% endwith %}
{% closesubindex nav %}
</div><!-- contents -->
{% endblock %}

