{# inputs: page, list #}
{% extend 'htmlbase.tpl' %}

{% block content %}
<div class="contents">
<div class="textblock">
{% if section=='' and letter=='' %}
  {{ tr.classMembersDescription }}
{% endif %}

{% include 'htmlmemberindex.tpl' %}

</div>
</div><!-- contents -->
{% endblock %}

