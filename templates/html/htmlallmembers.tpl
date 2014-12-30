{% extend 'htmlbase.tpl' %}

{% block title %}
  <div class="headertitle"><div class="title">{{ compound.name }} {{ tr.memberList }}</div></div>
{% endblock %}

{% block content %}
<div class="contents">
<p>{{ tr.theListOfAllMembers }} <a class="el" href="{{ compound.fileName }}{{ config.HTML_FILE_EXTENSION }}">{{ compound.name }}</a>{{ tr.incInheritedMembers }}</p>
<table class="directory">
{% for mi in compound.allMembersList %}
  <tr class="{% cycle 'even' 'odd' %}">
  {# TODO: objective-C #}
  <td class="entry">{% with obj=mi.member text=mi.ambiguityScope|append:mi.member.name %}
      {% include 'htmlobjlink.tpl' %}
      {% endwith %}
    {# TODO: add arguments #}
    {# TODO: add column with scope #}
  </td>
  </tr>
{% endfor %}
</table>
</div>
{% endblock %}
