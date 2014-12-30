{# input: memberListInfo #}
{% if memberListInfo %}
  {% if memberListInfo.members|length>0 or memberListInfo.memberGroups|length>0 %}
    {% for member in memberListInfo.members %}
      {% include 'htmlmemdecl.tpl' %}
    {% endfor %}
    {% for memgroup in memberListInfo.memberGroups %}
      {% with memberListInfo=memgroup inheritId='' %}
        {% for member in memberListInfo.members %}
          {% include 'htmlmemdecl.tpl' %}
        {% endfor %}
      {% endwith %}
    {% endfor %}
  {% endif %}
{% endif %}
