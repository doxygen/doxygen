var NAVTREE =
[
{% recursetree index.nav %}
  [ "{{ node.name }}", {% if node.file %}"{% if node.isReference %}{{ node.externalReference }}{% endif %}{{ node.file }}{{ config.HTML_FILE_EXTENSION }}{% if node.anchor %}#{{ node.anchor }}{% endif %}"{% else %}null{% endif %},{% if not node.is_leaf_node %} [
     {{ children }}
  ]{% else %} null{% endif %} ]{% if not node.last %},{% endif %}
{% endrecursetree %}
];

var NAVTREEINDEX =
[
{% with navlist=index.nav|flatten|listsort:config.HTML_FILE_EXTENSION|prepend:'{{file}}'|append:'#{{anchor}}' navpages=navlist|paginate:250 %}
  {% for page in navpages %}
    "{{ page.0.file }}{{ config.HTML_FILE_EXTENSION }}{% if page.0.anchor %}#{{ page.0.anchor }}{% endif %}"{% if not forloop.last %},{%endif %}
    {% with idx=forloop.counter0 entries=page %}
      {% create forloop.counter0|prepend:'navtreeindex'|append:'.js' from 'htmljsnavindex.tpl' %}
    {% endwith %}
  {% endfor %}
{% endwith %}
];

var SYNCONMSG  = '{{ tr.panelSyncOn }}';
var SYNCOFFMSG = '{{ tr.panelSyncOff }}';
