{# input: si: SymbolIndex #}
var indexSectionsWithContent =
{
{% set count=0 %}
{% for idx in searchIndices %}
  {% if idx.symbolIndices %}
    {{ count }}:"{% for si in idx.symbolIndices %}{{ si.letter }}{% endfor %}"{%if not forloop.last %},{% endif %}
    {% set count=count+1 %}
  {% endif %}
{% endfor %}
};
var indexSectionNames =
{
{% set count=0 %}
{% for idx in searchIndices %}
  {% if idx.symbolIndices %}
    {{ count }}:"{{ idx.name }}"{% if not forloop.last %},{% endif %}
    {% set count=count+1 %}
  {% endif %}
{% endfor %}
};
var IndexSectionLabels =
{
{% set count=0 %}
{% for idx in searchIndices %}
  {% if idx.symbolIndices %}
    {{ count }}:"{{ idx.text }}"{% if not forloop.last %},{% endif %}
    {% set count=count+1 %}
  {% endif %}
{% endfor %}
};
