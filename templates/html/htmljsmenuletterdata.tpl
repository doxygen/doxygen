{# inputs: page, list, section, text #}
{text:'{{ text }}',url:'{{ page.fileName }}{{ section }}{{ config.HTML_FILE_EXTENSION }}'
{% if list|length>maxItemsForMultiPageList %}
,children:[
  {% with index=list|alphaIndex:'name' %}
    {% for sect in index %}
      {text:'{{ sect.letter }}',url:'{{ page.fileName }}{{ section }}_{{ sect.label }}{{ config.HTML_FILE_EXTENSION }}'}{% if not forloop.last %},{% endif %}
    {% endfor %}
  {% endwith %}
]
{% endif %}
}
