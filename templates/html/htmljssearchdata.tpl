{# input: si: SymbolIndex #}
var indexSectionsWithContent =
{
{% set count=0 %}
{% for idx in searchIndices %}{% if idx.symbolIndices %}{% if count>0 %},
{% endif %}  {{ count }}:"{% for si in idx.symbolIndices %}{{ si.letter }}{% endfor %}"{% set count=count+1 %}{% endif %}{% endfor %}
};

var indexSectionNames =
{
{% set count=0 %}
{% for idx in searchIndices %}{% if idx.symbolIndices %}{% if count>0 %},
{% endif %}  {{ count }}:"{{ idx.name }}"{% set count=count+1 %}{% endif %}{% endfor %}
};

var indexSectionLabels =
{
{% set count=0 %}
{% for idx in searchIndices %}{% if idx.symbolIndices %}{% if count>0 %},
{% endif %}  {{ count }}:"{{ idx.text }}"{% set count=count+1 %}{% endif %}{% endfor %}
};

