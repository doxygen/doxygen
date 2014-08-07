{% extend 'htmlbase.tpl' %}
{% msg %}Generating HTML output for page {{ compound.name }}{% endmsg %}

{% block navpath %}
{% with navpath=compound.navigationPath %}
  {% include 'htmlnavpath.tpl' %}
{% endwith %}
{% endblock %}

{% block content %}
<div class="contents">
{{ compound.details }}
</div>
{% endblock %}
