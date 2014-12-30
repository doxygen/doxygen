{# input: si symbolIndex #}
var searchData =
[
{% for group in si.symbolGroups %}['{{ group.id }}',['{{ group.name }}',
{% for sym in group.symbols %}
{% spaceless %}
['{{ sym.relPath }}{{ sym.fileName }}{{ config.HTML_FILE_EXTENSION }}{% if sym.anchor %}#{{ sym.anchor }}{% endif %}',
{% if not config.EXT_LINKS_IN_WINDOW %}1{% else %}0{% endif %},
'{{ sym.scope|nowrap }}']
{% endspaceless %}
{% if not forloop.last %},{% endif %}
{% endfor %}
]]{% if not forloop.last %},{% endif %}
{% endfor %}
];
