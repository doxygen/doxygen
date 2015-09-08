{% extend 'htmlbase.tpl' %}
{% msg %}Generating HTML output for class {{ compound.name }}{% endmsg %}

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
    {% if compound.allMembersList %}
      {% if not first %} &#124; {% endif %}
      <a href="{{ compound.allMembersFileName }}{{ config.HTML_FILE_EXTENSION }}#all-members-list">{{ tr.listOfAllMembers }}</a>
      {% set first=False %}
    {% endif %}
    {% with memberListInfo=compound.publicTypes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.unoIDLServices %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.unoIDLInterfaces %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.publicSlots %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.signals %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.publicMethods %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.publicStaticMethods %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.publicAttributes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.publicStaticAttributes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.protectedTypes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.protectedSlots %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.protectedMethods %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.protectedStaticMethods %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.protectedAttributes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.protectedStaticAttributes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.packageTypes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.packageMethods %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.packageStaticMethods %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.packageAttributes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.packageStaticAttributes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.properties %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.events %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.privateTypes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.privateSlots %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.privateMethods %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.privateStaticMethods %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.privateAttributes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.privateStaticAttributes %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.friends %}
      {% include 'htmlmemsummary.tpl' %}
    {% endwith %}
    {% with memberListInfo=compound.related %}
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
{# includes #}
  {% if compound.includeInfo %}
    <div class="textblock">
    {% with ii=compound.includeInfo %}
      {% include 'htmlinclude.tpl' %}
    {% endwith %}
    </div>
  {% endif %}
{# inheritancegraph #}
  {% if compound.hasInheritanceDiagram %}
    {% with obj=compound %}
      {% include 'htmldynheader.tpl' %}
    {% endwith %}
      {{ tr.inheritanceDiagramFor:compound.name }}
    </div>
    {% with obj=compound %}
      {% include 'htmldyncontents.tpl' %}
    {% endwith %}
      {{ compound.inheritanceDiagram }}
    </div>
    {# TODO: legend #}
  {% else %}
    {# textual inheritance list #}
    {% if compound.inherits|length>0 %}
      <p>
      {% markers c in compound.inherits with tr.inheritsList:compound.inherits|length %}
        {% with obj=c.class text=c.name %}
          {% include 'htmlobjlink.tpl' %}
        {% endwith %}
      {% endmarkers %}
      </p>
    {% endif %}
    {% if compound.inheritedBy|length>0 %}
      <p>
      {% markers c in compound.inheritedBy with tr.inheritedByList:compound.inheritedBy|length %}
        {% with obj=c.class text=c.name %}
          {% include 'htmlobjlink.tpl' %}
        {% endwith %}
      {% endmarkers %}
      </p>
    {% endif %}
  {% endif %}
{# collaborationgraph #}
  {% if compound.hasCollaborationDiagram %}
    {% with obj=compound %}
      {% include 'htmldynheader.tpl' %}
    {% endwith %}
      {{ tr.collaborationDiagramFor:compound.name }}
    </div>
    {% with obj=compound %}
      {% include 'htmldyncontents.tpl' %}
    {% endwith %}
      {{ compound.collaborationDiagram }}
    </div>
  {% endif %}
{# memberdecls #}
    {# nestedClasses #}
      {% with list=compound.classes label='nested-classes' title=tr.classes local=1 %}
        {% include 'htmldeclcomp.tpl' %}
      {% endwith %}
    {# publicTypes #}
      {% with memberListInfo=compound.publicTypes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# services #}
      {% with memberListInfo=compound.unoIDLServices %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# interfaces #}
      {% with memberListInfo=compound.unoIDLInterfaces %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# publicSlots #}
      {% with memberListInfo=compound.publicSlots %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# signals #}
      {% with memberListInfo=compound.signals %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# publicMethods #}
      {% with memberListInfo=compound.publicMethods %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# publicStaticMethods #}
      {% with memberListInfo=compound.publicStaticMethods %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# publicAttributes #}
      {% with memberListInfo=compound.publicAttributes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# publicStaticAttributes #}
      {% with memberListInfo=compound.publicStaticAttributes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# protectedtypes #}
      {% with memberListInfo=compound.protectedTypes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# protectedslots #}
      {% with memberListInfo=compound.protectedSlots %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# protectedmethods #}
      {% with memberListInfo=compound.protectedMethods %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# protectedstaticmethods #}
      {% with memberListInfo=compound.protectedStaticMethods %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# protectedattributes #}
      {% with memberListInfo=compound.protectedAttributes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# protectedstaticattributes #}
      {% with memberListInfo=compound.protectedStaticAttributes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# packagetypes #}
      {% with memberListInfo=compound.packageTypes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# packagemethods #}
      {% with memberListInfo=compound.packageMethods %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# packagestaticmethods #}
      {% with memberListInfo=compound.packageStaticMethods %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# packageattributes #}
      {% with memberListInfo=compound.packageAttributes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# packagestaticattributes #}
      {% with memberListInfo=compound.packageStaticAttributes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# properties #}
      {% with memberListInfo=compound.properties %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# events #}
      {% with memberListInfo=compound.events %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# privatetypes #}
      {% with memberListInfo=compound.privateTypes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# privateslots #}
      {% with memberListInfo=compound.privateSlots %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# privatemethods #}
      {% with memberListInfo=compound.privateMethods %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# privatestaticmethods #}
      {% with memberListInfo=compound.privateStaticMethods %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# privateattributes #}
      {% with memberListInfo=compound.privateAttributes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# privatestaticattributes #}
      {% with memberListInfo=compound.privateStaticAttributes %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# friends #}
      {% with memberListInfo=compound.friends %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# related #}
      {% with memberListInfo=compound.related %}
        {% include 'htmlmemdecls.tpl' %}
      {% endwith %}
    {# member groups #}
      {% if compound.memberGroups %}
        {% for memberListInfo in compound.memberGroups %}
          {% include 'htmlmemdecls.tpl' %}
        {% endfor %}
      {% endif %}
    {# additionalInheritedMembers #}
      {% if compound.additionalInheritedMembers %}
        <table class="memberdecls">
        <tr class="heading"><td colspan="2"><h2 class="groupheader">
        <a name="inherited"></a>{{ tr.additionalInheritedMembers }}
        </h2></td></tr>
        {# write additional inherited members #}
        {% for info in compound.additionalInheritedMembers %}
          {% include 'htmlmeminherit.tpl' %}
        {% endfor %}
        </table>
      {% endif %}
{# detailed description #}
{% if compound.hasDetails %}
  {% if compound.anchor %}
    <a name="{{ compound.anchor }}" id="{{ compound.anchor }}"></a>
  {% else %}
    <a name="details" id="details"></a>
  {% endif %}
  <h2 class="groupheader">{{ tr.detailedDesc }}</h2>
  <div class="textblock">
  {# template specifier #}
  {% if compound.language=='cpp' and compound.templateDecls %}
    <h3>{% spaceless %}
      {% for targList in compound.templateDecls %}
        template&lt;
        {% for targ in targList %}
          {{ targ.type }}{% if targ.name %} {{ targ.name }}{% endif %}{% if targ.defVal %}&#160;= {{ targ.defVal }}{% endif %}{% if not forloop.last %}, {% endif %}
        {% endfor %}
        &gt;<br/>
      {% endfor %}
    {% endspaceless %}
    {{ compound.compoundType }}&#160;{{ compound.name }}
    </h3>
  {% endif %}
  {# brief description #}
  {% if compound.brief and config.REPEAT_BRIEF %}
    <p>
    {{ compound.brief }}
    </p>
  {% endif %}
  {{ compound.details }}
  </div>
  {# type constraints #}
    {% with obj=compound %}
      {% include 'htmltypeconstraints.tpl' %}
    {% endwith %}
  {# examples #}
    {% if compound.examples %}
      <dl><dt><b>{{ tr.examples }}</b><dd>
      {% markers obj in compound.examples with tr.exampleList:compound.examples|length %}
        {% with text=obj.text %}
          {% include 'htmlobjlink.tpl' %}
        {% endwith %}
      {% endmarkers %}
      </dd></dl>
    {% endif %}
  {# source definition #}
    {% if compound.sourceDef %}
      {% markers obj in compound.sourceDef with tr.definedAtLineInSourceFile %}
        {% with text=obj.text %}
          {% include 'htmlobjlink.tpl' %}
        {% endwith %}
      {% endmarkers %}
    {% endif %}
{% endif %}
{# member definitions #}
  {# inline classes #}
    {% with classList=compound.innerClasses %}
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
  {# services #}
    {% with memberListInfo=compound.detailedServices %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
  {# interfaces #}
    {% with memberListInfo=compound.detailedInterfaces %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
  {# constructors #}
    {% with memberListInfo=compound.detailedConstructors %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
  {# functions #}
    {% with memberListInfo=compound.detailedMethods %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
  {# related #}
    {% with memberListInfo=compound.detailedRelated %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
  {# variables #}
    {% with memberListInfo=compound.detailedVariables %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
  {# properties #}
    {% with memberListInfo=compound.detailedProperties %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
  {# events #}
    {% with memberListInfo=compound.detailedEvents %}
      {% include 'htmlmemdef.tpl' %}
    {% endwith %}
{# used files #}
  {% if config.SHOW_USED_FILES %}
    <hr/>
    {{ compound.generatedFromFiles }}
    <ul>
    {% for file in compound.usedFiles %}
      <li>{% if file.sourceFileName %}
            <a class="el" href="{{ file.sourceFileName }}{{ config.HTML_FILE_EXTENSION }}">
          {% endif %}
          {% if not file.sourceFileName and file.isLinkable %}
            <a class="el" href="{{ file.fileName }}{{ config.HTML_FILE_EXTENSION }}">
          {% endif %}
          {% if config.FULL_PATH_NAMES %}
            {{ file.name }}
          {% else %}
            {{ file.name|stripPath }}
          {% endif %}
          {% if file.sourceFileName or file.isLinkable %}
            </a>
          {% endif %}
          {% if file.versionInfo %} {{ file.versionInfo }}{% endif %}
      </li>
    {% endfor %}
    </ul>
  {% endif %}
</div>
{% endblock %}

