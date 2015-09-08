{% extend 'htmlbase.tpl' %}
{% msg %}Generating HTML source code for file {{ compound.name }}{% endmsg %}

{% block navpath %}
  {% if not config.GENERATE_TREEVIEW %}
    {% with navpath=compound.navigationPath %}
      {% include 'htmlnavpath.tpl %}
    {% endwith %}
  {% endif %}
{% endblock %}

{% block title %}
  {# write summary links in the title area #}
  <div class="headertitle"><div class="title">{{ compound.name }}
  {% if compound.version %} ({{ compound.version }}){% endif %}
  </div></div>
{% endblock %}

{% block content %}
<div class="contents">
<div class="textblock">
<a href="{{ page.relPath }}{{ compound.fileName }}{{ config.HTML_FILE_EXTENSION }}">{{ tr.gotoDocumentation }}</a>
</div>
<div class="fragment">
{{ compound.sources }}
</div><!-- fragment -->
</div>
{% endblock %}

