var {{ varName }} =
[
{% recursetree node.children %}
  {% set dup='' %}
  {% if node.parent %}
    {% if node.parent.file==node.file %}
      {% set dup='_dup' %}
    {% endif %}
  {% endif %}
  {% if node.anchor %}
    {% set varName=node.file|append:'_'|append:node.anchor|append:dup %}
  {% else %}
    {% set varName=node.file|append:dup %}
  {% endif %}
[ "{{ node.name }}",{% if node.file %}"{% if node.isReference %}{{ node.externalReference }}{% endif %}{{ node.file|decodeURL }}{{ config.HTML_FILE_EXTENSION }}{% if node.anchor %}#{{ node.anchor }}{% endif %}"{% else %}null{% endif %},{% if not node.is_leaf_node %}{% if node.separateIndex %}"{{ varName }}"{% else %} [
     {{ children }}
    ]{% endif %}{% else %} null{% endif %} ]{% if not node.last %},{% endif %}
  {% if node.separateIndex %}
     {% create varName|append:'.js' from 'htmljsnavpage.tpl' %}
  {% endif %}
{% endrecursetree %}
];
