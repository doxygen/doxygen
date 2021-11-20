{% for group in groups %}
  {% if group.partOfGroups %}
    {% with groups=group.partOfGroups %}
      {% include 'htmlparentgroup.tpl' %}
    {% endwith %}
  {% endif %}
  {% with obj=group text=group.title %}
    {% include 'htmlobjlink.tpl' %}
  {% endwith %}
  {% if not forloop.last %} &#124; {% endif %}
{% endfor %}
&raquo;
