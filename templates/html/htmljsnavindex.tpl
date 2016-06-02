{# input idx, entries #}
var NAVTREEINDEX{{ idx }} =
{
{% for entry in entries %}
  "{{ entry.file|decodeURL }}{{ config.HTML_FILE_EXTENSION }}{% if entry.anchor %}#{{ entry.anchor }}{% endif %}":[{% for e in entry.path %}{% if not forloop.first %}{{ e.index }}{% if not forloop.last%},{% endif %}{% endif %}{% endfor %}]{% if not forloop.last %},{%endif %}
{% endfor %}
};
