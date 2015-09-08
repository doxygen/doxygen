{% extend 'htmlbase.tpl' %}
{% msg %}Generating HTML output for module {{ compound.name }}{% endmsg %}

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
    {% if compound.modules %}
      <a href="#modules">{{ tr.modules }}</a>
      {% set first=False %}
    {% endif %}
    {% if compound.dirs %}
      <a href="#dirs">{{ tr.dirs }}</a>
      {% set first=False %}
    {% endif %}
    {% if compound.files %}
      <a href="#files">{{ tr.files }}</a>
      {% set first=False %}
    {% endif %}
    {% if compound.classes %}
      <a href="#classes">{{ tr.classes }}</a>
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
    {% with memberListInfo=compound.macros %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.typedefs %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.enums %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.enumvalues %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.functions %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.variables %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.signals %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.publicSlots %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.protectedSlots %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.privateSlots %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.events %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.properties %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.friends %}
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
{# group graph #}
  {% if compound.hasGroupGraph %}
    {% with obj=compound %}
      {% include 'htmldynheader.tpl' %}
    {% endwith %}
      {{ tr.collaborationDiagramFor:compound.name }}
    </div>
    {% with obj=compound %}
      {% include 'htmldyncontents.tpl' %}
    {% endwith %}
      {{ compound.groupGraph }}
    </div>
  {% endif %}
{# member declarations #}
  {# modules #}
    {% with list=compound.modules label='modules' title=tr.modules local=False %}
      {% include 'htmldeclcomp.tpl' %}
    {% endwith %}
  {# dirs #}
    {% with list=compound.dirs, label='dirs' title=tr.directories local=False %}
      {% include 'htmldeclcomp.tpl' %}
    {% endwith %}
  {# files #}
    {% with list=compound.files, label='files' title=tr.files local=False %}
      {% include 'htmldeclcomp.tpl' %}
    {% endwith %}
  {# namespaces #}
    {% with list=compound.namespaces, label='namespaces' title=tr.namespaces local=False %}
      {% include 'htmldeclcomp.tpl' %}
    {% endwith %}
  {# classes #}
    {% with list=compound.classes label='classes' title=tr.classes local=False %}
      {% include 'htmldeclcomp.tpl' %}
    {% endwith %}
  {# constantgroups #}
    {% with list=compound.constantgroups, label='constantgroups' title=tr.constantgroups local=False %}
      {% include 'htmldeclcomp.tpl' %}
    {% endwith %}
  {# defines #}
    {% with memberListInfo=compound.macros %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# typedefs #}
    {% with memberListInfo=compound.typedefs %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# enums #}
    {% with memberListInfo=compound.enums %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# enum values #}
    {% with memberListInfo=compound.enumvalues %}
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
  {# signals #}
    {% with memberListInfo=compound.signals %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# public slots #}
    {% with memberListInfo=compound.publicSlots %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# protected slots #}
    {% with memberListInfo=compound.protectedSlots %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# private slots #}
    {% with memberListInfo=compound.privateSlots %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# private events #}
    {% with memberListInfo=compound.events %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# private properties #}
    {% with memberListInfo=compound.properties %}
      {% include 'htmlmemdecls.tpl' %}
    {% endwith %}
  {# private friends #}
    {% with memberListInfo=compound.friends %}
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
  {# defines #}
    {% with memberListInfo=compound.detailedMacros %}
      {% include 'htmlmemdef.tpl' %}
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

