{% if memberListInfo %}
  {% if memberListInfo.members|length>0 %}
    {% if not first %} &#124; {% endif %}
      <a href="#{{ memberListInfo.anchor }}">{{ memberListInfo.title }}</a>
    {% set first=False %}
  {% endif %}
{% endif %}
