var NAVTREE =
[
  [ "{% if mainPage.title %}mainPage.title|jsstring{% else %}{{ tr.mainPage }}{% endif %}",
    "index{{ config.HTML_FILE_EXTENSION }}",
  ]
];

var NAVTREEINDEX =
[
{# write first entry of each sub index #}
{% for entries in navTree.subindices %}
  "{{ entries[0].url }}"{% if not forloop.last %},{% endif %}
{% endfor %}
  ]
];

{# write all sub indices #}
{% for entries in navTree.subindices %}
  {% with idx=forloop.counter0 %}
    {% create idx|prepend:'navtreeindex'|append:'.js' from 'htmlnavindex.tpl' %}
  {% endwith %}
{% endfor %}
