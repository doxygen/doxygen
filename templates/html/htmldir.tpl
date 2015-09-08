{% extend 'htmlbase.tpl' %}
{% msg %}Generating HTML output for directory {{ compound.name }}{% endmsg %}

{% block navpath %}
  {% if not config.GENERATE_TREEVIEW %}
    {% with navpath=compound.navigationPath %}
      {% include 'htmlnavpath.tpl' %}
    {% endwith %}
  {% endif %}
{% endblock %}

{% block title %}
  {# write summary links in the title area #}
  <div class="summary">
  {% with first=True %}
    {% if compound.dirs %}
      <a href="#subdirs">{{ tr.directories }}</a>
      {% set first=False %}
    {% endif %}
    {% if compound.files %}
      {% if not first %} &#124; {% endif %}
      <a href="#files">{{ tr.files }}</a>
      {% set first=False %}
    {% endif %}
  {% endwith %}
  </div>
  {{ block.super }}
{% endblock %}

{% block content %}
<div class="contents">
{# brief description #}
  {% if compound.brief %}
    {{ compound.brief }}
    {% if compound.hasDetails %}
      <a href="#details">{{ tr.more }}</a>
    {% endif %}
  {% endif %}
{# dir graph #}
  {% if compound.hasDirGraph %}
    {% with obj=compound %}
      {% include 'htmldynheader.tpl' %}
    {% endwith %}
      {{ tr.dirDependencyGraphFor:compound.dirName }}
    </div>
    {% with obj=compound %}
      {% include 'htmldyncontents.tpl' %}
    {% endwith %}
      {{ compound.dirGraph }}
    </div>
  {% endif %}
{# member declarations #}
  {# directories #}
    {% with list=compound.dirs label='subdirs' title=tr.directories local=False %}
      {% include 'htmldeclcomp.tpl' %}
    {% endwith %}
  {# files #}
    {% with list=compound.files, label='files' title=tr.files local=False %}
      {% include 'htmldeclcomp.tpl' %}
    {% endwith %}
{# end member declarations #}
{# detailed description #}
{% if compound.hasDetails %}
  {# anchor #}
  <a name="details" id="details"></a>
  {# header #}
  <h2 class="groupheader">{{ tr.detailedDesc }}</h2>
  <div class="textblock">
  {# brief #}
  {% if compound.brief and config.REPEAT_BRIEF %}
    <p>
    {{ compound.brief }}
    </p>
  {% endif %}
  {# details #}
  {{ compound.details }}
  </div>
{% endif %}
</div>
{% endblock %}

