{# input compound #}
{% if compound.partOfGroups %}
<div class="ingroups">
  {% for group in compound.partOfGroups %}
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
</div>
{% endif %}
