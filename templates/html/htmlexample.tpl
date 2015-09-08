{% extend 'htmlbase.tpl' %}
{% msg %}Generating HTML output for example {{ compound.name }}{% endmsg %}

{% block navpath %}
  {% if not config.GENERATE_TREEVIEW %}
    {% with navpath=compound.navigationPath %}
      {% include 'htmlnavpath.tpl' %}
    {% endwith %}
  {% endif %}
{% endblock %}

{% block content %}
<div class="contents">
{{ compound.details }}
{{ compound.example }}
</div>
{% endblock %}
