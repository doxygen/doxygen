{# inputs: page, list, section, text #}
{text:"{{ text }}",url:"{{ page.fileName }}{{ section }}{{ config.HTML_FILE_EXTENSION }}"
{% if list|length>maxItemsForFlatList %}
,children:[
  {% with index=list|alphaIndex:'name' %}
    {% for sect in index %}
      {% spaceless %}
      {text:"{{ sect.letter }}",url:"
      {% if list|length<=maxItemsForMultiPageList %}
      {{ page.fileName }}{{ section }}{{ config.HTML_FILE_EXTENSION }}#index_{{ sect.label }}"
      {% else %}
      {{ page.fileName }}{{ section }}_{{ sect.label }}{{ config.HTML_FILE_EXTENSION }}"
      {% endif %}
      }
      {% endspaceless %}
      {% if not forloop.last %},{% endif %}
    {% endfor %}
  {% endwith %}
]
{% endif %}
}
