{% extend 'htmlbase.tpl' %}
{% msg %}Generating HTML source code for file {{ compound.name }}{% endmsg %}

{% block navpath %}
  {% if compound.navigationPath %}
    <div id="nav-path" class="navpath">
      <ul>
      {% for obj in compound.navigationPath %}
        <li class="navelem">
          {% with text=obj.text %}
            {% include 'htmlobjlink.tpl' %}
          {% endwith %}
        </li>
      {% endfor %}
      </ul>
    </div>
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

