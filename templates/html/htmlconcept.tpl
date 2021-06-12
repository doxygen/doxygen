{% extend 'htmlbase.tpl' %}
{% msg %}Generating HTML output for concept {{ compound.name }}{% endmsg %}

{% block navpath %}
{% if not config.GENERATE_TREEVIEW %}
  {% with navpath=compound.navigationPath %}
    {% include 'htmlnavpath.tpl' %}
  {% endwith %}
{% endif %}
{% endblock %}

{% block title %}
  {{ block.super }}
{% endblock %}

{% block content %}
<div class="contents">
{# brief description #}
  {% if compound.brief %}
    <p>{{ compound.brief }}
    {% if compound.hasDetails %}
      <a href="#details">{{ tr.more }}</a>
    {% endif %}
    </p>
  {% endif %}
{# includes #}
  {% if compound.includeInfo %}
    <p>
    {% with ii=compound.includeInfo %}
      {% include 'htmlinclude.tpl' %}
    {% endwith %}
    </p>
  {% endif %}
{# concept definition #}
<h2 class="groupheader">{{ tr.conceptDefinition }}</h2>
<div class="fragment">{{ compound.initializerAsCode }}</div>
{# detailed description #}
{% if compound.hasDetails %}
  {% if compound.anchor %}
    <a name="{{ compound.anchor }}" id="{{ compound.anchor }}"></a>
  {% else %}
    <a name="details" id="details"></a>
  {% endif %}
  <h2 class="groupheader">{{ tr.detailedDesc }}</h2>
  <div class="textblock">
  {# brief description #}
  {% if compound.brief and config.REPEAT_BRIEF %}
    <p>
    {{ compound.brief }}
    </p>
  {% endif %}
  {{ compound.details }}
  </div>
  {# source definition #}
    {% if compound.sourceDef %}
      {% markers obj in compound.sourceDef with tr.definedAtLineInSourceFile %}
        {% with text=obj.text %}
          {% include 'htmlobjlink.tpl' %}
        {% endwith %}
      {% endmarkers %}
    {% endif %}
{% endif %}
</div><!-- contents -->
{% endblock %}

