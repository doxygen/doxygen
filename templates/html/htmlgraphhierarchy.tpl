{% extend 'htmlbase.tpl' %}
{% block content %}
<div class="contents">
<div class="textblock">
<p><a href="hierarchy{{ config.HTML_FILE_EXTENSION }}">{{ tr.gotoTextualHierarchy }}</a></p>
</div>
<table border="0" cellspacing="10" cellpadding="0">
{% for d in classHierarchy.diagrams %}
<tr><td>{{ d.graph }}</td></tr>
{% endfor %}
</table>
</div>
{% endblock %}
