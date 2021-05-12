{# input: si symbolIndex #}
var searchData =
[
{% for group in si.symbolGroups %}  ['{{ group.id }}_{{ symbolCount }}',['{{ group.name }}',{% spaceless %}{% for sym in group.symbols %}['{{ sym.relPath }}{{ sym.fileName }}{{ config.HTML_FILE_EXTENSION }}{% if sym.anchor %}#{{ sym.anchor }}{% endif %}',
{% if not config.EXT_LINKS_IN_WINDOW %}1{% else %}0{% endif %},
'{{ sym.scope|nowrap|e }}']
{% if not forloop.last %},{% endif %}
{% endfor %}
{% update symbolCount=symbolCount+1 %}
{% endspaceless %}
]]{% if not forloop.last %},{% endif %}
{% endfor %}
];
