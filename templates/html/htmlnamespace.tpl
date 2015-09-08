{% extend 'htmlbase.tpl' %}
{% msg %}Generating HTML output for namespace {{ compound.name }}{% endmsg %}

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
    {% if compound.classes %}
      <a href="#nested-classes">{{ tr.classes }}</a>
      {% set first=False %}
    {% endif %}
    {% if compound.namespaces %}
      {% if not first %} &#124; {% endif %}
      <a href="#namespaces">{{ tr.namespaces }}</a>
      {% set first=False %}
    {% endif %}
    {% if compound.constantgroups %}
      {% if not first %} &#124; {% endif %}
      <a href="#constantgroups">{{ tr.constantgroups }}</a>
      {% set first=False %}
    {% endif %}
    {% with memberListInfo=compound.typedefs %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.enums %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.functions %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.variables %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
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
{# member declarations #}
  {# classes #}
    {% with list=compound.classes label='nested-classes' title=tr.classes local=False %}
      {% include 'htmldeclcomp.tpl' %}
    {% endwith %}
  {# namespaces #}
    {% with list=compound.namespaces, label='namespaces' title=tr.namespaces local=False %}
      {% include 'htmldeclcomp.tpl' %}
    {% endwith %}
  {# constantgroups #}
    {% with list=compound.constantgroups, label='constantgroups' title=tr.constantgroups local=False %}
      {% include 'htmldeclcomp.tpl' %}
    {% endwith %}
  {# typedefs #}
    {% with memberListInfo=compound.typedefs %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# enums #}
    {% with memberListInfo=compound.enums %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# functions #}
    {% with memberListInfo=compound.functions %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# variables #}
    {% with memberListInfo=compound.variables %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# membergroups #}
    {% if compound.memberGroups %}
      {% for memberListInfo in compound.memberGroups %}
        {% include 'htmlmemdecls.tpl' %}
      {% endfor %}
    {% endif %}
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
  {# source definition #}
  {% if compound.sourceDef %}
    {% markers obj in compound.sourceDef with tr.definedAtLineInSourceFile %}
      {% with text=obj.text %}
        {% include 'htmlobjlink.tpl' %}
      {% endwith %}
    {% endmarkers %}
  {% endif %}
  </div>
{% endif %}
{# member definitions #}
  {# inline classes #}
    {% with classList=compound.inlineClasses %}
      {% include 'htmlinlineclasses.tpl' %}
    {% endwith %}
  {# typedefs #}
    {% with memberListInfo=compound.detailedTypedefs %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
  {# enums #}
    {% with memberListInfo=compound.detailedEnums %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
  {# functions #}
    {% with memberListInfo=compound.detailedFunctions %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
  {# variables #}
    {% with memberListInfo=compound.detailedVariables %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
{# end member definitions #}
</div>
{% endblock %}

