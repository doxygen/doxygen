{# inputs: obj (with .isLinkable .anchor .fileName), text, config, page.relPath #}
{% if obj.isLinkable %}
<a class="el" href="{{ page.relPath }}{{ obj.fileName }}{{ config.HTML_FILE_EXTENSION }}{% if obj.anchor %}#{{ obj.anchor }}{% endif %}">{{ text }}</a>
{% else %}
<b>{{ text }}</b>
{% endif %}
