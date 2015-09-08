{# inputs: obj (with .isLinkable .isReference .anchor .fileName .externalReference), text, config, page.relPath #}
{% if obj.isLinkable %}
{% if obj.isReference %}
<a class="elRef" href="{{ obj.externalReference }}{{ obj.fileName }}{{ config.HTML_FILE_EXTENSION }}{% if obj.anchor %}#{{ obj.anchor }}{% endif %}">{{ text }}</a>
{% else %}
<a class="el" href="{{ page.relPath }}{{ obj.fileName }}{{ config.HTML_FILE_EXTENSION }}{% if obj.anchor %}#{{ obj.anchor }}{% endif %}">{{ text }}</a>
{% endif %}
{% else %}
<b>{{ text }}</b>
{% endif %}
